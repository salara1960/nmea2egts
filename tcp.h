#ifndef _TCP_H_
#define _TCP_H_

#include "func.h"


#define MAX_NMEA_MSG     2//13//12//11//10//9//8//7//6
#define NMEA_TYPE_LEN    3
#define MAX_LIST_ITEM   24
#define MAX_ITEM_LEN    20


#define DEF_SPEED B115200



enum {
    ixNone = -1,
    ixGGA = 0,
    ixRMC/*,
    ixGST,
    ixGMP,
    ixGLL,
    ixVTG,
    ixGNS,
    ixGRS,
    ixGSA,
    ixGSV,
    ixHDT,
    ixROT,
    ixZDA*/
};

int fd;
bool tcp_connect;

extern s_loc_data loc_data;
extern volatile bool egts_connect;

int init_uart(char *dev_name);
int deinit_uart(int fd);

void loopDev(void *arg);
void *loopTCP(void *arg);

extern int msgPutEgts(void *str, int len);


#endif
