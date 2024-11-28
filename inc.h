#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <netinet/in.h>
#include <termios.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <endian.h>
#include <pthread.h>
#include <signal.h>
#include <linux/gpio.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/input.h>
#include <linux/types.h>
#include <math.h>
#include <assert.h>


//--------------------------------------------------------------------------------------------------

#define SET_PRN_MUTEX


#define MSK_TZ          3
#define TIME_ZONE       (MSK_TZ * (60 * 60))
#define ADD_TO_2008     1199145600
#define LOCAL_03062023  1685739600


//--------------------------------------------------------------------------------------------------



enum {
    noBoard = 0,
    bATBl,
    bATB,
    bRD,
    bATV,
    unKnown = 255
};

enum {
    logOff = 0,
    logOn,
    logDebug,
    logDump
};

#pragma pack(push,1)
typedef struct {
    void *str;
    int len;
} sens_data_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    long mtype;       //4
    sens_data_t msg;        //8
    unsigned int numb;//4
} s_sens_COM;          //16
#pragma pack(pop)

//#pragma once


#pragma pack(push,1)
    typedef struct {
            uint32_t TM;
            uint32_t NTM;
            uint32_t LAT;
            uint32_t LONG;
            uint16_t SPD;
            uint16_t DIR;
            int ALT;
            uint32_t ODM;
            uint8_t DIN;
            uint8_t SAT;
            uint8_t DOUT;
            uint16_t AIN[8];
            unsigned VLD:1;   //cor valid
            unsigned MV:1;    //1-moving, 0-stop
            unsigned LAHS:1;  //0-north latitude, 1-south latitude
            unsigned LOHS:1;  //0-ost longitude, 1-west longitude
            unsigned unused:4;
    } s_loc_data;
#pragma pack(pop)

//  Структура с переменными для запоминания данных геолокации
#pragma pack(push,1)
typedef struct {
    double dec_longitude;
    double dec_latitude;
    double dec_dir;
    uint32_t valid;
    //
    uint32_t epoch;
    uint8_t mlsec;
    struct tm dt;
    char mode;

    // GGA - Global Positioning System Fixed Data
    double nmea_longitude;
    double nmea_latitude;
    double utc_time;//UTC time of the GGA or GNS fix associated with this sentence
    char ns, ew;
    int quality;
    int satelite;
    double HDOP;
    double altitude;
    char altitude_units;
    double geoid;
    char geoid_units;

    // RMC - Recommended Minimmum Specific GNS Data
    char rmc_status;
    double speed;
    double dir;

} javad_gps_t;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct {
    int in_spd;
    uint32_t out_spd;
} val_speed_t;
#pragma pack(pop)


//--------------------------------------------------------------------------------------------------
/*
#define HTONS(x) \
    ((uint16_t)((x >> 8) | ((x << 8) & 0xff00)))
#define HTONL(x) \
    ((uint32_t)((x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | ((x << 24) & 0xff000000)))
*/
//--------------------------------------------------------------------------------------------------


