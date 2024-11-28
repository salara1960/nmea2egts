/*
Для работы с протоколом ЕГТС используется адрес: 194.58.79.4.1:27199
*/

#include "egts.h"



//-----------------------------------------------------------------------------------------------------------

static int SEND_FLAGS = MSG_DONTWAIT | MSG_NOSIGNAL;

static const uint8_t eCRC8Table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};

static const uint16_t eCrc16Table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

const uint8_t maxPackName = 3;

const uint8_t max_servCODE = 5;
const uint8_t servCODE[] = {
    EGTS_AUTH_SERVICE,
    EGTS_TELEDATA_SERVICE,
    EGTS_COMMANDS_SERVICE,
    EGTS_FIRMWARE_SERVICE,
    EGTS_ECALL_SERVICE
};

const uint8_t servUSE[] = {
    EGTS_SST_IN_SERVICE,
    EGTS_SST_IN_SERVICE,
    EGTS_SST_OUT_OF_SERVICE,//128, //сервис в нерабочем состоянии (выключен)
    EGTS_SST_OUT_OF_SERVICE,
    EGTS_SST_OUT_OF_SERVICE
};


//*********************************************************************************************************************
//*********************************************************************************************************************
//*********************************************************************************************************************
//*********************************************************************************************************************


static char ip_egts[128] = {0};

char dev[imei_size + 1] = {0};
s_loc_data loc_data;
uint32_t termID = TermID;
int fd_elog = -1;

//---------------------------------------------------------------------------------------------------------------------

volatile bool egts_connect = false;

//char uNameFile[64] = {0};

int sens_queue_id_egts = {-1};
struct msqid_ds sens_msqid_ds_egts, *sens_mymsq_egts = NULL;
s_sens_COM sens_mess_egts;
volatile bool mk_quesens_egts = false;

int msgPutEgts(void *str, int len);



