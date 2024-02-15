#ifndef KASORA_MOUNT_H
#define KASORA_MOUNT_H

#include <stddef.h>
#include <sys/types.h>

struct mount_point{
    char *host_path;//host路径
    char *container_path;//容器路径
    unsigned int attr;//属性
    char *tmpfs_size;//如果是tmpfs的话，要有个大小,直接1G、512M这样写
};

#define MOUNT_ATTR_READONLY 0x1
#define MOUNT_ATTR_NODEV    0x2
#define MOUNT_ATTR_NOEXEC   0x4
#define MOUNT_ATTR_NOSUID   0x8
#define MOUNT_ATTR_TMPFS    0x10
#define MOUNT_POINT_END     0x20

int mount_root(struct mount_point *mp,int enable_x11);

#endif