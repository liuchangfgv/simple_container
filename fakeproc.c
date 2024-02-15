#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// 定义一个简单的文本内容
static const char *hello_str = "Hello, World!\nThis is a sample file in our virtual filesystem.\n";
static const char *hello_path = "/sample.txt";

// getattr 函数用于获取文件属性
static int hello_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        // 根目录的属性
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else if (strcmp(path, hello_path) == 0) {
        // 文件的属性
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(hello_str);
    } else {
        res = -ENOENT;
    }

    return res;
}

// readdir 函数用于读取目录内容
static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);

    return 0;
}

// read 函数用于读取文件内容
static int hello_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else
        size = 0;

    return size;
}

// 定义操作函数结构体
static struct fuse_operations hello_oper = {
    .getattr	= hello_getattr,
    .readdir	= hello_readdir,
    .read		= hello_read,
};

int enable_fakeproc(char *path)
{
    int argc = 2;
    char *argv[] = {
        "fakefuse",
        path
    };
    return fuse_main(argc, argv, &hello_oper, NULL);
}
