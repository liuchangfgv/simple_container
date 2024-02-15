#define _GNU_SOURCE
#define __USE_GNU
#include <asm-generic/errno.h>
#include <linux/seccomp.h>
#include <seccomp.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sched.h>

int initSeccomp(){
    scmp_filter_ctx ctx = NULL;

    ctx = seccomp_init(SCMP_ACT_ALLOW);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(chmod), 1,SCMP_A1(SCMP_CMP_MASKED_EQ,S_ISUID,S_ISUID));//禁用suid
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(chmod), 1, SCMP_A1(SCMP_CMP_MASKED_EQ, S_ISGID, S_ISGID));//禁用sgid
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(fchmod), 1,SCMP_A1(SCMP_CMP_MASKED_EQ,S_ISUID,S_ISUID));//禁用suid
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(fchmod), 1, SCMP_A1(SCMP_CMP_MASKED_EQ, S_ISGID, S_ISGID));//禁用sgid
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(fchmodat), 1,SCMP_A1(SCMP_CMP_MASKED_EQ,S_ISUID,S_ISUID));//禁用suid
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(fchmodat), 1, SCMP_A1(SCMP_CMP_MASKED_EQ, S_ISGID, S_ISGID));//禁用sgid

    seccomp_rule_add(ctx,SCMP_ACT_ERRNO(ENOSYS),SCMP_SYS(mount),0);//禁止mount
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(umount), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(umount2), 0);

    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(acct), 0);//可能存在敏感信息泄漏的风险，故禁用
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(lookup_dcookie), 0);//会泄漏主机信息
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(perf_event_open), 0);//会泄漏主机信息

    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(add_key), 0);//禁止向内核加入密钥环
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(keyctl), 0);//内核密钥环
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(request_key), 0);

    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(bpf), 0);//禁止容器使用bpf
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(create_module), 0);//禁止容器添加内核模块
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(delete_module), 0);//禁止容器卸载内核模块
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(finit_module), 0);//禁止容器操作内核模块
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(init_module), 0);//禁止内核模块的操作

    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(clone), 1,SCMP_A2(SCMP_CMP_MASKED_EQ,CLONE_NEWNS,CLONE_NEWNS));//禁止创建名字空间
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(pivot_root), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(setns), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(unshare), 0);
    
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(get_mempolicy), 0);//禁止容器修改内核内存和NUMA设置
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(ioperm), 0);//防止io权限等级修改
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(iopl), 0);//同上
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(mbind), 0);//内核内存和NUMA设置
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(move_pages), 0);//同上
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(set_mempolicy), 0);
    
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(swapon), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(swapoff), 0);

    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(kcmp), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(process_vm_readv), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(process_vm_writev), 0);

    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(kexec_file_load), 0);//极度危险
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(kexec_load), 0);//同上
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(reboot), 0);
    
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(open_by_handle_at), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(ustat), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(userfaultfd), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(uselib), 0);
    
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(personality), 0);//BUG
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(quotactl), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(sysfs), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ERRNO(ENOSYS), SCMP_SYS(_sysctl), 0);

    seccomp_load(ctx);
    seccomp_release(ctx);

}