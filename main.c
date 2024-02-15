#include "mount.h"
#include <stdint.h>
#include <stdio.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "net.h"
#include "mount.h"
#include "seccomp.h"
#include <sys/sysmacros.h>
#include <stdlib.h>

#define STACK_SIZE (1024 * 1024)
static char child_stack[STACK_SIZE];

int container_pid;

// void signal_handler(int signum) {
//     if (signum == 30) {
// 		kill(container_pid,3);
//     }
// }

// int mkdev(
// 	dev_t console_dev
// ){
// 	mount("tmpfs","/dev","tmpfs",0,NULL);
// 	mknod("/dev/console",S_IFCHR |0600, console_dev);
// 	mknod("/dev/tty1",S_IFCHR |0600, console_dev);
// 	mknod("/dev/null",S_IFCHR | 0666,makedev(1,3));
// 	// chmod("/dev/null",0666);
// 	mknod("/dev/zero",S_IFCHR | 0666,makedev(1,5));
// 	// chmod("/dev/zero",0666);
// 	mknod("/dev/full",S_IFCHR | 0666,makedev(1,7));
// 	// chmod("/dev/full",0666);
// 	mknod("/dev/random",S_IFCHR | 0666,makedev(1,8));
// 	// chmod("/dev/random",0666);
// 	mknod("/dev/urandom",S_IFCHR | 0666,makedev(1,9));
// 	// chmod("/dev/urandom",0666);
// 	mknod("/dev/tty",S_IFCHR | 0666,makedev(5,0));
// 	// chmod("/dev/tty",0666);
// 	mknod("/dev/ptmx",S_IFCHR | 0666,makedev(5,2));
// 	// chmod("/dev/ptmx",0666);
// 	mkdir("/dev/pts",0655);
// 	// chmod("/dev/pts",0755);
// 	mount("devpts","/dev/pts","devpts",0,NULL);

// 	return 0;
// }

// void umountall(){
// #define ROOT_PATH "./root"
// 	umount(ROOT_PATH"/sys/fs/cgroup/systemd");
// 	umount(ROOT_PATH"/sys/fs/cgroup");
// 	umount(ROOT_PATH"/var/lib/journal");
// 	umount(ROOT_PATH"/run/lock");
// 	umount(ROOT_PATH"/tmp");
// 	umount(ROOT_PATH"/run");
// 	umount(ROOT_PATH"/sys");
// 	umount(ROOT_PATH"/proc");
// 	umount(ROOT_PATH"/dev/pts");
// 	umount(ROOT_PATH"/dev");
// }

int child(){
	//关于在容器中运行systemd的条件，可以参考 https://systemd.io/CONTAINER_INTERFACE/
	// chroot("root");
	// chdir("/");
	// mount("proc","/proc","proc",0,NULL);
	// mount("dev","/dev","devtmpfs",0,NULL);
	// dev_t num;
	// ioctl(0, TIOCGDEV, &num);
	// mkdev(num);
	// mount("sys","/sys","sysfs",MS_NOEXEC|MS_NOSUID|MS_RDONLY|MS_NODEV,NULL);
	// mount("proc","/proc/sys","proc",MS_NOEXEC|MS_NOSUID|MS_RDONLY|MS_NODEV,NULL);
	// mount("tmpfs","/run","tmpfs",0,NULL);
	// mount("tmpfs","/tmp","tmpfs",0,NULL);
	// mkdir("/run/lock",0655);
	// mount("tmpfs","/run/lock","tmpfs",0,NULL);
	// mount("tmpfs","/var/lib/journal","tmpfs",0,NULL);
	// mount("cgroup2","/sys/fs/cgroup","cgroup2",0,NULL);
	// mkdir("/sys/fs/cgroup/systemd",0655);
	// mount("tmpfs","/sys/fs/cgroup/systemd","tmpfs",0,NULL);

    struct mount_point mp[] = {
        {
            .host_path = "root",
            .container_path = "/"
        },
        {
            .attr = MOUNT_POINT_END,
        }
    };

    mount_root(mp, 1);
    initSeccomp();
  
	clearenv();
	setenv("PATH","/bin:/usr/bin:/sbin:/usr:/sbin",1);
	setenv("container","llccontaner",1);
	setenv("container_ttys","tty1",1);
	setenv("TERM","linux",1);
	sethostname("aaa",strlen("aaa"));
	

	char *cmd[2];
	// int pid;
	// pid = fork();
	// if(fork()){
		// if(!fork()){
		// 	int tun_no = tun_alloc("tun0");
		// 	enable_net_dev("tun0");
		// 	set_net_ip_mask("tun0", "10.9.9.2/24");
		// 	waitpid(pid, NULL, 0);
		// 	kill(1,3);
		// }
		cmd[0]="/sbin/init";cmd[1]=NULL;
		execvp(cmd[0],cmd);
	// }
	// else{
		// int pid;
		// pid = fork();
		// if(pid){
			// int tun_no = tun_alloc("tun0");
			// enable_net_dev("tun0");
			// set_net_ip_mask("tun0", "10.9.9.2/24");
			// waitpid(pid, NULL, 0);
			// kill(0,30);
			// return 0;
		// }
		// else{
			// cmd[0]="/bin/bash";cmd[1]=NULL;
			// execvp(cmd[0],cmd);
		// }
	// }
	printf("ん? run %s faild\n",cmd[0]);
	return 0;
}

int main(){
	// signal(30, signal_handler);
	if(getuid()){
		printf("Must run as root!\n");
		return -1;
	}
	printf("loading...\n");

	container_pid = clone(child,child_stack+STACK_SIZE,
			CLONE_NEWPID|CLONE_NEWUTS|CLONE_NEWNET|
                    CLONE_NEWCGROUP|
                    CLONE_NEWNS|CLONE_NEWIPC|SIGCHLD 
			,NULL);
	printf("pid:%d\n",container_pid);
	waitpid(container_pid,NULL,0);
	// umountall();
	printf("end\n");
	return 0;
}
