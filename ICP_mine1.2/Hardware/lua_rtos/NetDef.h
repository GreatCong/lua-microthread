#ifndef __NETDEF_H__
#define __NETDEF_H__

struct Net_state{
  int net_on;
	int net_mode;
	int net_protocol;
};

typedef struct Net_state Wifi_state;

#define NET_MODE_STA 0
#define NET_MODE_AP 1

#define NET_TCP_SERVER 0
#define NET_TCP_CLIENT 1
#define NET_UDP_SERVER 2
#define NET_UDP_CLIENT 3

#endif