//-----------------------------------------------------------------------------
int mkQuePrnEgts()
{
int ret = 1;


    sens_mymsq_egts = &sens_msqid_ds_egts; // инициализировать указатель на структуру для очереди сообщений
    key_t mkey = ftok(uNameFile, 0x47 + ret + ((ret >> 4) << 3)); // получаем ключ для очереди сообщений COM
    if (mkey != -1) {
        sens_queue_id_egts = msgget(mkey, IPC_CREAT | IPC_NOWAIT | 0660);
        if (sens_queue_id_egts != -1) {
            sens_mess_egts.mtype = 1;                              // тип сообщения, пока 1
            msgctl(sens_queue_id_egts, IPC_STAT, sens_mymsq_egts);      // читаем статус очереди
            sens_mymsq_egts->msg_qbytes = sizeof(s_sens_COM) << 8; // на 256 записи
            msgctl(sens_queue_id_egts, IPC_SET, sens_mymsq_egts);       // увеличиваем(уменьшаем) буфер очереди
            if (dbg > logOn) Report(1, "[%s] make queue for sens thread with id:%d key:0x%X max[bytes:%u records:%u]\n",
                                        __func__, sens_queue_id_egts, (unsigned int)mkey,
                                        sens_mymsq_egts->msg_qbytes,
                                        sens_mymsq_egts->msg_qbytes / sizeof(s_sens_COM));
            ret = 0;
            mk_quesens_egts = true;
        }    
    }
    usDelay(5000);

    return ret;
}
//------------------------------------------------------------------------------------------
/*int add_to_elog(void *st, int len)
{
int ret = 0;
struct stat sb;

    if (fd_elog < 0) return 0;

    ret = write(fd_elog, st, len);
    
    if (!fstat(fd_elog, &sb)) {
        if (sb.st_size > MAX_SIZE_ELOG) {
            close(fd_elog);
            ret = -1; 
            fd_elog = ret;
        }
    }

    return ret;
}*/
//--------------------------------------------------------------------------------
int msgGetEgts(sens_data_t *msg)
{
int ret = 0;

    if (!mk_quesens_egts || !msg || !egts_connect) return ret;

    msgctl(sens_queue_id_egts, IPC_STAT, sens_mymsq_egts);//читаем статус очереди
    if (sens_mymsq_egts->msg_qnum > 0) {
        ret = msgrcv(sens_queue_id_egts,
                    &sens_mess_egts,
                    (sizeof(s_sens_COM) - sizeof(long)),
                    sens_mess_egts.mtype,
                    MSG_DONTWAIT);
        if (ret > 0) memcpy((uint8_t *)msg, (uint8_t *)&sens_mess_egts.msg, sizeof(sens_data_t));
    }

    return ret;
}
//--------------------------------------------------------------------------------
int msgPutEgts(void *str, int len)
{
int ret = 0;

    if (!mk_quesens_egts || !egts_connect) return ret;//msgType3 - magnit - 2
    //if (!mk_quesens_egts) return ret;//msgType3 - magnit - 2

    void *st = (void *)calloc(1, len);// + 1);
    if (!st) {
        //devError |= devMem;
        return ret;
    }
    memcpy(st, str, len);

    sens_data_t msg = {st, len};// + 1
    memcpy((uint8_t *)&sens_mess_egts.msg, (uint8_t *)&msg, sizeof(sens_data_t));
    sens_mess_egts.numb = sizeof(sens_data_t);

    if (!msgsnd(sens_queue_id_egts,
                &sens_mess_egts,
                (sizeof(s_sens_COM) - sizeof(long)),
                IPC_NOWAIT)) {
        ret = 1;//error
        //
        //add_to_elog(str, len);
        //
    } else {
        if (dbg != logOff) Report(1,"[%s] Error send packet to egts server : errno=%d (%s)\n", __func__, errno, strerror(errno));
    }

    return ret;
}
//--------------------------------------------------------------------------------
int delQuePrnEgts()
{
sens_data_t msg = {0};

    if (!mk_quesens_egts) return 0;

    while (msgGetEgts(&msg) > 0){
        if (msg.str) free(msg.str);
    }
    msgctl(sens_queue_id_egts, IPC_RMID, 0);

    mk_quesens_egts = false;

    if (dbg > logOn) Report(1, "[%s] delete sens queue with id:%d\n", __func__, sens_queue_id_egts);

    return 0; 
}
//--------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------
static void delBuf(s_mem_str *ptr)
{
    if (ptr) {
        if (ptr->stx) free(ptr->stx);
        ptr->size = 0;
    }
}
//---------------------------------------------------------------------------------------------------------------------
static size_t addBuf(s_mem_str *ptr, const char *src)
{
size_t ret = 0;

    if (ptr && src) {
        size_t slen = strlen(src);
        if (slen) {
            ptr->stx = (char *)realloc(ptr->stx, ptr->size + slen + 1);
            if (ptr->stx) {
                memcpy(&(ptr->stx[ptr->size]), src, slen);
                ptr->size += slen;
                ptr->stx[ptr->size] = 0;
            }
        }
        ret = ptr->size;
    }

    return ret;
}
//---------------------------------------------------------------------------------------------------------------------
static uint8_t CRC8EGTS(const uint8_t *uk, uint8_t len)
{
uint8_t crc = 0xFF;

    while (len--) crc = eCRC8Table[crc ^ *uk++];

    return crc;
}
//---------------------------------------------------------------------------------------------------
static uint16_t CRC16EGTS(const uint8_t *uk, uint16_t len)
{
uint16_t crc = 0xFFFF;

    while (len--) crc = (crc << 8) ^ eCrc16Table[(crc >> 8) ^ *uk++];

    return crc;
}
//---------------------------------------------------------------------------------------------------
static char *TimeToStr(time_t ct)
{
char *arba = ctime(&ct);

    arba[strlen(arba) - 1] = 0;

    return (arba);
}
//-----------------------------------------------------------------------
static const char *PackName(uint8_t pt)
{
    switch (pt) {
        case EGTS_PT_RESPONSE://0
            return "EGTS_PT_RESPONSE";
        case EGTS_PT_APPDATA://1
            return "EGTS_PT_APPDATA";
        case EGTS_PT_SIGNED_APPDATA://2
            return "EGTS_PT_SIGNED_APPDATA";;
    }

    return "EGTS_PT_UNKNOWN";
}
//---------------------------------------------------------------------------------------------------
static const char *VHTName(uint32_t tip)
{
    switch (tip & 0x1f) {
        case 1:
            return "Class M1 (пассажирский)";
        case 2:
            return "Class M2 (автобус)";
        case 3:
            return "Class M3 (автобус)";
        case 4:
            return "Class N1 (легкая грузовая)";
        case 5:
            return "Class N2 (тяжелая грузовая)";
        case 6:
            return "Class N3 (тяжелая грузовая)";
        case 7:
            return "Class L1e (мотоцикл)";
        case 8:
            return "Class L2e (мотоцикл)";
        case 9:
            return "Class L3e (мотоцикл)";
        case 10:
            return "Class L4e (мотоцикл)";
        case 11:
            return "Class L5e (мотоцикл)";
        case 12:
            return "Class L6e (мотоцикл)";
        case 13:
            return "Class L7e (мотоцикл)";
    }

    return "Class Uknown";
}
//---------------------------------------------------------------------------------------------------
static const char *VPSTName(uint32_t tip)
{
    if ((tip & 0x3f) == 0) return "PowerType undefined";
         if (tip & 1) return "Бензин";
    else if (tip & 2) return "Дизел";
    else if (tip & 4) return "Сжиженный природный газ (CNG)";
    else if (tip & 8) return "Жидкий пропан (LPG)";
    else if (tip & 0x10) return "Электро. (более 42B и 100 А/ч)";
    else if (tip & 0x20) return "Водород";

    return "PowerType unknown";

}
//-----------------------------------------------------------------------
static const char *ServName(uint8_t stp)
{
    switch (stp) {
        case EGTS_AUTH_SERVICE://         (1)
            return "EGTS_AUTH_SERVICE";
        case EGTS_TELEDATA_SERVICE://     (2)
            return "EGTS_TELEDATA_SERVICE";
        case EGTS_COMMANDS_SERVICE://     (4)
            return "EGTS_COMMANDS_SERVICE";
        case EGTS_FIRMWARE_SERVICE://     (9)
            return "EGTS_FIRMWARE_SERVICE";
        case EGTS_ECALL_SERVICE://        (10)
            return "EGTS_ECALL_SERVICE";
    }

    return "EGTS_UNKNOWN_SERVICE";
}
//-----------------------------------------------------------------------
static const char *SRVRPName(uint8_t prio)//Service Routing Priority
{
    switch (prio & 3) {
        case 0:
            return "TopPriority";
        case 1:
            return "HighPriority";
        case 2:
            return "MidlePriority";
        case 3:
            return "LowPriority";
    }

    return "UnknownPriority";
}
//-----------------------------------------------------------------------
static const char *SSTName(uint8_t stat)
{
    switch (stat) {
        case EGTS_SST_IN_SERVICE://0, //сервис в рабочем состоянии и разрешен к использованию
            return "EGTS_SST_IN_SERVICE";
        case EGTS_SST_OUT_OF_SERVICE://128, //сервис в нерабочем состоянии (выключен)
            return "EGTS_SST_OUT_OF_SERVICE";
        case EGTS_SST_DENIED://129//сервис запрещен для использования
            return "EGTS_SST_DENIED";
        case EGTS_SST_NO_CONF://130//сервис не настроен
            return "EGTS_SST_NO_CONF";
        case EGTS_SST_TEMP_UNAVAIL://131//сервис временно недоступен
            return "EGTS_SST_TEMP_UNAVAIL";
    }

    return "EGTS_SST_UNKNOWN";
}
//----------------------------------------------------------------------
static const char *SubRecName(uint8_t srt)
{
    switch (srt) {
        case EGTS_SR_RECORD_RESPONSE:
            return "EGTS_SR_RECORD_RESPONSE";//(0)
        case EGTS_SR_TERM_IDENTITY:
            return "EGTS_SR_TERM_IDENTITY";//(1)
        case EGTS_SR_MODULE_DATA:
            return "EGTS_SR_MODULE_DATA";//(2)
        case EGTS_SR_VEHICLE_DATA:
            return "EGTS_SR_VEHICLE_DATA";//(3)

        case EGTS_SR_DISPATCHER_IDENTITY:
            return "EGTS_SR_DISPATCHER_IDENTITY";//5
        case EGTS_SR_AUTH_PARAMS:
            return "EGTS_SR_AUTH_PARAMS";//(6)
        case EGTS_SR_AUTH_INFO:
            return "EGTS_SR_AUTH_INFO";//(7)
        case EGTS_SR_SERVICE_INFO:
            return "EGTS_SR_SERVICE_INFO";//(8)
        case EGTS_SR_RESULT_CODE:
            return "EGTS_SR_RESULT_CODE";//(9)
        case EGTS_SR_POS_DATA:

            return "EGTS_SR_POS_DATA";//16
        case EGTS_SR_EXT_POS_DATA:
            return "EGTS_SR_EXT_POS_DATA";//17
        case EGTS_SR_AD_SENSORS_DATA:
            return "EGTS_SR_AD_SENSORS_DATA";//18
        case EGTS_SR_COUNTERS_DATA:
            return "EGTS_SR_COUNTERS_DATA";//19
        case EGTS_SR_ACCEL_DATA:
            return "EGTS_SR_ACCEL_DATA";//20
        case EGTS_SR_STATE_DATA:
            return "EGTS_SR_STATE_DATA";//21
        case EGTS_SR_LOOPIN_DATA:
            return "EGTS_SR_LOOPIN_DATA";//22
        case EGTS_SR_ABS_DIG_SENS_DATA:
            return "EGTS_SR_ABS_DIG_SENS_DATA";//23
        case EGTS_SR_ABS_AN_SENS_DATA:
            return "EGTS_SR_ABS_AN_SENS_DATA";//24
        case EGTS_SR_ABS_CNTR_DATA:
            return "EGTS_SR_ABS_CNTR_DATA";//25
        case EGTS_SR_ABS_LOOPIN_DATA:
            return "EGTS_SR_ABS_LOOPIN_DATA";//26
        case EGTS_SR_LIQUID_LEVEL_SENSOR:
            return "EGTS_SR_LIQUID_LEVEL_SENSOR";//27
        case EGTS_SR_PASSENGERS_COUNTERS:
            return "EGTS_SR_PASSENGERS_COUNTERS";//28
    }

    return "EGTS_SR_UNKNOWN";
}
//---------------------------------------------------------------------------------------------------
static char *EpochToStr(uint32_t epoch, char *ts)
{
    if (ts) {
        const time_t ep = (time_t)epoch;
        struct tm *ct = localtime(&ep);
        sprintf(ts, "%02d:%02d:%02d %02d.%02d.%04d",
                    ct->tm_hour, ct->tm_min, ct->tm_sec,
                    ct->tm_mday, ct->tm_mon + 1, ct->tm_year + 1900);
    }

    return ts;
}
//---------------------------------------------------------------------------------------------------
void disconnectEGTS(int *esoc, uint8_t pr)
{
int s = *esoc;
sens_data_t msg = {0};

    if (s > 0) {
        shutdown(s, SHUT_RDWR);
        close(s);
        *esoc = -1;
        //
        egts_connect = false;
        //delQuePrnEgts();
        //if (!mk_quesens_egts) return 0;

        while (msgGetEgts(&msg) > 0) {
            if (msg.str) free(msg.str);
        }   
        //
        if (pr) {
            if (dbg != logOff) Report(1, "[%s] Disconnect from egts server (sock %d)\n", __func__, s);
        }
    }
}
//---------------------------------------------------------------------------------------------------
int connectEGTS(char *eurl, uint16_t eport)
{
int esoc = -1;
uint16_t tport = eport;//, t_p;
struct sockaddr_in srv_conn;
socklen_t srvlen;
struct hostent *hostPtr = NULL;
char line[128] = {0};


    esoc = socket(AF_INET, SOCK_STREAM, 6);
    if (esoc < 0) {
        if (dbg != logOff) Report(1, "[%s] FATAL ERROR: open socket (%d)\n", __func__, esoc);
        return -1;
    } else if (esoc > FD_SETSIZE) {
        if (dbg != logOff) Report(1, "[%s:%s] FD_SETSIZE ERROR: open socket (%d)\n", dev, __func__, esoc);
        return -1;
    }

    strcpy(line, eurl);

    /**/
    hostPtr = gethostbyname(line);
    if (!hostPtr) {
        hostPtr = gethostbyaddr(line, strlen(line), AF_INET);
        if (!hostPtr) {
            if (dbg != logOff) Report(1,"[%s] ERROR resolving egts server address '%s:%u' (%s)\n", __func__, line, tport, strerror(errno));
            return -1;
        }
    }
    /**/

    srvlen = sizeof(struct sockaddr);
    memset(&srv_conn, 0, srvlen);
    srv_conn.sin_family = AF_INET;
    srv_conn.sin_port   = htons(tport);
    (void)memcpy(&srv_conn.sin_addr, hostPtr->h_addr, hostPtr->h_length);
    //srv_conn.sin_addr.s_addr = inet_addr(line);

    if (connect(esoc, (struct sockaddr *)&srv_conn, srvlen) == -1) {
        if (dbg != logOff) Report(1, "[%s] ERROR: connect to egts '%s:%d' (%s)\n", __func__, line, tport, strerror(errno));
        esoc = -1;
    } else {
        fcntl(esoc, F_SETFL, (fcntl(esoc, F_GETFL)) | O_NONBLOCK);
        if (dbg != logOff) Report(1, "[%s] Connect to egts '%s:%d' OK (sock %d)\n", __func__, line, tport, esoc);
    }


    return esoc;
}
//---------------------------------------------------------------------------------------------------
int make_ack_EGTS(uint8_t *buf, uint32_t num, uint16_t rn, uint16_t pid, uint8_t res)
{
s_min_hdr *hdr = (s_min_hdr *)buf;
int dl = sizeof(s_min_hdr);

    memset(buf, 0, sizeof(s_min_hdr));
    hdr->PRV = 1;
    hdr->SKID = 0;
    //hdr->PR = 0;unsigned PR:2;//Priority: 00-высший, 01-высокий, 10-средний, 11-низкий
    //hdr->CMP = 0;//unsigned CMP:1;//Compressed
    //hdr->ENA = 0;//unsigned ENA:2;//Encryption Algorithm
    //hdr->RTE = 0;//RTE (Route)
    //hdr->PRF = 0;//unsigned RTE:1;//1-PeerAddress+RecipientAddress+TTL present
    //hdr->PRF = 0;//unsigned PRF:2;//Prefix=0x00
    hdr->HL = sizeof(s_min_hdr);// 11 //uint8_t HL;//Header Length
    //hdr->HE = 0;//uint8_t HE;//Header Encoding
    hdr->FDL = sizeof(s_ack_SFRD);// + sizeof(s_ack_RCODE);//uint16_t FDL;//Frame Data Length
    hdr->PID = pid;//(uint16_t)num;//uint16_t PID;//Packet Identifier
    hdr->PT = EGTS_PT_RESPONSE;// Тип пакета Транспортного Уровня //uint8_t PT;//Packet Type
    hdr->HCS = CRC8EGTS((uint8_t *)hdr, hdr->HL - 1);//uint8_t HCS;//Header Check Sum
//SFRD
    s_ack_SFRD *ack = (s_ack_SFRD *)(buf + dl);
    memset((uint8_t *)ack, 0, sizeof(s_ack_SFRD));
    ack->RPID = rn;//pid;
    ack->RESULT = res;
    dl += sizeof(s_ack_SFRD);
//SFRCS
    uint16_t word = CRC16EGTS((uint8_t *)ack, sizeof(s_ack_SFRD));
    memcpy(buf + dl, &word, sizeof(uint16_t));
    dl += sizeof(uint16_t);

    if (dbg > logDebug)
        Report(1, "[%s]:\n\tPRV:0x%02X\n\tSKID:0x%02X\n\tFlags:0x%02X\n"
                         "\t\tPRF:%02u RTE:%u ENA:%02u CMP:%u PR:%02u\n"
                         "\tHL:%u\n\tHE:%u\n\tFDL:%u(0x%04X)\n\tPID:%u(0x%04X)\n"
                         "\tPT:%u\t'%s'\n\tHCS:0x%02X\n"
                         "\tRPID:%u(0x%04X) RESULT:%u\n"
                         "\tCRC16:0x%04X\n",
                    __func__, hdr->PRV, hdr->SKID, *(buf + 2),
                    hdr->PRF, hdr->RTE, hdr->ENA, hdr->CMP, hdr->PR,
                    hdr->HL, hdr->HE, hdr->FDL, hdr->FDL,
                    hdr->PID, hdr->PID, hdr->PT, PackName(hdr->PT), hdr->HCS,
                    ack->RPID, ack->RPID, ack->RESULT,
                    word);

    return dl;
}
//---------------------------------------------------------------------------------------------------
int parse_from_EGTS(uint8_t *buf, int len, uint8_t *e_snd, uint16_t *e_rn, uint16_t *e_pid)
{
s_min_hdr *hdr = (s_min_hdr *)buf;
int i, ret = -1, dli = 0;
uint8_t ptype = hdr->PT;
int8_t er = 0;
uint32_t tim = 0, oid = 0, evid = 0;
uint8_t sst, rst, rtype, byte, snd = 0, no_auth = 0, pt = 0;
uint16_t recRN = 0, crc16, rlen = 0, rPID = 0, hPID = hdr->PID;
uint8_t *uki, *rflag;
int dl = (int)sizeof(s_min_hdr);
uint8_t *uk = buf;
uint8_t *uke = buf + len - 2 - 1;//указатель на последний байт данных
char vrem[512] = {0};
char tmp[256] = {0};
char srst[2560];
char stx[1024];
uint8_t dbgf = dbg;
s_mem_str t_stx = {NULL, 0};
s_mem_str t_srst = {NULL, 0};


    memcpy(&crc16, buf + len - 2, 2);
    uint8_t CRC8 = CRC8EGTS (uk, hdr->HL - 1);
    uk += dl;//uk to REC_hdr
    uint16_t CRC16 = CRC16EGTS(uk, len - dl - 2);
    if (CRC8  != hdr->HCS) er |= 1;
    if (CRC16 != crc16)    er |= 2;

    if (dbgf > logDebug)
        Report(1, "\tPRV:0x%02X\n\tSKID:0x%02X\n\tFlags:0x%02X\n"
                "\t\tPRF:%u RTE:%u ENA:%u CMP:%u PR:%u\n"
                "\tHL:%u\n\tHE:%u\n\tFDL:%u(0x%04X)\n\tPID:%u(0x%04X)\n"
                "\tPT:%u\t'%s'\n\tHCS:0x%02X/0x%02X\n",
                hdr->PRV, hdr->SKID, *(buf + 2),
                hdr->PRF, hdr->RTE, hdr->ENA, hdr->CMP, hdr->PR,
                hdr->HL, hdr->HE, hdr->FDL, hdr->FDL,
                hdr->PID, hdr->PID, hdr->PT, PackName(ptype), hdr->HCS, CRC8);


    switch (ptype) {
        case EGTS_PT_RESPONSE:
        {
            s_ack_SFRD *ack_SFRD = (s_ack_SFRD *)uk;
            if (dbgf > logDebug) {
                sprintf(stx, "REC(%u) : '%s'\n\tRPID:%u(0x%04X) RESULT:%u\n",
                             hdr->FDL, PackName(ptype), ack_SFRD->RPID, ack_SFRD->RPID, ack_SFRD->RESULT);
                addBuf(&t_stx, stx);
            }
            ret = ack_SFRD->RESULT;
            uki = uk + sizeof(s_ack_SFRD);
            memset(tmp, 0, sizeof(tmp));
            while (uki <= uke) {
                memset(srst, 0, sizeof(srst));
                pt = 0;
                rtype = *uki++;
                switch (rtype) {
                    //
                    case EGTS_SR_AUTH_PARAMS ://    (6)
                    {
                        pt = 1;
                        if (dbgf >= logDebug) sprintf(srst,"\tSubRecord '%s' : rType(%u)\n", SubRecName(rtype), rtype);
                        uint16_t uBIT16;
                        s_auth_params *auth_params = (s_auth_params *)uki;
                        if (dbgf >= logDebug) sprintf(srst+strlen(srst),
                                       "\t\tFlags:0x%02X - EXE:%u SSE:%u MSE:%u ISLE:%u PKE:%u ENA:%u\n",
                                       *uki,
                                       auth_params->EXE, auth_params->SSE, auth_params->MSE,
                                       auth_params->ISLE, auth_params->PKE, auth_params->ENA);
                        uki++;
                        if (auth_params->PKE) {
                            uBIT16 = *(uint16_t *)uki;
                            uint16_t ul = uBIT16; if (ul > 512) ul = 512;
                            uki += sizeof(uint16_t);//uk to PBK
                            if (dbgf > logDebug) {
                                sprintf(srst+strlen(srst), "\t\t\tPKL:%u PBK:", uBIT16);
                                int dlt = strlen(srst);
                                if ( (sizeof(srst) - dlt) > (ul << 1) ) {
                                    for (int i = 0; i < ul; i++) sprintf(srst+strlen(srst), "%02X", *(uki + i));
                                }
                                strcat(srst, "\n");
                            }
                            uki += uBIT16;
                        }
                        if (auth_params->ISLE) {
                            uBIT16 = *(uint16_t *)uki;
                            if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\t\tISL:%u\n", uBIT16);
                            uki += sizeof(uint16_t);//uk to MSZ
                        }
                        if (auth_params->MSE) {
                            uBIT16 = *(uint16_t *)uki;
                            if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\t\tMSZ:%u\n", uBIT16);
                            uki += sizeof(uint16_t);//uk to SS
                        }
                        if (auth_params->SSE) {
                            dl = snprintf(vrem, sizeof(vrem)-1, "%s", uki);
                            if (dbgf > logDebug)
                                if (dl) sprintf(srst+strlen(srst),"\t\t\tSS:%s\n", vrem);
                            uki += dl + 1;
                        }
                        if (auth_params->EXE) {
                            dl = snprintf(vrem, sizeof(vrem)-1, "%s", uki);
                            if (dbgf > logDebug)
                                if (dl) sprintf(srst+strlen(srst),"\t\t\tEXP:%s\n", vrem);
                            uki += dl + 1;
                        }
                    }
                    break;
                        default : {
                            pt = 0;
                            if (dbgf > logDebug) {
                                if (strlen(tmp) < sizeof(tmp) - 4) sprintf(tmp+strlen(tmp),"%02X", rtype);
                            }
                        }
                }//switch (rtype)
                if (pt && (dbgf > logDebug)) addBuf(&t_stx, srst);
            }//while (uki <= uke)
            if ( strlen(tmp) && (dbgf > logDebug) ) {
                sprintf(stx, "%s\n", tmp);
                addBuf(&t_stx, stx);
            }
        }
        break;//EGTS_PT_RESPONSE
        case EGTS_PT_APPDATA:
        {
            s_mrec_hdr *rec_hdr = (s_mrec_hdr *)uk;
            recRN = rec_hdr->RN;
            rflag = uk + sizeof(s_mrec_hdr) - 1;
            uki = uk + sizeof(s_mrec_hdr);//uk to OID
            if (rec_hdr->OBFE) {
                memcpy(&oid, uki, sizeof(uint32_t));
                uki += sizeof(uint32_t);//uk to EVID
            }
            if (rec_hdr->EVFE) {
                memcpy(&evid, uki, sizeof(uint32_t));
                uki += sizeof(uint32_t);//uk to TM
            }
            if (rec_hdr->TMFE) {
                memcpy(&tim, uki, sizeof(uint32_t));
                uki += sizeof(uint32_t);//uk to SST
                tim += UTS2010;
            }
            sst = *uki++;
            rst = *uki++;
            //uki -> pointer to Record Data (record type)
            rtype = *uki++;//get SRT (Subrecord Type)
            memcpy(&rlen, uki, sizeof(uint16_t));//get SRL (Subrecord Length)
            uki += sizeof(uint16_t);//uk to SRD (Subrecord Data)

            switch (rst) {// switch by Service Type  RST
                case EGTS_AUTH_SERVICE ://        (1)
                    //memset(srst, 0, sizeof(srst));
                    while (uki <= uke) {
                        if (dbgf > logDebug) sprintf(srst, "\tSubRecord '%s' : rLen:%u rType(%u)\n", SubRecName(rtype), rlen, rtype);
                        switch (rtype) {
                            case EGTS_SR_RECORD_RESPONSE:// (0)
                                rPID = *(uint16_t *)uki;
                                uki += sizeof(uint16_t);
                                byte = *uki++;
                                if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\tCRN %u for RN %u with RS %u\n", rPID, recRN, byte);
                            break;
                            case EGTS_SR_SERVICE_INFO://    (8)
                            {
                                s_sr_info *sr_info = (s_sr_info *)uki;
                                byte = *(uki + 2);
                                if (dbgf > logDebug) {
                                    if (sr_info->SRVA) strcpy(vrem, "Requested Service");
                                                  else strcpy(vrem, "Supported Service");
                                    sprintf(srst+strlen(srst), "\t\tST(%u):'%s' SST(%u):'%s' SRVRP(%u):'%s' SRVA(%u):'%s'\n",
                                                sr_info->ST, ServName(sr_info->ST), sr_info->SST, SSTName(sr_info->SST),
                                                sr_info->SRVRP, SRVRPName(byte), sr_info->SRVA, vrem);
                                }
                                uki += rlen;
                            }
                            break;
                            case EGTS_SR_RESULT_CODE://     (9)
                                no_auth = *uki;
                                if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\tRCD:%u\n", no_auth);
                                uki += rlen;
                                ret = no_auth;
                            break;
                            case EGTS_SR_AUTH_PARAMS ://    (6)
                            {
                                uint16_t uBIT16;
                                s_auth_params *auth_params = (s_auth_params *)uki;
                                if (dbgf > logDebug) sprintf(srst+strlen(srst),
                                               "\t\tFlags:0x%02X - EXE:%u SSE:%u MSE:%u ISLE:%u PKE:%u ENA:%u\n",
                                               *uki,
                                               auth_params->EXE, auth_params->SSE, auth_params->MSE,
                                               auth_params->ISLE, auth_params->PKE, auth_params->ENA);
                                uki++;
                                if (auth_params->PKE) {
                                    uBIT16 = *(uint16_t *)uki;
                                    if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\t\tPKL:%u PBK:", uBIT16);
                                    uki += sizeof(uint16_t);//uk to PBK
                                    if (dbgf > logDebug) {
                                        for (int i = 0; i < uBIT16; i++) sprintf(srst+strlen(srst), "%02X", *(uki + i));
                                        strcat(srst, "\n");
                                    }
                                    uki += uBIT16;
                                }
                                if (auth_params->ISLE) {
                                    uBIT16 = *(uint16_t *)uki;
                                    if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\t\tISL:%u\n", uBIT16);
                                    uki += sizeof(uint16_t);//uk to MSZ
                                }
                                if (auth_params->MSE) {
                                    uBIT16 = *(uint16_t *)uki;
                                    if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\t\tMSZ:%u\n", uBIT16);
                                    uki += sizeof(uint16_t);//uk to SS
                                }
                                if (auth_params->SSE) {
                                    //memset(vrem, 0, sizeof(vrem));
                                    dl = snprintf(vrem, sizeof(vrem)-1, "%s", uki);   //if (dl > 255) vrem[255] = '\0';   dl = strlen(vrem);
                                    if (dbgf > logDebug)
                                        if (dl) sprintf(srst+strlen(srst),"\t\t\tSS:%s\n", vrem);
                                    uki += dl + 1;
                                }
                                if (auth_params->EXE) {
                                    //memset(vrem, 0, sizeof(vrem));
                                    dl = snprintf(vrem, sizeof(vrem)-1, "%s", uki);   //if (dl > 255) vrem[255] = '\0';   dl = strlen(vrem);
                                    if (dbgf > logDebug)
                                        if (dl) sprintf(srst+strlen(srst),"\t\t\tEXP:%s\n", vrem);
                                    uki += dl + 1;
                                }
                            }
                            break;
                            case EGTS_SR_AUTH_INFO://       (7)
                            {
                                uint8_t *ukz = uki;
                                dl = snprintf(vrem, sizeof(vrem)-1, "%s", uki);
                                if (dbgf > logDebug) sprintf(srst+strlen(srst), "\t\tUNM:'%s'", vrem);//User Name
                                uki += dl + 1;//uk to UPSW
                                dl = snprintf(vrem, sizeof(vrem)-1, "%s", uki);
                                if (dbgf > logDebug) sprintf(srst+strlen(srst), " UPSW:'%s'", vrem);//User Password
                                uki += dl + 1;//uk to SS
                                if (rlen > (uki - ukz)) { 
                                    dl = snprintf(vrem, sizeof(vrem)-1, "%s", uki);
                                    if (dbgf > logDebug) sprintf(srst+strlen(srst), " SS:'%s'", vrem);//Server Sequence
                                    uki += dl + 1;
                                }
                                if (dbgf > logDebug) strcat(srst, "\n");
                            }
                            break;
                                default : uki += rlen;
                        }//switch (rtype) //switch by SubRecord Type  SRT
                        if (uke > uki) {
                            rtype = *uki++;                       // get SRT (Subrecord Type)
                            memcpy(&rlen, uki, sizeof(uint16_t)); // get SRL (Subrecord Length)
                            uki += sizeof(uint16_t);              // uk to SRD (Subrecord Data)
                        }
                        if (dbgf > logDebug) addBuf(&t_srst, srst);
                    }//while (uki <= uke)
                    //
                    if (dbgf > logDebug) {
                        sprintf(stx, "REC(%u) : '%s'\n\tRL:%u(0x%04X) RN:%u\n"
                             "\trFlag:0x%02X\n\t\tSSOD:%u RSOD:%u GRP:%u RPP:%02u TMFE:%u EVFE:%u OBFE:%u\n",
                             hdr->FDL, PackName(ptype), rec_hdr->RL, rec_hdr->RL, rec_hdr->RN,
                             *rflag, rec_hdr->SSOD, rec_hdr->RSOD, rec_hdr->GRP,
                             rec_hdr->RPP, rec_hdr->TMFE, rec_hdr->EVFE, rec_hdr->OBFE);
                        if (rec_hdr->OBFE) sprintf(stx+strlen(stx),"OID:%u ", oid);
                        if (rec_hdr->EVFE) sprintf(stx+strlen(stx),"EVID:%u ", evid);
                        if (rec_hdr->TMFE) sprintf(stx+strlen(stx),"TM:'%s' (%u) ", TimeToStr((time_t)tim), tim);
                        sprintf(stx+strlen(stx),"\tSST:%u RST:%u\n", sst, rst);
                        addBuf(&t_stx, stx);
                        //addBuf(&t_srst, "\n");
                        addBuf(&t_stx, t_srst.stx);
                    }
                    snd = 1;
                    //
                break;//end EGTS_AUTH_SERVICE
                /*case EGTS_TELEDATA_SERVICE ://    (2)
                case EGTS_COMMANDS_SERVICE ://    (4)
                case EGTS_FIRMWARE_SERVICE ://    (9)
                case EGTS_ECALL_SERVICE    ://    (10)
                break;*/
                    default : {
                        if (dbgf > logDebug) {
                            dli = (sizeof(stx) >> 1) - 3 - (sprintf(stx, "UNSUPPORTED SERVICE '%s'(%u), all data :\n", ServName(rst), rst));
                            if (dli > len) dli = len;
                            for (i = 0; i < dli; i++) sprintf(stx+strlen(stx), "%02X", buf[i]);
                            strcat(stx, "\n");
                            addBuf(&t_stx, stx);
                        }
                    }
            }//switch (rst)
        }
        break;//end EGTS_PT_APPDATA
            default : {
                if (dbgf > logDebug) {
                    dli = (sizeof(stx) >> 1) - 3 - (sprintf(stx, "REC(%u) : '%s'\n", hdr->FDL, PackName(ptype)));
                    if (dli > len) dli = len;
                    for (i = 0; i < dli; i++) sprintf(stx+strlen(stx), "%02X", buf[i]);
                    strcat(stx, "\n");
                    addBuf(&t_stx, stx);
                }
            }
    }//switch (ptype)

    if (dbgf > logDebug) {
        sprintf(stx, "\tCRC16:0x%04X/0x%04X\n-------------------------\n", crc16, CRC16);
        addBuf(&t_stx, stx);
        Report(1, t_stx.stx);
#ifdef PRINT_BUF_SIZE
        Report(1, "[%s] stx size = %lu srst size = %ld)\n", __func__, t_stx.size, t_srst.size);
#endif
    }

    delBuf(&t_srst);
    delBuf(&t_stx);

    if (snd) {
        *e_snd = snd;
        *e_rn = recRN;
        *e_pid = hPID;
    }

    if (er) ret = er;

    return ret;
}
//---------------------------------------------------------------------------------------------------
int mkPack(s_list_rec *lst, uint8_t *buf, uint32_t num, s_loc_data *sloc, uint8_t *err, uint8_t pr)
{
int len = 0, dl = 0, ix = 0;
s_min_hdr *min_hdr = (s_min_hdr *)buf;
s_rec_hdr *rec_hdr = NULL;
uint8_t er = 0, stype = lst->SERVICE, subRec = 255, hflags = 0, flags = 0, hrflags = 0;
uint8_t *urec  = buf + sizeof(s_min_hdr) + sizeof(s_rec_hdr);
uint8_t dbgf = dbg;
s_mem_str t_stx = {NULL, 0};
char stx[1200] = {0};


    if (stype == 255) {
        er = 1;
        *err = er;
        return len;
    }

    memset((uint8_t *)min_hdr, 0, sizeof(s_min_hdr));
    min_hdr->PRV = 1;
    min_hdr->HL  = sizeof(s_min_hdr);
    min_hdr->FDL = sizeof(s_rec_hdr);//edit in addRec()
    min_hdr->PID = (uint16_t)num;
    min_hdr->PT  = EGTS_PT_APPDATA;
    min_hdr->HCS = 0;//CRC8EGTS (buf, min_hdr->HL - 1); !!!!! <- calc in the end proc
    len         += sizeof(s_min_hdr);
    hflags       = *(buf + 2);

    rec_hdr         = (s_rec_hdr *)(buf + sizeof(s_min_hdr));
    memset((uint8_t *)rec_hdr, 0, sizeof(s_rec_hdr));
    rec_hdr->RL     = 0;//edit in addRec() //sizeof(s_term_id) + size_imei + sizeof(s_sr_veh_data);
    rec_hdr->RN     = (uint16_t)num;
    rec_hdr->SSOD   = 0;
    rec_hdr->TMFE   = 1;
    rec_hdr->OBFE   = 0;
    //rec_hdr->OID    = 1;//cl->car_id;
    rec_hdr->TM     = (uint32_t)time(NULL);
    rec_hdr->TM    -= UTS2010;
    rec_hdr->SST    = stype;//EGTS_AUTH_SERVICE or EGTS_TELEDATA_SERVICE
    rec_hdr->RST    = stype;//EGTS_AUTH_SERVICE or EGTS_TELEDATA_SERVICE
    len            += sizeof(s_rec_hdr);
    hrflags         = *(buf + sizeof(s_min_hdr) + 4);

    //------------------------------------------------------------------------
    while (ix < maxList) {
        subRec = lst->SR[ix];
        if (subRec != 255) {
            memset(stx, 0, sizeof(stx));
            //--------------------------------------------------------------------
            switch (subRec) {
                case EGTS_SR_TERM_IDENTITY :
                {
                    dl = sizeof(s_term_id) + imei_size;
                    s_term_id *term_id = (s_term_id *)urec;
                    memset(urec, 0, dl + 1);
                    term_id->SID   = EGTS_SR_TERM_IDENTITY;
                    term_id->Len   = imei_size + sizeof(s_term_id) - sizeof(uint16_t) - sizeof(uint8_t);//20;
                    term_id->IMEIE = 1;
                    term_id->TID = termID;
                    flags  = *(urec + 7);
                    uint8_t *uimei = urec + sizeof(s_term_id);
                    memcpy(uimei, dev, imei_size);
                    if (dbgf >= logDebug) {
                        sprintf(stx,
                            "\tSUB_REC(%d): %s\n\t\tSID:%u Len:%u(0x%04X)\n\t\tTID:%u(0x%08X)\n"
                            "\t\tFlags:0x%02X\n"
                            "\t\t\tMNE:%u BSE:%u NIDE:%u SSRA:%u LNGCE:%u IMSIE:%u IMEIE:%u HDIDE:%u\n"
                            "\t\tIMEI:%.*s\n",
                            dl, SubRecName(subRec),
                            term_id->SID, term_id->Len, term_id->Len, term_id->TID, term_id->TID,
                            flags,
                            term_id->MNE, term_id->BSE, term_id->NIDE,
                            term_id->SSRA, term_id->LNGCE, term_id->IMSIE,
                            term_id->IMEIE, term_id->HDIDE, imei_size,
                            uimei);
                    }
                    len += dl;

                    rec_hdr->RL  += dl;
                    min_hdr->FDL += dl;
                    urec  += dl;
                }
                break;
                case EGTS_SR_DISPATCHER_IDENTITY :
                {
                    dl = sizeof(s_sr_disp_id);
                    memset(urec, 0, dl);
                    s_sr_disp_id *sr_disp_id = (s_sr_disp_id *)urec;
                    sr_disp_id->SID = EGTS_SR_DISPATCHER_IDENTITY;
                    sr_disp_id->Len = sizeof(s_sr_disp_id) - sizeof(uint16_t) - sizeof(uint8_t);
                    sr_disp_id->DT = 0;//1;
                    sr_disp_id->DID = DispatcherID;
                    if (dbgf > logDebug)
#ifndef ARM                        
                        sprintf(stx, "\tSUB_REC(%lu): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                    "\t\t\tDT:%u DID:%u\n",
                                    sizeof(s_sr_disp_id), SubRecName(subRec), sr_disp_id->SID, sr_disp_id->Len, sr_disp_id->Len,
                                    sr_disp_id->DT, sr_disp_id->DID);
#else
                        sprintf(stx, "\tSUB_REC(%u): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                    "\t\t\tDT:%u DID:%u\n",
                                    sizeof(s_sr_disp_id), SubRecName(subRec), sr_disp_id->SID, sr_disp_id->Len, sr_disp_id->Len,
                                    sr_disp_id->DT, sr_disp_id->DID);               
#endif
                    len += dl;

                    rec_hdr->RL  += dl;
                    min_hdr->FDL += dl;
                    urec  += dl;
                }
                break;
                case EGTS_SR_SERVICE_INFO :
                {
                    dl = sizeof(s_sr_srv_info);
                    for (uint8_t si = 0; si < max_servCODE; si++) {//5 subRecord create !!!
                        memset(urec, 0, dl);
                        s_sr_srv_info *sr_srv_info = (s_sr_srv_info *)urec;
                        sr_srv_info->SID = EGTS_SR_SERVICE_INFO;
                        sr_srv_info->Len = sizeof(s_sr_srv_info) - sizeof(uint16_t) - sizeof(uint8_t);
                        sr_srv_info->ST  = servCODE[si];
                        sr_srv_info->SST = servUSE[si];
                        //sr_srv_info->SRVRP = 3;////Service Routing Priority : 00 - high priority, 11 - low priority
                        if (dbgf > logDebug) 
#ifndef ARM                            
                            sprintf(stx,
                                "\tSUB_REC(%lu): %s : SID:%u Len:%u(0x%04X)\n"
                                "\t\tST:'%s'(%u) SST:'%s'(%u) SRVRP:'%s'(%u) SRVA:'%s'(%u)\n",
                                sizeof(s_sr_srv_info), SubRecName(subRec), sr_srv_info->SID, sr_srv_info->Len, sr_srv_info->Len,
                                ServName(servCODE[si]), sr_srv_info->ST, SSTName(sr_srv_info->SST), sr_srv_info->SST,
                                SRVRPName(sr_srv_info->SRVRP), sr_srv_info->SRVRP, 
                                (sr_srv_info->SRVA) ? "Requested Service" : "Supported Service", //vr,
                                sr_srv_info->SRVA);
#else
                            sprintf(stx,
                                "\tSUB_REC(%u): %s : SID:%u Len:%u(0x%04X)\n"
                                "\t\tST:'%s'(%u) SST:'%s'(%u) SRVRP:'%s'(%u) SRVA:'%s'(%u)\n",
                                sizeof(s_sr_srv_info), SubRecName(subRec), sr_srv_info->SID, sr_srv_info->Len, sr_srv_info->Len,
                                ServName(servCODE[si]), sr_srv_info->ST, SSTName(sr_srv_info->SST), sr_srv_info->SST,
                                SRVRPName(sr_srv_info->SRVRP), sr_srv_info->SRVRP, 
                                (sr_srv_info->SRVA) ? "Requested Service" : "Supported Service", //vr,
                                sr_srv_info->SRVA);                   
#endif
                        len += dl;

                        rec_hdr->RL  += dl;
                        min_hdr->FDL += dl;
                        urec  += dl;
                    }
                }
                break;
                case EGTS_SR_VEHICLE_DATA :
                {
                    dl = sizeof(s_sr_veh_data);
                    memset(urec, 0, dl);
                    s_sr_veh_data *sr_veh_data = (s_sr_veh_data *)urec;
                    sr_veh_data->SID  = EGTS_SR_VEHICLE_DATA;// - 3
                    sr_veh_data->Len  = sizeof(s_sr_veh_data) - sizeof(uint16_t) - sizeof(uint8_t);
                    memset(sr_veh_data->VIN, 0, (int)sizeof(sr_veh_data->VIN));
                    //sprintf(sr_veh_data->VIN, "%d", cl->car_id);
                    //if (car_gnum) strcpy(sr_veh_data->VIN, car_gnum);
                    sr_veh_data->VHT  = 1;//0001 — пассажирский (Class М1)
                    sr_veh_data->VPST = 1;//Bit 0: 1 — бензин; Bit 1: 1 — дизель,
                    if (dbgf > logDebug)
#ifndef ARM
                        sprintf(stx,
                                "\tSUB_REC(%lu): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                "\t\t\tVIN:'%.*s' VHT(%u):'%s' VPST(%u):'%s'\n",
                                sizeof(s_sr_veh_data), SubRecName(subRec), sr_veh_data->SID, sr_veh_data->Len, sr_veh_data->Len,
                                (int)sizeof(sr_veh_data->VIN), sr_veh_data->VIN,
                                sr_veh_data->VHT, VHTName(sr_veh_data->VHT),
                                sr_veh_data->VPST, VPSTName(sr_veh_data->VPST));
#else
                        sprintf(stx,
                                "\tSUB_REC(%u): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                "\t\t\tVIN:'%.*s' VHT(%u):'%s' VPST(%u):'%s'\n",
                                sizeof(s_sr_veh_data), SubRecName(subRec), sr_veh_data->SID, sr_veh_data->Len, sr_veh_data->Len,
                                (int)sizeof(sr_veh_data->VIN), sr_veh_data->VIN,
                                sr_veh_data->VHT, VHTName(sr_veh_data->VHT),
                                sr_veh_data->VPST, VPSTName(sr_veh_data->VPST));               
#endif
                    len += dl;

                    rec_hdr->RL  += dl;
                    min_hdr->FDL += dl;
                    urec  += dl;
                }
                break;
                case EGTS_SR_POS_DATA :
                {
                    char ntm[EPOCH_STR_LEN];
                    uint32_t alt = 0, odm = 0;
                    float flat, flon;
                    dl = sizeof(s_sr_pos_data);
                    memset(urec, 0, dl);
                    s_sr_pos_data *sr_pos_data = (s_sr_pos_data *)urec;
                    sr_pos_data->SID = EGTS_SR_POS_DATA;
                    sr_pos_data->Len = sizeof(s_sr_pos_data) - sizeof(uint16_t) - sizeof(uint8_t);
                    //if (sloc) {
                        sr_pos_data->NTM  = sloc->NTM;
                        sr_pos_data->LAT  = sloc->LAT;
                        sr_pos_data->LONG = sloc->LONG;
                        sr_pos_data->VLD  = sloc->VLD;
                        sr_pos_data->MV   = sloc->MV;
                        sr_pos_data->LAHS = sloc->LAHS;
                        sr_pos_data->LOHS = sloc->LOHS;
                        sr_pos_data->DIR  = sloc->DIR & 0xff;
                        sr_pos_data->DIN  = sloc->DIN;
                        sr_pos_data->SPD  = (sloc->SPD * 10) & 0x3fff;
                        if (sloc->DIR > 0xff) sr_pos_data->SPD |= 0x8000;
                        if (sloc->ALT < 0) {
                            sr_pos_data->SPD |= 0x4000;
                            sloc->ALT *= -1;
                        }
                        alt = sloc->ALT;
                        memcpy(&sr_pos_data->ALT, &alt, 3);
                        odm = sloc->ODM * 10;
                        memcpy(&sr_pos_data->ODM, &odm, 3);
                    /*} else {
                        sr_pos_data->NTM  = (uint32_t)time(NULL);
                        sr_pos_data->LAT  = 0x17C013e9;
                        sr_pos_data->LONG = 0x47445B05;
                        sr_pos_data->VLD  = 1;
                        sr_pos_data->MV   = 0;
                        sr_pos_data->LAHS = 0;// north latitude
                        sr_pos_data->LOHS = 0;// ost longitude
                        sr_pos_data->SPD  = 0;
                        sr_pos_data->DIR  = 45;
                        sr_pos_data->DIN  = 0x81;
                        memset(sr_pos_data->ALT, 0, 3);
                        memset(sr_pos_data->ODM, 0, 3);
                    }*/
                    sr_pos_data->NTM -= UTS2010;
                    memcpy(&flat, &sr_pos_data->LAT, 4);
                    memcpy(&flon, &sr_pos_data->LONG, 4);
                    sr_pos_data->FIX  = 1;
                    sr_pos_data->CS   = 1;
                    sr_pos_data->BB   = 0;
                    sr_pos_data->ALTE = 1;
                    sr_pos_data->SRC  = 0;//0x23;//36 - const char *SrcLocation[] = {...,"неизвестно"};
                    flags = *(urec + 15);
                    if (dbgf > logDebug)
                        sprintf(stx,
                                "\tSUB_REC(%d): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                "\t\tNTM:%u(0x%08X)'%s'\n\t\tLAT:%f^ (0x%08X) LONG:%f^ (0x%08X)\n"
                                "\t\tFlags:0x%02X\n\t\t\tALTE:%u LOHS:%u LAHS:%u MV:%u BB:%u CS:%u FIX:%u VLD:%u\n"
                                "\t\tSPD:%u DIR:%u ODM:%u[%02X,%02X,%02X] DIN:%02X SRC:%u ALT:%u[%02X,%02X,%02X]\n",
                                (int)sizeof(s_sr_pos_data), SubRecName(subRec), sr_pos_data->SID, sr_pos_data->Len, sr_pos_data->Len,
                                sr_pos_data->NTM, sr_pos_data->NTM, EpochToStr(sr_pos_data->NTM, ntm),//ShowTime((time_t)sr_pos_data->NTM),
                                flat, sr_pos_data->LAT, flon, sr_pos_data->LONG,
                                flags, sr_pos_data->ALTE, sr_pos_data->LOHS, sr_pos_data->LAHS,
                                sr_pos_data->MV, sr_pos_data->BB, sr_pos_data->CS,
                                sr_pos_data->FIX, sr_pos_data->VLD,
                                sr_pos_data->SPD, sr_pos_data->DIR, odm,
                                sr_pos_data->ODM[0], sr_pos_data->ODM[1], sr_pos_data->ODM[2],
                                sr_pos_data->DIN, sr_pos_data->SRC, alt,
                                sr_pos_data->ALT[0], sr_pos_data->ALT[1], sr_pos_data->ALT[2]);
                    len += dl;

                    rec_hdr->RL  += dl;
                    min_hdr->FDL += dl;
                    urec  += dl;
                }
                break;
                case EGTS_SR_EXT_POS_DATA :
                {
                    dl = sizeof(s_sr_ext_pos_data);
                    memset(urec, 0, dl);
                    s_sr_ext_pos_data *sr_ext_pos_data = (s_sr_ext_pos_data *)urec;
                    sr_ext_pos_data->SID = EGTS_SR_EXT_POS_DATA;// - 17
                    sr_ext_pos_data->Len = sizeof(s_sr_ext_pos_data) - sizeof(uint16_t) - sizeof(uint8_t);//3
                    sr_ext_pos_data->SFE = 1;
                    sr_ext_pos_data->NSFE = 1;
                    //if (sloc) 
                    sr_ext_pos_data->SAT = sloc->SAT;
                    //     else sr_ext_pos_data->SAT = 12;
                    if (sloc->VLD) sr_ext_pos_data->NS = NS_GNSS | NS_GPS | NS_GALILEO;
                              else sr_ext_pos_data->NS = 0;
                    flags = *(urec + 3);
                    if (dbgf > logDebug)
                        sprintf(stx,
                                "\tSUB_REC(%d): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                "\t\tFlags:0x%02X\n\t\t\tNSFE:%u SFE:%u PFE:%u HFE:%u VFE:%u\n"
                                "\t\tSat:%u NS:%u\n",
                                (int)sizeof(s_sr_pos_data), SubRecName(subRec),
                                sr_ext_pos_data->SID, sr_ext_pos_data->Len, sr_ext_pos_data->Len,
                                flags, 
                                sr_ext_pos_data->NSFE, sr_ext_pos_data->SFE, sr_ext_pos_data->PFE, sr_ext_pos_data->HFE, sr_ext_pos_data->VFE,
                                sr_ext_pos_data->SAT, sr_ext_pos_data->NS);
                    len += dl;

                    rec_hdr->RL  += dl;
                    min_hdr->FDL += dl;
                    urec  += dl;
                }
                break;
                case EGTS_SR_AD_SENSORS_DATA :
                {
                    int i, j;
                    uint16_t word = 0;
                    dl = sizeof(s_sr_ad_sensors_data);
                    memset(urec, 0, sizeof(s_sr_ad_sensors_data));
                    s_sr_ad_sensors_data *sr_ad_sensors_data = (s_sr_ad_sensors_data *)urec;
                    sr_ad_sensors_data->SID = EGTS_SR_AD_SENSORS_DATA;
                    sr_ad_sensors_data->Len = sizeof(s_sr_ad_sensors_data) - sizeof(uint16_t) - sizeof(uint8_t);
                    sr_ad_sensors_data->DIOE = 0;
                    if (sloc) {
                        sr_ad_sensors_data->DOUT = sloc->DOUT;
                        sr_ad_sensors_data->ASFE = 0xff;
                        j = 1;
                        for (i = 0; i < 8; i++) {
                            if (sr_ad_sensors_data->ASFE & j) {
                                word = htons(sloc->AIN[i]);
                                memcpy(&sr_ad_sensors_data->ANS[i][1], &word, 2);
                            }
                            j <<= 1;
                        }
                    }
                    if (dbgf > logDebug) {
#ifndef ARM 
                        sprintf(stx,
                                    "\tSUB_REC(%lu): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                    "\t\tDIOE:0x%02X DOUT:0x%02X ASFE:0x%02X\n",
                                    sizeof(s_sr_ad_sensors_data), SubRecName(subRec),
                                    sr_ad_sensors_data->SID, sr_ad_sensors_data->Len, sr_ad_sensors_data->Len,
                                    sr_ad_sensors_data->DIOE, sr_ad_sensors_data->DOUT, sr_ad_sensors_data->ASFE);
#else
                        sprintf(stx,
                                    "\tSUB_REC(%u): %s\n\t\tSID:%u Len:%u(0x%04X)\n"
                                    "\t\tDIOE:0x%02X DOUT:0x%02X ASFE:0x%02X\n",
                                    sizeof(s_sr_ad_sensors_data), SubRecName(subRec),
                                    sr_ad_sensors_data->SID, sr_ad_sensors_data->Len, sr_ad_sensors_data->Len,
                                    sr_ad_sensors_data->DIOE, sr_ad_sensors_data->DOUT, sr_ad_sensors_data->ASFE);                   
#endif
                        for (i = 0; i < 8; i++) {
                            memcpy(&word, &sr_ad_sensors_data->ANS[i][1], 2);
                            word = htons(word);
                            sprintf(stx+strlen(stx),"\t\tANS[%u]:%u [%02X,%02X,%02X]\n",
                                                i, word,
                                                sr_ad_sensors_data->ANS[i][0],
                                                sr_ad_sensors_data->ANS[i][1],
                                                sr_ad_sensors_data->ANS[i][2]);
                        }
                        //addBuf(&t_stx, stx);
                    }

                    len += dl;

                    rec_hdr->RL  += dl;
                    min_hdr->FDL += dl;
                    urec  += dl;
                }
                break;
            }//switch (subRec)
            if (dbgf >= logDebug) addBuf(&t_stx, stx);
            //--------------------------------------------------------------------
        } else break;//(subRec == 255)
        ix++;
    }//while (ix < maxList)

    //----------------------------------------------------------------------------

    min_hdr->HCS = CRC8EGTS(buf, min_hdr->HL - 1);
    uint16_t bit16 = CRC16EGTS(buf + sizeof(s_min_hdr), len - sizeof(s_min_hdr));
    memcpy(buf + len, &bit16, 2);
    len += 2;// 2 - CRC16

    //----------------------------------------------------------------------------

    if (dbgf > logDebug) {
        sprintf(stx, "\tCRC16 : 0x%04X\n-------------------------------------\n", bit16);
        addBuf(&t_stx, stx);
        char tm[EPOCH_STR_LEN];
        Report(1,
                    "Hdr(%lu):\n\tPRV:0x%02X\n\tSKID:0x%02X\n\tFlags:0x%02X\n"
                    "\t\tPRF:%u RTE:%u ENA:%u CMP:%u PR:%u\n"
                    "\tHL:%u\n\tHE:%u\n\tFDL:%u(0x%04X)\n\tPID:%u(0x%04X)\n\tPT:%u\t'%s'\n\tHCS:0x%02X\n"
                    "REC(%lu) : '%s' RL:%u(0x%04X) RN:%u(0x%04X)\n"
                    "\tFlags:0x%02X\n\t\tSSOD:%u RSOD:%u GRP:%u RPP:%02u TMFE:%u EVFE:%u OBFE:%u\n"
                    "\tTM:%u(0x%08X)'%s' SST:%u RST:%u\n",
                    sizeof(s_min_hdr), min_hdr->PRV, min_hdr->SKID, hflags,
                    min_hdr->PRF, min_hdr->RTE, min_hdr->ENA, min_hdr->CMP, min_hdr->PR,
                    min_hdr->HL, min_hdr->HE, min_hdr->FDL, min_hdr->FDL,
                    min_hdr->PID, min_hdr->PID, min_hdr->PT, PackName(min_hdr->PT), min_hdr->HCS,
                    sizeof(s_rec_hdr), ServName(stype), rec_hdr->RL, rec_hdr->RL, rec_hdr->RN, rec_hdr->RN,
                    hrflags, rec_hdr->SSOD, rec_hdr->RSOD, rec_hdr->GRP,
                    rec_hdr->RPP, rec_hdr->TMFE, rec_hdr->EVFE, rec_hdr->OBFE,
                    //rec_hdr->OID,
                    rec_hdr->TM, rec_hdr->TM, EpochToStr(rec_hdr->TM, tm),
                    rec_hdr->SST, rec_hdr->RST);
        //Report(1, stx);
        Report(1, t_stx.stx);

#ifdef PRINT_BUF_SIZE
        Report(1, "[%s] stx size = %lu\n", __func__, t_stx.size);
#endif

    }

    //----------------------------------------------------------------------------

    *err = er;

    delBuf(&t_stx);

    return len;
}
//---------------------------------------------------------------------------------------------------
void locDataPrn(s_loc_data *sloc)
{
    if (!sloc) return;

    char *stx = (char *)calloc(1, LEN_1K);
    if (stx) {
        char tm[EPOCH_STR_LEN] = {0};
        char ntm[EPOCH_STR_LEN] = {0};
        sprintf(stx, "Data to EGTS:\n\tTM:%u(0x%08X)'%s'\n\tNTM:%u(0x%08X)'%s'\n"
                 "\tLAT:%f(0x%08X) LONG:%f(0x%08X)\n "
                 "\tSPD:%u DIR:%u ALT:%d ODM:%u DIN:0x%02X SAT:%u\n"
                 "\tVLD:%u MV:%u LAHS:%u LOHS:%u\n"
                 "\tDOUT:%02X\n",
                sloc->TM, sloc->TM, EpochToStr(sloc->TM, tm),
                sloc->NTM, sloc->NTM, EpochToStr(sloc->NTM, ntm),
                (float)sloc->LAT, sloc->LAT, (float)sloc->LONG, sloc->LONG,
                sloc->SPD, sloc->DIR, sloc->ALT, sloc->ODM, sloc->DIN, sloc->SAT,
                sloc->VLD, sloc->MV, sloc->LAHS, sloc->LOHS, sloc->DOUT);
        for (int8_t i = 0; i < 8; i++) sprintf(stx+strlen(stx),"\tAIN[%u]:%u(%04X)\n", i + 1, sloc->AIN[i], sloc->AIN[i]);
        Report(1, "%s\n", stx);
        free(stx);
    }
}
//---------------------------------------------------------------------------------------------------
/*
int mkLocData(char *line, s_loc_data *sloc)
{
//
//6:1703073858798:0.974654:0.655162:179.339397:0.247728:2.752015:7:1:0:1703073858700
//double dsav = (BIN_GPS.UTC + ADD_TO_2008) * 1000;
//uint64_t ts_sav = dsav;
//dl = sprintf(chap, "%d:%llu:%f:%f:%f:%f:%f:%u:%u:%d:%llu\n",
//            msgType7,
//            BIN_GPS.msec,
//            BIN_GPS.latitude, BIN_GPS.longitude, BIN_GPS.altitude, BIN_GPS.speed, BIN_GPS.dir,
//            BIN_GPS.satelite, gps_valid, BIN_GPS.flags.stop, ts_sav); 
//
int ret = -1, i, max_elem = 11, dl = 0;
uint32_t bit32;
//char vrem[512] = {0};
char tmp[32];
char *uke = NULL;
float flo;
double dbl;
uint8_t byte;
unsigned long long int tm;


    memset((uint8_t *)sloc, 0, sizeof(s_loc_data));

    //if (strlen(line) >= 512) return ret;

    //strcpy(vrem, line);
    char *uks = line;
    char *end = strchr(uks, '\n');
    if (!end) end = strchr(uks, '\0');
    for (i = 0; i < max_elem; i++) {
        uke = strchr(uks, ':');
        if (!uke) uke = end;
        dl = uke - uks;
        memcpy(tmp, uks, dl);
        switch (i) {
            case 0:    
                if (dl == 1) byte = tmp[0] - 0x30;
                        else byte = atol(tmp);
                if (byte != 6) return ret;
            break;
            case 1:
                tm = strtoull(tmp, NULL, 10);
                tm /= 1000;
                sloc->TM = (uint32_t)tm;
            break;
            case 2:
                dbl = strtod(tmp, NULL);
                dbl *= (180 / M_PI);
                flo = dbl;
                flo /= 90.0;
                flo *= 0xffffffff;
                sloc->LAT = flo;
            break;
            case 3:
                dbl = strtod(tmp, NULL);
                dbl *= (180 / M_PI);
                flo = dbl;
                flo /= 180.0;
                flo *= 0xffffffff;
                sloc->LONG = flo;
            break;
            case 4:
                sloc->ALT = (int)strtod(tmp, NULL);
            break;
            case 5:// м/сек -> км/час
                dbl = strtod(tmp, NULL);
                dbl /= 1000;
                dbl *= 3600;
                sloc->SPD = (uint16_t)dbl;
                sloc->SPD &= 0x3fff;// 14 младших разрядов
            break;
            case 6:
                dbl = strtod(tmp, NULL);
                dbl *= (180 / M_PI);
                sloc->DIR = (uint16_t)dbl;
            break;
            case 7:
                if (dl == 1) byte = tmp[0] - 0x30;
                else {
                    bit32 = atol(tmp);
                    byte = (uint8_t)bit32;
                }
                sloc->SAT = byte;
            break;
            case 8:
                if (dl == 1) byte = tmp[0] - 0x30;
                        else byte = (uint8_t)atol(tmp);
                sloc->VLD = byte & 1;
            break;
            case 9:
                if (sloc->VLD) sloc->MV = ~atoi(tmp);
                          else sloc->MV = 0;
            break;
            case 10:
                tm = strtoull(tmp, NULL, 10);
                tm /= 1000;
                sloc->NTM = (uint32_t)tm;
            break;
        }
        uks = uke + 1;
        if (uks >= end) break;
    }
    ret = i + 1;

    return ret;
}*/
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
void *egts_cli_loop(void *arg)
{
//char ipa[64] = {0};
uint16_t eport = egts_port_def;
sens_data_t sd = {NULL, 0};
//
char chap[max_line] = {0};
uint32_t tmr_egts, seq_number_egts = 0;//, car_busy_seq_number = 0;
uint8_t from_egts[LEN_2K] = {0};
uint8_t to_egts[LEN_2K] = {0};
int lenrecv_egts = 0, lenrecv_tmp_egts = 0, uk_egts = 0, esocket = -1, ready_egts = 0, to_egts_len = 0;
int ei, dl_egts = 0, rrc = 0;
uint8_t auth_egts = 0;
s_min_hdr *min_hdr = (s_min_hdr *)from_egts;
s_loc_data ldata;
uint8_t err_mkp = 0;
uint32_t wait_tmr_egts = 5;
s_list_rec list_auth;
s_list_rec list_data;
uint8_t egts_ack_done = 1;
uint32_t tmr_egts_ack = 0;
uint8_t snd_ack_egts = 0;
uint16_t eRN, ePID;
struct timeval cli_tv = {0, 10000};
fd_set read_Fds;
int8_t cmd = -1; // 0 - auth, 1 - data
//

        sleep(1);


        strncpy(ip_egts, (char *)arg, sizeof(ip_egts) - 1);
        char *uk = strchr(ip_egts, ':');
        if (uk) {
            eport = (uint16_t)atol(uk + 1);
            *uk = '\0';
        }
        if (dbg != logOff) Report(1, "[%s] Start egts_client thread for '%s:%u' (pid:%d)\n", __func__, ip_egts, eport, (int)getpid());

        usleep(5000);

        if (mkQuePrnEgts()) {
            Report(1, "[%s] Error mkQuePrn() (%s - %u)\n", __func__, strerror(errno), errno);
            goto outl; 
        }

        //
        memset((uint8_t *)&ldata, 0, sizeof(s_loc_data));

        memset((uint8_t *)&list_auth, 255, sizeof(s_list_rec));
        list_auth.SERVICE = EGTS_AUTH_SERVICE;
        list_auth.SR[0] = EGTS_SR_TERM_IDENTITY;//EGTS_SR_DISPATCHER_IDENTITY;//EGTS_SR_TERM_IDENTITY;

        memset((uint8_t *)&list_data, 255, sizeof(s_list_rec));
        list_data.SERVICE = EGTS_TELEDATA_SERVICE;
        list_data.SR[0] = EGTS_SR_POS_DATA;
        list_data.SR[1] = EGTS_SR_EXT_POS_DATA;

        tmr_egts = get_tmr_sec(2);//6//10//2
        //


        //------------------------------------------------------------------------

        while (!QuitAll) { 

            //
            if (esocket < 0) {
                egts_connect = false;
                if (gps_valid) {
                    if (check_tmr_sec(tmr_egts)) {
                        esocket = connectEGTS(ip_egts, eport);
                        tmr_egts = get_tmr_sec(wait_tmr_egts);//5
                    }
                }
            }
            if (esocket > 0) {
                if (!auth_egts && egts_ack_done) {
                    err_mkp = 0;
                    to_egts_len = mkPack(&list_auth, to_egts, ++seq_number_egts, &ldata, &err_mkp, dbg);
                    if (!err_mkp) {
                        if (send(esocket, to_egts, to_egts_len, SEND_FLAGS) != to_egts_len) {
                            if (dbg != logOff) Report(1, "Send to EGTS (%d) Error !!!\n", to_egts_len);
                            //
                            disconnectEGTS(&esocket, 1);
                            egts_ack_done = 1;   tmr_egts_ack = 0;
                            lenrecv_egts = lenrecv_tmp_egts = 0;
                            uk_egts = ready_egts = 0;
                            dl_egts = 0;
                            memset(from_egts, 0, sizeof(from_egts));
                            auth_egts = 0;
                            tmr_egts = get_tmr_sec(wait_tmr_egts);//5
                            //
                        } else {
                            cmd = 0;
                            if (dbg > logDebug) {
                                sprintf(chap, "Send to EGTS (%d) :", to_egts_len);
                                for (ei = 0; ei < to_egts_len; ei++) sprintf(chap+strlen(chap),"%02X", to_egts[ei]);
                                Report(1, "%s\n", chap);
                            }
                            egts_ack_done = 0;
                            tmr_egts_ack = get_tmr_sec(wait_tmr_egts_ack);
                        }
                    } else {
                        err_mkp = 0;
                        auth_egts = 0;
                        if (dbg != logOff) Report(1, "EGTS make packet Error (len=%d)!\n", to_egts_len);
                        tmr_egts = get_tmr_sec(wait_tmr_egts);//2
                    }
                }
            }//if (esocket > 0)
            //
            if (esocket > 0) {
                //cli_tv.tv_sec = 0; cli_tv.tv_usec = 10000;
                FD_ZERO(&read_Fds);
                FD_SET(esocket, &read_Fds);
                if (select(esocket + 1, &read_Fds, NULL, NULL, &cli_tv) > 0) {
                        if (FD_ISSET(esocket, &read_Fds)) {// event from device socket
                            lenrecv_tmp_egts = recv(esocket, &from_egts[uk_egts], 1, MSG_DONTWAIT);
                            if (!lenrecv_tmp_egts) {
                                //if (dbg != logOff) Report(1,"Disconnect from egts side (sock %d)\n", esocket);
                                disconnectEGTS(&esocket, 1);
                                egts_ack_done = 1;   tmr_egts_ack = 0;
                                lenrecv_egts = lenrecv_tmp_egts = 0;
                                uk_egts = ready_egts = 0;
                                dl_egts = 0;
                                memset(from_egts, 0, sizeof(from_egts));
                                auth_egts = 0;
                                tmr_egts = get_tmr_sec(wait_tmr_egts);
                            } else if (lenrecv_tmp_egts > 0) {
                                ready_egts = 0;
                                lenrecv_egts += lenrecv_tmp_egts;
                                uk_egts      += lenrecv_tmp_egts;
                                if ((lenrecv_egts == sizeof(s_min_hdr)) && (!dl_egts)) {
                                    dl_egts = min_hdr->FDL + 2;//DFL + CRC16_len
                                } else {
                                    if (dl_egts > 0) {
                                        if ( lenrecv_egts == dl_egts + sizeof(s_min_hdr) ) ready_egts = 1;
                                    }
                                }
                                if (!ready_egts) {
                                    if ( uk_egts >= sizeof(from_egts) - sizeof(s_min_hdr) ) ready_egts = 1;
                                }
                            }
                        }//if (FD_ISSET(esocket, &read_Fds)) // event from egts socket

                        if (ready_egts) {
                            if (dbg > logDebug) {
                                sprintf(chap, "Recv from EGTS (%d) :", lenrecv_egts);
                                for (ei = 0; ei < lenrecv_egts; ei++) sprintf(chap+strlen(chap),"%02X", from_egts[ei]);
                                Report(1, "%s\n",chap);
                            }
                            snd_ack_egts = 0;
                            eRN = ePID = 0;
                            if (parse_from_EGTS(from_egts, lenrecv_egts, &snd_ack_egts, &eRN, &ePID) != 0) {
                                if (dbg != logOff) Report(1,"[%s] Error parse egts message. Disconnect now (sock %d)\n", __func__, esocket);
                                disconnectEGTS(&esocket, 1);
                                tmr_egts_ack = 0;
                                auth_egts = 0;
                                tmr_egts = get_tmr_sec(wait_tmr_egts);//5
                            } else {
                                if (!cmd) {
                                    if (dbg != logOff) Report(1,"[%s] Access granted to egts server !!!\n", __func__);                                  
                                    auth_egts = 1;
                                    egts_connect = true;
                                }
                            }
                            tmr_egts_ack = 0;
                            egts_ack_done = 1;
                            uk_egts = ready_egts = 0;
                            lenrecv_egts = lenrecv_tmp_egts = 0;
                            dl_egts = 0;
                            memset(from_egts, 0, sizeof(from_egts));
                        }
                }
                if (auth_egts) {
                    err_mkp = 0;
                    if (egts_ack_done) {
                        if ((rrc = msgGetEgts(&sd)) > 0) {
                            if (dbg > logOn) Report(1, "[%s] msgGetEgts(%p, %d)=%d\n", __func__, sd.str, sd.len, rrc);
                            memcpy((uint8_t *)&ldata, (uint8_t *)sd.str, sd.len);
                            free(sd.str);

                            //
                            to_egts_len = mkPack(&list_data, to_egts, ++seq_number_egts, &ldata, &err_mkp, dbg);
                            cmd = 1;
                            //-----
                            if (!err_mkp && (to_egts_len > 0)) {
                                if (send(esocket, to_egts, to_egts_len, SEND_FLAGS) != to_egts_len) {
                                    if (dbg != logOff) Report(1, "[%s] Send to EGTS Error !!!\n", __func__);
                                    disconnectEGTS(&esocket, 1);
                                    egts_ack_done = 1;   tmr_egts_ack = 0;
                                    lenrecv_egts = lenrecv_tmp_egts = 0;
                                    uk_egts = ready_egts = 0;
                                    dl_egts = 0;
                                    memset(from_egts, 0, sizeof(from_egts));
                                    auth_egts = 0;
                                    tmr_egts = get_tmr_sec(wait_tmr_egts);
                                } else {
                                    if (dbg > logDebug) {
                                        sprintf(chap, "Send to EGTS (%d) :", to_egts_len);
                                        for (ei = 0; ei < to_egts_len; ei++) sprintf(chap+strlen(chap),"%02X", to_egts[ei]);
                                        Report(1, "[%s] %s\n", __func__, chap);
                                    }
                                    egts_ack_done = 0;
                                    tmr_egts_ack = get_tmr_sec(wait_tmr_egts_ack);
                                }
                            } else {
                                err_mkp = 0;
                                if (dbg != logOff) Report(1, "EGTS make packet Error (len=%d)!\n", to_egts_len);
                            }
                            //
                        }
                    }
                    //---  wait ack from EGTS ---
                    if (!egts_ack_done && tmr_egts_ack) {
                        if (check_tmr_sec(tmr_egts_ack)) {
                            egts_ack_done = 1;
                            tmr_egts_ack = 0;
                            if (dbg != logOff) Report(1, "EGTS no ack, Timeout %d sec. !!!\n", wait_tmr_egts_ack);
                            //
                            disconnectEGTS(&esocket, 1);
                            egts_ack_done = 1;   tmr_egts_ack = 0;
                            lenrecv_egts = lenrecv_tmp_egts = 0;
                            uk_egts = ready_egts = 0;
                            dl_egts = 0;
                            memset(from_egts, 0, sizeof(from_egts));
                            auth_egts = 0;
                            tmr_egts = get_tmr_sec(wait_tmr_egts);
                        }
                    }
                    //
                }
            }//if (esocket > 0)..
            //

            usleep(2000);

        }//While(....

        //------------------------------------------------------------------------

        delQuePrnEgts();

outl:

    if (esocket > 0) close(esocket);

    if (dbg != logOff) Report(1, "[%s] Done egts_client thread\n", __func__);

    if (total_threads) total_threads--;

    pthread_exit(NULL);

}
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------



