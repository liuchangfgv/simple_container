#ifndef NET_H
#define NET_H

int tun_alloc(char *dev);
int enable_net_dev(char *name);
int set_net_ip_mask(char *name,char *ip_mask);

#endif