#include "func.h"

//-----------------------------------------------------------------------

//const char *version = "0.0.1 19.06.2024";
//const char *version = "0.0.2 20.06.2024";
//const char *version = "0.0.3 24.06.2024";
//const char *version = "0.1.0 25.06.2024";
const char *version = "0.1.1 28.11.2024";



uint8_t QuitAll  = 0;
uint8_t dbg = logOn;

bool gps_valid = false;

char uNameFile[64] = {0};

pid_t pid_main;

volatile static uint64_t varta = 0;

uint8_t SIGHUPs  = 1;
uint8_t SIGTERMs = 1;
uint8_t SIGINTs  = 1;
uint8_t SIGKILLs = 1;
uint8_t SIGSEGVs = 1;
uint8_t SIGABRTs = 1;
uint8_t SIGSYSs  = 1;
uint8_t SIGTRAPs = 1;
uint8_t SIGFPEs  = 1;


char *buf_prn = NULL;
#ifdef SET_PRN_MUTEX
    pthread_mutex_t prn_buf_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

volatile uint8_t en_prn = 0;
volatile uint8_t total_threads = 0;

#ifdef SET_LOG_MUTEX
    pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

uint16_t tcp_port = 9876;
char ip[128] = {0};
const char *all_param[] = {
    "help", 
    "dbg=",
    "tcp=",
    "dev=",
    "egts="
};

bool tcp_flag = false;
char dev_name[128] = {0};

