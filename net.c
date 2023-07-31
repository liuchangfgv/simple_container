#include <linux/if.h>
#include <linux/if_addr.h>
#include <linux/if_tun.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "net.h"

int tun_alloc(char *dev){
    /*
    * 创建tun虚拟网卡
    * 输入参数网卡名字
    * 返回fd
    */
    struct ifreq ifr;
    int fd,err;

    if((fd = open("/dev/net/tun",O_RDONLY)) < 0){
        return -1;
    }

    memset(&ifr,0,sizeof(ifr));

    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strcpy(ifr.ifr_name,dev);

    err = ioctl(fd,TUNSETIFF,(void *)&ifr);
    if(err){
        if(err > 0)
            return -err;
        return err;
    }

    return fd;

}

int enable_net_dev(char *name){
    /*
    * 启用网卡
    * 参数网卡名字
    * 返回值为0则成功
    */
    struct ifreq ifr;
    int socketfd = socket(AF_INET,SOCK_DGRAM,0);
    if(socketfd < 0){
        perror("socket");
        close(socketfd);
        return errno;
    }

    memset(&ifr,0,sizeof(ifr));
    strncpy(ifr.ifr_name,name,IFNAMSIZ);

    if(ioctl(socketfd,SIOCGIFFLAGS,&ifr) < 0){
        perror("ioctl");
        close(socketfd);
        return errno;
    }

    ifr.ifr_flags |= IFF_UP;
    if(ioctl(socketfd,SIOCSIFFLAGS,&ifr) < 0){
        perror("ioctl");
        close(socketfd);
        return errno;
    }

    close(socketfd);
    return 0;
}

int set_net_ip_mask(char *name,char *ip_mask){
    /*
    * 支持：
    * "192.168.1.6" 仅设置ip
    * "192.168.5.6/24" 设置ip和掩码
    * "/24" 设置掩码
    */
    char ip[16]={0},mask[16]={0};
    struct ifreq ifr;
    int socketfd;
    char *po = ip_mask;
    int temp = 0;
    int ipa,ipb,ipc,ipd;

    if(*po != '/'){
        sscanf(ip_mask,"%d.%d.%d.%d",&ipa,&ipb,&ipc,&ipd);
        sprintf(ip,"%d.%d.%d.%d",ipa,ipb,ipc,ipd);
    }
    
    while(*po){
        if(*po == '/'){
            temp = atoi(po+1);
            uint32_t mask_t = 0;
            for(int i = 0;i < temp;i ++){
                mask_t |= 1 << (31 - i);
            }
            sprintf(mask,"%d.%d.%d.%d",(mask_t >> 24),(mask_t >> 16) & 0xff,(mask_t >> 8) & 0xff,mask_t & 0xff );
            break;
        }
        po++;
    }

    socketfd = socket(AF_INET,SOCK_DGRAM,0);
    if(socketfd < 0){
        perror("socket");
        close(socketfd);
        return errno;
    }

    memset(&ifr,0,sizeof(ifr));
    strncpy(ifr.ifr_name,name,IFNAMSIZ);

    if(*ip){
        struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
        addr->sin_family = AF_INET;
        addr->sin_addr.s_addr = inet_addr(ip);
        if(ioctl(socketfd,SIOCSIFADDR,&ifr) < 0){
            perror("ip_ioctl");
            close(socketfd);
            return errno;
        }
    }

    if(*mask){
        struct sockaddr_in *netmask = (struct sockaddr_in *)&ifr.ifr_netmask;
        netmask->sin_family = AF_INET;
        netmask->sin_addr.s_addr = inet_addr(mask);
        if(ioctl(socketfd,SIOCSIFNETMASK,&ifr) < 0){
            perror("netmask_ioctl");
            close(socketfd);
            return errno;
        }
    }
    
    close(socketfd);
    return 0;
}
