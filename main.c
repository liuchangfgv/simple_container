#include <stdint.h>
#include<stdio.h>
#define _GNU_SOURCE
#define __USE_GNU
#include<sched.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/mount.h>
#include<string.h>
#include<sys/stat.h>
#include<errno.h>

#define STACK_SIZE (1024 * 1024)
static char child_stack[STACK_SIZE];

int mkdev(
	mode_t console_mode,dev_t console_dev
){
	mount("tmpfs","/dev","tmpfs",0,NULL);
	mknod("/dev/console",console_mode, console_dev);
	mknod("/dev/null",1,3);
	mknod("/dev/zero",1,5);
	mknod("/dev/full",1,7);
	mknod("/dev/random",1,8);
	mknod("/dev/urandom",1,9);
	mknod("/dev/tty",5,0);
	mknod("/dev/ptmx",5,2);
	mkdir("/dev/pts",0655);
	mount("devpts","/dev/pts","devpts",0,NULL);

	return 0;
}

void get_console(uint16_t *num){
	ioctl(0, TIOCGDEV, &num);//num[3]
}

void umountall(){
#define ROOT_PATH "./root"
	umount(ROOT_PATH"/sys/fs/cgroup/systemd");
	umount(ROOT_PATH"/sys/fs/cgroup");
	umount(ROOT_PATH"/var/lib/journal");
	umount(ROOT_PATH"/run/lock");
	umount(ROOT_PATH"/tmp");
	umount(ROOT_PATH"/run");
	umount(ROOT_PATH"/sys");
	umount(ROOT_PATH"/proc");
	umount(ROOT_PATH"/dev/pts");
	umount(ROOT_PATH"/dev");
}

int child(){
	chroot("root");
	chdir("/");
	mount("proc","/proc","proc",0,NULL);
	// mount("dev","/dev","devtmpfs",0,NULL);
	uint8_t num[3];
	get_console(num);
	mkdev(num[1],num[0]);
	mount("sys","/sys","sysfs",MS_NOEXEC|MS_NOSUID|MS_RDONLY|MS_NODEV,NULL);
	// mount("proc","/proc/sys","proc",MS_NOEXEC|MS_NOSUID|MS_RDONLY|MS_NODEV,NULL);
	mount("tmpfs","/run","tmpfs",0,NULL);
	mount("tmpfs","/tmp","tmpfs",0,NULL);
	mkdir("/run/lock",0655);
	mount("tmpfs","/run/lock","tmpfs",0,NULL);
	mount("tmpfs","/var/lib/journal","tmpfs",0,NULL);
	mount("cgroup2","/sys/fs/cgroup","cgroup2",0,NULL);
	mkdir("/sys/fs/cgroup/systemd",0655);
	mount("tmpfs","/sys/fs/cgroup/systemd","tmpfs",0,NULL);
  
	clearenv();
	setenv("PATH","/bin:/usr/bin:/sbin:/usr:/sbin");
	sethostname("aaa",strlen("aaa"));
	char *cmd[2];
	if(fork()){
		cmd[0]="/sbin/init";cmd[1]=NULL;
		execvp(cmd[0],cmd);
	}
	else{
		cmd[0]="/bin/bash";cmd[1]=NULL;
		execvp(cmd[0],cmd);
	}
	printf("ん? run %s faild\n",cmd[0]);
	return 0;
}

int main(){
	if(getuid()){
		printf("Must run as root!\n");
		return -1;
	}
	printf("loading...\n");

	int pid = clone(child,child_stack+STACK_SIZE,
			CLONE_NEWPID|CLONE_NEWUTS|CLONE_NEWNET|
                    CLONE_NEWCGROUP|
                    CLONE_NEWNS|CLONE_NEWIPC|SIGCHLD 
			,NULL);
	waitpid(pid,NULL,0);
	umountall();
	printf("end\n");
	return 0;
}
