#ifndef SWITCH_H
#define SWITCH_H

typedef struct{
    int tun_num;
    int port_num;
} net_switch_port;

typedef struct net_switch_port_List{
    net_switch_port *port;
    struct net_switch_port_List *next;
} net_switch_port_List;

typedef struct{
    net_switch_port_List ports;
} net_switch;

void sendpack(net_switch,int);
net_switch *new_net_switch();
int net_switch_add_port(net_switch,int);

#endif