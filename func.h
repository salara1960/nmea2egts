#ifndef FUNC_H_
#define FUNC_H_

#include "inc.h"


//-----------------------------------------------------------------

#define BUF_SIZE      2048
#define BUF_SIZE_PRN  BUF_SIZE

//#define MAX_SIZE_LOG  4000 * 1024 //4M max log_file
#define MAX_SIZE_ELOG 2000 * 1024 //2M max log_file


#define MAX_PARAM  5//4//16//15//14//13//12//11//10//9

#define LEN_512  512
#define LEN_1K  1024
#define LEN_2K  2048
#define LEN_3K  3072
#define LEN_4K  4096

//#define MAX_CNT_FLASH 50


#define TOTAL_SPEED 13

//-----------------------------------------------------------------------

#define _1ms 1
#define _2ms 2
#define _3ms 3
#define _4ms 4
#define _5ms 5
#define _6ms 6
#define _7ms 7
#define _8ms 8
#define _9ms 9
#define _10ms 10
#define _20ms 2 * _10ms
#define _30ms 3 * _10ms
#define _40ms 4 * _10ms
#define _50ms 5 * _10ms
#define _60ms 6 * _10ms
#define _70ms 7 * _10ms
#define _80ms 8 * _10ms
#define _90ms 9 * _10ms
#define _100ms 10 * _10ms

#define _150ms _100ms + _50ms
#define _200ms 2 * _100ms
#define _250ms _200ms + _50ms
#define _300ms 3 * _100ms
#define _350ms _300ms + _50ms
#define _400ms 4 * _100ms
#define _450ms _400ms + _50ms
#define _500ms 5 * _100ms
#define _550ms _500ms + _50ms
#define _600ms 6 * _100ms
#define _650ms _600ms + _50ms
#define _700ms 7 * _100ms
#define _750ms _700ms + _50ms
#define _800ms 8 * _100ms
#define _850ms _800ms + _50ms
#define _900ms 9 * _100ms
#define _950ms _900ms + _50ms
#define _1000ms 10 * _100ms

#define _1s _1000ms
#define _1s1 _1s + _100ms
#define _1s2 _1s + _200ms
#define _1s3 _1s + _300ms
#define _1s4 _1s + _400ms
#define _1s5 _1s + _500ms
#define _1s6 _1s + _600ms
#define _1s7 _1s + _700ms
#define _1s8 _1s + _800ms
#define _1s9 _1s + _900ms
#define _2s _1s * 2
#define _2s5 (_1s * 2) + _500ms
#define _3s _1s * 3
#define _3s5 (_1s * 3) + _500ms
#define _4s _1s * 4
#define _4s5 (_1s * 4) + _500ms
#define _5s _1s * 5
#define _5s5 (_1s * 5) + _500ms
#define _6s _1s * 6
#define _6s5 (_1s * 6) + _500ms
#define _7s _1s * 7
#define _7s5 (_1s * 7) + _500ms
#define _8s _1s * 8
#define _8s5 (_1s * 8) + _500ms
#define _9s _1s * 9
#define _9s5 (_1s * 9) + _500ms
#define _10s _1s * 10
#define _15s _1s * 15
#define _20s _1s * 20
#define _25s _1s * 25
#define _30s _1s * 30


//#define SPEED_EGTS_SEND 10
//#define APK_EGTS_SEND   44
//#define APK_RESET_GNSS  51


//------------------------------------------------------------------------
/*
enum {
    sType1 = 1,      // giroscop
    sType2 = 2,      // accel
    sType3 = 4,      // magnit
    sType4 = 8,      // bar
    //
    sType5 = 0x10,    // BLE
    sType6 = 0x20,    // WI-FI
    sType7 = 0x40,    // GNSS
    sType8 = 0x80,    // LSN
    sType9 = 0x100,   // Result
};
*/

enum {
    pHelp = 0,
    pDbg,
    pPort,
    pDev,  
    pEgts,
};

/*
#pragma pack(push,1)
typedef struct {
    unsigned ml:6;//20;
    unsigned tele_afu:2;
    unsigned unused:12;
    unsigned stop:1;   // 0 - движение, 1 - остановка
    unsigned extra:1;  // координаты получены из решения НЗ
    unsigned diff:1;   // 0 - автономное решение, 1 - дифференциальное решение
    unsigned st:9;
} ccp_t;
#pragma pack(pop)
*/

//------------------------------------------------------------------------

const char *version;
uint8_t QuitAll;
uint8_t dbg;

bool gps_valid;

pid_t pid_main;
char *buf_prn;
uint8_t volatile en_prn;
volatile uint8_t total_threads;
char uNameFile[64];

extern uint32_t termID;

uint16_t tcp_port;
char ip[128];

bool tcp_flag;
char dev_name[128];
extern uint32_t SPEED;
extern val_speed_t all_speed[TOTAL_SPEED];

//-------------------------------------------------------------------------
//
int parse_param_start(char *p_line);
void GetSignal_(int sig);
//
void usDelay(uint32_t us);
uint64_t get_msec();
uint32_t get_tmr_sec(uint32_t tm);
int check_tmr_sec(uint32_t tm);
int tmrOpen();
void set_ms(uint64_t ms);
uint64_t get_ms();
uint64_t get_tmr(uint64_t tm);
int check_tmr(uint64_t tm);    
//    
int TNP(char *ts);
void Report(const uint8_t addTime, const char *fmt, ...);
void done_mutex();
//
//-------------------------------------------------------------------------


#endif

