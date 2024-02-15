#include "mount.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

void creatDirIfNotexist(char *path,unsigned int mode){
    struct stat st;

    if(stat(path, &st) == -1){
        if(mkdir(path, mode) == -1){
            fprintf(stderr, "Error creating directory %s: %s\n", path, strerror(errno));
        }
    }
}

int mount_root(struct mount_point *mp,dev_t console,int enable_x11){
    mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL);//这可以避免在host中同时挂载

    //获取一个随机的uuid
    int random_uuid_fd = open("/proc/sys/kernel/random/uuid", O_RDONLY);
    char uuid[40];
    read(random_uuid_fd,uuid,36);
    uuid[36]='\0';
    close(random_uuid_fd);

    //容器rootfs在host中的真实位置
    char real_root_dir[512]={};
    sscanf(real_root_dir,"/tmp/kasora_container_%s/", uuid);
    mkdir(real_root_dir, 0666);

    //挂载所有要挂载的路径
    while(mp != NULL){
        char contaniner_path[1024];
        if(mp->container_path[0] != '/')//消除多于的/
            sscanf(contaniner_path, "%s%s", real_root_dir,mp->container_path);
        else
            sscanf(contaniner_path, "%s%s", real_root_dir,mp->container_path + 1);
        unsigned long mount_flag = 0;
        if(mp->attr & MOUNT_ATTR_NODEV){
            mount_flag |= MS_NODEV;
        }
        if(mp->attr & MOUNT_ATTR_NOEXEC){
            mount_flag |= MS_NOEXEC;
        }
        if(mp->attr & MOUNT_ATTR_NOSUID){
            mount_flag |= MS_NOSUID;
        }
        if(mp->attr & MOUNT_ATTR_TMPFS){//挂载的是tmpfs
            char tmp[128] = {};
            if(mp->tmpfs_size){//大小
                sscanf(tmp, "size=%s", mp->tmpfs_size);
            }
            creatDirIfNotexist(contaniner_path, 0666);
            mount("tmpfs",contaniner_path,"tmpfs",mount_flag,tmp);
        }
        else{
            creatDirIfNotexist(contaniner_path, 0666);
            mount(mp->host_path,contaniner_path,NULL,MS_BIND | MS_PRIVATE | mount_flag,NULL);
        }
        mp++;
    }

    //挂载一些tmpfs和/proc
    char tmp[256];
    sscanf(tmp,"%s%s",real_root_dir,"proc");
    creatDirIfNotexist(tmp, 0666);
    mount("proc",tmp,"proc",0,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"proc/sys");//systemd要求该目录必须只读
    //如果ipc,uts,user,pid ns均开启可以不只读
    mount("/proc/sys",tmp,NULL,MS_BIND | MS_PRIVATE | MS_RDONLY,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"tmp");
    creatDirIfNotexist(tmp, 0666);
    mount("tmpfs",tmp,"tmpfs",0,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"run");
    creatDirIfNotexist(tmp, 0666);
    mount("tmpfs",tmp,"tmpfs",0,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"run/lock");
    creatDirIfNotexist(tmp, 0666);
    mount("tmpfs",tmp,"tmpfs",0,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"proc/sys/net");//开启net ns的情况下这个目录可写
    mount("/proc/sys/net",tmp,NULL,MS_BIND | MS_PRIVATE,NULL);

    //将x11 socket映射到容器内
    if(enable_x11){
        sscanf(tmp,"%s%s",real_root_dir,"tmp/.X11-unix");
        creatDirIfNotexist(tmp, 0777);
        mount("/tmp/.X11-unix",tmp,NULL,MS_BIND | MS_PRIVATE, NULL);
    }

    //挂载/sys
    sscanf(tmp,"%s%s",real_root_dir,"sys");
    creatDirIfNotexist(tmp, 0666);
    mount("sys",tmp,"sysfs",MS_NOEXEC|MS_NOSUID|MS_RDONLY|MS_NODEV,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"sys/fs/cgroup");
    mount("/sys/fs/cgroup",tmp,NULL,MS_BIND | MS_PRIVATE,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"sys/fs/fuse");
    mount("/sys/fs/fuse",tmp,NULL,MS_BIND | MS_PRIVATE,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"sys/fs/selinux");//只读,如果存在selinux
    mount("/sys/fs/selinux",tmp,NULL,MS_BIND | MS_PRIVATE | MS_RDONLY,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"sys/frimware");//该目录不可见
    mount("tmpfs",tmp,"tmpfs",MS_PRIVATE | MS_RDONLY,NULL);
    sscanf(tmp,"%s%s",real_root_dir,"sys/kernel");//该目录不可见
    mount("tmpfs",tmp,"tmpfs",MS_PRIVATE | MS_RDONLY,NULL);

    //处理/dev
    sscanf(tmp,"%s%s",real_root_dir,"dev");
    creatDirIfNotexist(tmp, 0666);
    sscanf(tmp,"%s%s",real_root_dir,"dev/console");
    mknod(tmp,S_IFCHR |0600, console);
    sscanf(tmp,"%s%s",real_root_dir,"dev/tty1");
	mknod(tmp,S_IFCHR |0600, console);
    sscanf(tmp,"%s%s",real_root_dir,"dev/null");
    mknod(tmp,S_IFCHR | 0666,makedev(1,3));
    sscanf(tmp,"%s%s",real_root_dir,"dev/zero");
	mknod(tmp,S_IFCHR | 0666,makedev(1,5));
    sscanf(tmp,"%s%s",real_root_dir,"dev/full");
	mknod(tmp,S_IFCHR | 0666,makedev(1,7));
    sscanf(tmp,"%s%s",real_root_dir,"dev/random");
	mknod(tmp,S_IFCHR | 0666,makedev(1,8));
    sscanf(tmp,"%s%s",real_root_dir,"dev/urandom");
	mknod(tmp,S_IFCHR | 0666,makedev(1,9));
    sscanf(tmp,"%s%s",real_root_dir,"dev/tty");
	mknod(tmp,S_IFCHR | 0666,makedev(5,0));
    sscanf(tmp,"%s%s",real_root_dir,"dev/ptmx");
	mknod(tmp,S_IFCHR | 0666,makedev(5,2));
    sscanf(tmp,"%s%s",real_root_dir,"dev/pts");
	creatDirIfNotexist(tmp, 0666);
	mount("devpts",tmp,"devpts",0,NULL);

    //换根
    sscanf(tmp,"%s%s",real_root_dir,"tmp/.oldroot");
    syscall(SYS_pivot_root,real_root_dir,tmp);
    chdir("/");
    umount2("/tmp/.oldroot", MNT_DETACH);
    rmdir("/tmp/.oldroot");

    return 1;
}