//--------------------------------------------------------------------------------
//   Функция анализирует параметры, заданные при старте утилиты
//    и определяющие режим работы и последовательность действий
//
int parse_param_start(char *p_line)
{
int i;
char *uk = NULL, *ukz = NULL;


    char *uks = strchr(p_line, '\n');
    if (uks) *uks = '\0';
    for (i = 0; i < MAX_PARAM; i++) {
        uks = strstr(p_line, all_param[i]);
        if (uks) {
            uks += strlen(all_param[i]);
            switch (i) {
                case pHelp:
                    Report(1, "Start program with params: ./nmea2egts tcp=<server_port> egts=<server_addr:server_port:termID\n"
                              "  help - show this message\n"
                              "  tcp=<tcp_server_port> - port for receive nmea message (default 9876)\n"
                              "  or dev=<serial_port:speed> - serial port and speed for receive nmea message (/dev/ttyUSB0:115200)\n"
                              "  egts=<server_ip:server_port:tid> - activate egts client, where tid - termID\n"
                              "  dbg=<level> - level : 'off', 'on', 'debug', 'dump' (default - on)\n");
                    return -1;
                break;
                case pDbg:
                    if (!strcmp(uks, "off")) {
                        dbg = logOff;
                    } else if (!strcmp(uks, "on")) {
                        dbg = logOn;
                    } else if (!strcmp(uks, "debug")) {
                        dbg = logDebug;
                    } else if (!strcmp(uks, "dump")) {
                        dbg = logDump;
                    } 
                break;
                case pPort:
                {
                    uint16_t resa = atoi(uks);
                    if ((resa > 0) && (resa < 0xffff)) tcp_port = resa;
                    tcp_flag = true;
                }
                break;
                case pDev:
                {
                    if (!tcp_flag) {
                        strcpy(dev_name, uks);
                        uk = strchr(dev_name, ':');
                        if (uk) {
                            int spd = atoi(uk + 1);
                            *uk = '\0';
                            for (int j = 0; j < TOTAL_SPEED; j++) {
                                if (spd == all_speed[j].in_spd) {
                                    SPEED = all_speed[j].out_spd;
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
                case pEgts:
                {
                    strcpy(ip, uks);
                    uk = strchr(ip, ':');
                    if (uk) {
                        uk++;
                        ukz = strchr(uk, ':');
                        if (ukz) {
                            uint32_t t_id = atol(ukz + 1);
                            *ukz = '\0';
                            if (t_id) termID = t_id;
                        }
                    }
                }
                break;
            }
        }
    }

    return 0;
}
//------------------------------------------------------------------------------------
//--------------------  function for recive SIGNAL from system -----------------------
// Функция обрабатывает сигналы, полученные от системы в ходе выполнения утилиты
//
void GetSignal_(int sig)
{
uint8_t out = 0;
char stx[64] = {0};


    switch (sig) {
        case SIGALRM://получен сигнал от Таймера - истек период в 1 миллисекунду
            ++varta;
            return;
        case SIGHUP://-1
            strcpy(stx, "\tSignal SIGHUP\n");
        break;
        case SIGUSR1://-10
            //if (!result_flag) result_flag = 1;
        break;
        case SIGUSR2://-12
            en_prn++;
            en_prn &= 1;
            //strcpy(stx, "\tSignal USR2\n");
        break;
        case SIGFPE://-8
            if (SIGFPEs) {
                SIGFPEs = 0;
                strcpy(stx, "\tSignal SIGFPE\n");
                out = 1;
            }
        break;
        case SIGKILL:
            if (SIGKILLs) {
                SIGKILLs = 0;
                strcpy(stx, "\tSignal SIGKILL\n");
                out = 1;
            }
        break;
        case SIGPIPE:
            strcpy(stx, "\tSignal SIGPIPE\n");
        break;
        case SIGTERM://-15
        case SIGQUIT://-3
                out = 1;
        break;
        case SIGINT:
            if (SIGINTs) {
                SIGINTs = 0;
                out = 1;
            }
        break;
        case SIGSEGV:
            if (SIGSEGVs) {
                SIGSEGVs = 0;
                strcpy(stx, "\tSignal SIGSEGV\n");
                out = 1;
            }
        break;
        case SIGABRT:
            if (SIGABRTs) {
                SIGABRTs = 0;
                strcpy(stx, "\tSignal SIGABRT\n");
                out = 1;
            }
        break;
        case SIGSYS:
            if (SIGSYSs) {
                SIGSYSs = 0;
                strcpy(stx, "\tSignal SIGSYS\n");
                out = 1;
            }
        break;
        case SIGTRAP:
            if (SIGTRAPs) {
                SIGTRAPs = 0;
                strcpy(stx, "\tSignal SIGTRAP\n");
                out = 1;
            }
        break;
            default : sprintf(stx, "\tUNKNOWN signal %d", sig);
    }

    if (strlen(stx)) printf("%s", stx);

    if (out) QuitAll = 1;
}
//-----------------------------------------------------------------------
//  Функция устанавливает обработчик (GetSignal_) некоторых сигналов ОС Linux
//
void setSigSupport()
{
static struct sigaction Act = {0}, OldAct = {0};

    Act.sa_handler = &GetSignal_;
    Act.sa_flags   = 0;//SA_SIGINFO;//0;
    sigaction(SIGPIPE, &Act, &OldAct);
    sigaction(SIGSEGV, &Act, &OldAct);//очень плохо !
    sigaction(SIGABRT, &Act, &OldAct);
    sigaction(SIGINT,  &Act, &OldAct);
    sigaction(SIGSYS,  &Act, &OldAct);
    sigaction(SIGKILL, &Act, &OldAct);
    sigaction(SIGTRAP, &Act, &OldAct);

    sigaction(SIGHUP,  &Act, &OldAct);//-1  пока не используется

    sigaction(SIGTERM, &Act, &OldAct);//-15 Quit from app
    sigaction(SIGQUIT, &Act, &OldAct);//-3  Quit from app
    sigaction(SIGFPE,  &Act, &OldAct);//-8

    sigaction(SIGUSR1, &Act, &OldAct);//-10 print result
    sigaction(SIGUSR2, &Act, &OldAct);//-12 print data

    sigaction(SIGALRM, &Act, &OldAct);
}
//----------------------------------------------------------------------
void usDelay(uint32_t us)
{
    usleep(us);
}
//----------------------------------------------------------------------
uint32_t get_tmr_sec(uint32_t tm)
{
    return (uint32_t)(time(NULL) + tm);
}
//----------------------------------------------------------------------
int check_tmr_sec(uint32_t tm)
{
    return ((uint32_t)time(NULL) >= tm ? 1 : 0);
}
//----------------------------------------------------------------------
uint64_t get_msec()
{
struct timeval tvl;
uint64_t ret = 0;

    gettimeofday(&tvl, NULL);

    ret = (uint64_t)tvl.tv_sec;
    ret *= 1000;
    ret += (uint64_t)(tvl.tv_usec / 1000);

    return ret;
}
//----------------------------------------------------------------------
//  Функция устанавливает периодический таймер (период - 1 миллисекунда)
//  Функция возвращает ноль в успешном случае, или -1 в противном случае (чтение errno детализирует ошибку)
int tmrOpen()
{
    const struct itimerval itmr = {
        { 0, 1000 }, // it_interval
        { 0, 1000 }  // it_value
    };

    return (setitimer(ITIMER_REAL, &itmr, NULL));
}
//------------------------------
void set_ms(uint64_t ms)
{
    varta = ms;
}
//-----------------------------
uint64_t get_ms()
{
    return varta;
}
//------------------------------
uint64_t get_tmr(uint64_t tm)
{
    return (get_ms() + tm);
}
//------------------------------
int check_tmr(uint64_t tm)
{
    return (get_ms() > tm ? 1 : 0);
}
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//   Функция формирует символьную строку с текущими значениями даты и времени
//
int TNP(char *ts)
{
struct timeval tvl;

    gettimeofday(&tvl, NULL);
    struct tm *ctimka = localtime(&tvl.tv_sec);
    return sprintf(ts, "%02d.%02d %02d:%02d:%02d.%03d | ",
                ctimka->tm_mday,
                ctimka->tm_mon + 1,
                ctimka->tm_hour,
                ctimka->tm_min,
                ctimka->tm_sec,
                (int)(tvl.tv_usec/1000));
}
//-----------------------------------------------------------------------
void Report(const uint8_t addTime, const char *fmt, ...)
{
size_t len = BUF_SIZE_PRN;

#ifdef SET_PRN_MUTEX
    int ria = 1, cnt = 200;

    while ((ria = pthread_mutex_trylock(&prn_buf_mutex))) {
        usleep(100);
        cnt--;
    }
    if (!cnt && ria) {
        //devError |= devMutex;
        return;
    }
#endif

    buf_prn = (char *)calloc(1, len);//&prnBuff[0];
    if (buf_prn) {
        len--;
        va_list args;
        int dl = 0;
        if (addTime) dl = TNP(buf_prn);

        va_start(args, fmt);
        vsnprintf(buf_prn + dl, len - dl, fmt, args);
        va_end(args);

#ifdef ARM
        dl = write(1, buf_prn, strlen(buf_prn) + 1);
#else
        printf("%s", buf_prn);
#endif        
        free(buf_prn);
    }

#ifdef SET_PRN_MUTEX
    pthread_mutex_unlock(&prn_buf_mutex);
#endif
}
//------------------------------------------------------------------------------------------
void done_mutex()
{
#ifdef SET_PRN_MUTEX
    pthread_mutex_destroy(&prn_buf_mutex);
#endif
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
