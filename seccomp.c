#include <seccomp.h>
#include <stdlib.h>
#include <sys/stat.h>

int initSeccomp(){
    scmp_filter_ctx ctx = NULL;

    ctx = seccomp_init(SCMP_ACT_ALLOW);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(-1), SCMP_SYS(chmod), 1,SCMP_A1(SCMP_CMP_MASKED_EQ,S_ISUID,S_ISUID));

    seccomp_load(ctx);
    seccomp_release(ctx);

}