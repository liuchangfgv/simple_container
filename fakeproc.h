#ifndef FAKEPROC_H
#define FAKEPROC_H

#include <fuse.h>

struct fakeProcFile{
    char *name;
    int *(attr)(struct stat *stbuf);
    int *(read)(void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi);
};

#endif