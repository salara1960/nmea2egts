#ifndef __EGTS_H__
#define __EGTS_H__

#include "func.h"





#define SET_ERR_PRN_LOGS

#define IMEI_DEF "358605100007990"// - ATV // "358605100008402" - ATB
#define TermID   77770299         // 77770166 - ATB


#define EPOCH_STR_LEN 24

#define UTS2010 (1262304000) // unix timestamp 00:00:00 01.01.2010
#define DispatcherID 2116
#define imei_size 15
#define maxList 4
#define maxServ 5
#define max_line 1024
#define max_url_len 127
#define wait_tmr_egts_ack 5
#define egts_port_def 4045


// Тип пакета Транспортного Уровня
#define EGTS_PT_RESPONSE       0
#define EGTS_PT_APPDATA        1
#define EGTS_PT_SIGNED_APPDATA 2

// Сервисы | EGTS\EGTS 1.6\RUS\protocol_EGTS_services_v.1.6_p1_RUS.pdf | 6.1 СПИСОК СЕРВИСОВ
#define EGTS_AUTH_SERVICE           1
#define EGTS_TELEDATA_SERVICE       2
#define EGTS_COMMANDS_SERVICE       4
#define EGTS_FIRMWARE_SERVICE       9
#define EGTS_ECALL_SERVICE         10
#define EGTS_INSURANCE_SERVICE     20
#define EGTS_EUROPROTOCOL_SERVICE  22

// RST = EGTS_AUTH_SERVICE subrecords:
#define EGTS_SR_RECORD_RESPONSE     0
#define EGTS_SR_TERM_IDENTITY       1
#define EGTS_SR_MODULE_DATA         2
#define EGTS_SR_VEHICLE_DATA        3
#define EGTS_SR_DISPATCHER_IDENTITY 5
#define EGTS_SR_AUTH_PARAMS         6
#define EGTS_SR_AUTH_INFO           7
#define EGTS_SR_SERVICE_INFO        8
#define EGTS_SR_RESULT_CODE         9
#define EGTS_SR_TERM_IDENTITY2     10

// RST = EGTS_TELEDATA_SERVICE subrecords:
//#define EGTS_SR_RECORD_RESPONSE 0
#define EGTS_SR_POS_DATA            16
#define EGTS_SR_EXT_POS_DATA        17
#define EGTS_SR_AD_SENSORS_DATA     18
#define EGTS_SR_COUNTERS_DATA       19
#define EGTS_SR_ACCEL_DATA          20
#define EGTS_SR_STATE_DATA          21 // http://forum.gurtam.com/viewtopic.php?pid=48848#p48848
#define EGTS_SR_LOOPIN_DATA         22
#define EGTS_SR_ABS_DIG_SENS_DATA   23
#define EGTS_SR_ABS_AN_SENS_DATA    24
#define EGTS_SR_ABS_CNTR_DATA       25
#define EGTS_SR_ABS_LOOPIN_DATA     26
#define EGTS_SR_LIQUID_LEVEL_SENSOR 27
#define EGTS_SR_PASSENGERS_COUNTERS 28

// SST (Service Statement)
#define EGTS_SST_IN_SERVICE       0 //сервис в рабочем состоянии и разрешен к использованию
#define EGTS_SST_OUT_OF_SERVICE 128 //сервис в нерабочем состоянии (выключен)
#define EGTS_SST_DENIED         129 //сервис запрещен для использования
#define EGTS_SST_NO_CONF        130 //сервис не настроен
#define EGTS_SST_TEMP_UNAVAIL   131 //сервис временно недоступен

// NS type
#define NS_GNSS    1 // ГЛОНАСС;
#define NS_GPS     2 // GPS;
#define NS_GALILEO 4 // Galileo;
#define NS_COMPASS 8 // Compass;
#define NS_BEIDOU 16 // Beidou;
#define NS_DORIS  32 // DORIS;
#define NS_IRNSS  64 // IRNSS;
#define NS_QZSS  128 // QZSS.

//***********************************************************************************
//***********************************************************************************
//***********************************************************************************

typedef struct
{
    char *stx;
    size_t size;
} s_mem_str;


#pragma pack(push,1)
typedef struct {
    unsigned ENA :2;
    unsigned PKE :1;
    unsigned ISLE:1;
    unsigned MSE :1;
    unsigned SSE :1;
    unsigned EXE :1;
    unsigned unused:1;
//    uint16_t PKL; (Public Key Length) - длина публичного ключа в байтах
//    uint8_t РВК[0..512]; (Public Key) - данные публичного ключа
//    uint16_t ISL; (Identity String Length) - результирующая длина идентификационных данных
//    uint16_t MSZ; (ModSize) - параметр, применяемый в процессе шифрования
//    char SS[0..255]; (Server Sequence) - специальная серверная последовательность байтов, применяемая в процессе шифрования
//    uint8_t D; (Delimiter) - разделитель строковых параметров (всегда имеет значение 0)
//    char EXP[0..255]; (Ехр) - специальная последовательность, используемая в процессе шифрования
//    uint8_t D; (Delimiter) - разделитель строковых параметров (всегда имеет значение 0)
} s_auth_params;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct {
    uint8_t SERVICE;
    uint8_t SR[maxList];
} s_list_rec;
#pragma pack(pop)



#pragma pack(push,1)
typedef struct {
    uint8_t  PRV;    //0x01
    uint8_t  SKID;   //0x00
    unsigned PR :2; //Priority: 00-высший, 01-высокий, 10-средний, 11-низкий
    unsigned CMP:1; //Compressed
    unsigned ENA:2; //Encryption Algorithm
    unsigned RTE:1; //1-PeerAddress+RecipientAddress+TTL present
    unsigned PRF:2; //Prefix=0x00
    uint8_t HL;     //Header Length
    uint8_t HE;     //Header Encoding
    uint16_t FDL;   //Frame Data Length
    uint16_t PID;   //Packet Identifier
    uint8_t PT;     //Packet Type
    uint8_t HCS;    //Header Check Sum
} s_min_hdr;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint16_t RL; //Record length
    uint16_t RN; //Record Number
    unsigned OBFE:1; //Object ID Field Exists//0x58 = 01011000
    unsigned EVFE:1; //Event ID Field Exists
    unsigned TMFE:1; //Time Field Exists
    unsigned RPP:2; //Record Processing Priority
    unsigned GRP:1; //Group
    unsigned RSOD:1; //Recipient Service On Device
    unsigned SSOD:1; //Source Service On Device
//    uint32_t OID;//Object Identifier
//    uint32_t EVID;//Event Identifier
    uint32_t TM;//Time in sec. from 00:00:00 01.01.2010 UTC
    uint8_t SST;//Source Service Type
    uint8_t RST;//Recipient Service Type
    //uint8_t RD[3..65498];//Record Data
} s_rec_hdr;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint16_t RL; //Record length
    uint16_t RN; //Record Number
    unsigned OBFE:1; //Object ID Field Exists//0x58 = 01011000
    unsigned EVFE:1; //Event ID Field Exists
    unsigned TMFE:1; //Time Field Exists
    unsigned RPP:2; //Record Processing Priority
    unsigned GRP:1; //Group
    unsigned RSOD:1; //Recipient Service On Device
    unsigned SSOD:1; //Source Service On Device
//    uint32_t OID;//Object Identifier
//    uint32_t EVID;//Event Identifier
//    uint32_t TM;//Time in sec. from 00:00:00 01.01.2010 UTC
//    uint8_t SST;//Source Service Type
//    uint8_t RST;//Recipient Service Type
    //uint8_t RD[3..65498];//Record Data
} s_mrec_hdr;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t srTYPE;
    uint16_t srLEN;
    uint8_t srRC;
} s_ack_RCODE;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint16_t RPID;//PID from incomming packet (Response Packet ID)
    uint8_t RESULT;//EGTS_PC_OK - для транспортного уровня
} s_ack_SFRD;
#pragma pack(pop)



#pragma pack(push,1)
typedef struct {
    uint8_t SID;//EGTS_SR_TERM_IDENTITY
    uint16_t Len;//20
    uint32_t TID;
    unsigned HDIDE:1;
    unsigned IMEIE:1;
    unsigned IMSIE:1;
    unsigned LNGCE:1;
    unsigned SSRA:1;
    unsigned NIDE:1;
    unsigned BSE:1;
    unsigned MNE:1;
//    uint16_t HDID;//Home Dispatcher Identifier
//    char IMEI[15];
//    char IMSI[16];
//    char LNGC[3];
//    uint8_t NID[3];
//    uint16_t BS;//Max recv_buffer size
//    char MSISDN[15];//Phone number
} s_term_id;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t SID;//EGTS_SR_VEHICLE_DATA
    uint16_t Len;//19

    char VIN[17];//Vehicle Identification Number
    uint32_t VHT;//Vehicle Type
    uint32_t VPST;//Vehicle Propulsion Storage Type
} s_sr_veh_data;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t SID;//EGTS_SR_DISPATCHER_IDENTITY 5
    uint16_t Len;//5

    uint8_t DT;//Dispatcher Type
    uint32_t DID;//Dispatcher ID = 2116
//    char DSCR[255];//Description
} s_sr_disp_id;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t SID;//EGTS_SR_SERVICE_INFO        8
    uint16_t Len;//5

    uint8_t ST;//Service Type
    uint8_t SST;//Service Statement : EGTS_SST_IN_SERVICE or EGTS_SST_DENIED
    unsigned SRVRP:2;//Service Routing Priority : 00 - up priority, 11 - down priority
    unsigned unused:5;
    unsigned SRVA:1;//Service Attribute : 0 - supported, 1 - requested
} s_sr_srv_info;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t ST;//Service Type
    uint8_t SST;//Service Statement : EGTS_SST_IN_SERVICE, EGTS_SST_DENIED, ...
    // SRVP Service Parameters
    unsigned SRVRP:2;//Service Routing Priority : 00 - high priority, 11 - low priority
    unsigned none:5;//not used
    unsigned SRVA:1;//Service Attribute : 0 - supported, 1 - requested
} s_sr_info;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct {
    uint8_t PRV;//0x01
    uint8_t SKID;//0x00
    unsigned PR:2;//Priority: 00-высший, 01-высокий, 10-средний, 11-низкий
    unsigned CMP:1;//Compressed
    unsigned ENA:2;//Encryption Algorithm
    unsigned RTE:1;//1-PeerAddress+RecipientAddress+TTL present
    unsigned PRF:2;//Prefix=0x00
    uint8_t HL;//Header Length
    uint8_t HE;//Header Encoding
    uint16_t FDL;//Frame Data Length
    uint16_t PID;//Packet Identifier
    uint8_t PT;//Packet Type
    uint8_t HCS;//Header Check Sum
    uint16_t ack_RPID;//PID from incomming packet
    uint8_t ack_RESULT;//EGTS_PC_OK - для транспортного уровня
    uint16_t ack_CRN;//RN from incomming record
    uint8_t ack_RST;//status for incomming record
    uint8_t ack_RCD;//EGTS_PC_OK - auth. result
    uint16_t DCS;//Data Check Sum
} s_min_ack;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
//    uint8_t PRV;//0x01
//    uint8_t SKID;//0x00
//    unsigned PR:2;//Priority: 00-высший, 01-высокий, 10-средний, 11-низкий
//    unsigned CMP:1;//Compressed
//    unsigned ENA:2;//Encryption Algorithm
//    unsigned RTE:1;//1-PeerAddress+RecipientAddress+TTL present
//    unsigned PRF:2;//Prefix=0x00
//    uint8_t HL;//Header Length
//    uint8_t HE;//Header Encoding
//    uint16_t FDL;//Frame Data Length
//    uint16_t PID;//Packet Identifier
//    uint8_t PT;//Packet Type
//    uint8_t HCS;//Header Check Sum
    uint16_t ack_RPID;//PID from incomming packet
    uint8_t ack_RESULT;//EGTS_PC_OK - для транспортного уровня
//    uint16_t ack_CRN;//RN from incomming record
//    uint8_t ack_RST;//status for incomming record
//    uint8_t ack_RCD;//EGTS_PC_OK - auth. result
//    uint16_t DCS;//Data Check Sum
} s_ack;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct {
    uint8_t SID;//EGTS_SR_POS_DATA //16
    uint16_t Len;//20

    uint32_t NTM;//Navigation Time
    uint32_t LAT;
    uint32_t LONG;

    unsigned VLD:1;//cor valid
    unsigned FIX:1;//0-2D fix, 1-3D fix
    unsigned CS:1;//system coordinate : 0-WGS-84, 1-ПЗ-90.02
    unsigned BB:1;//0-current data, 1-data from black box
    unsigned MV:1;//1-moving, 0-stop
    unsigned LAHS:1;//0-north latitude, 1-south latitude
    unsigned LOHS:1;//0-ost longitude, 1-west longitude
    unsigned ALTE:1;//Present ALT

    uint16_t SPD;//speed (without 2 high bit, bit7-DIRH  bit6-ALTS)
    uint8_t DIR;//Direction
    uint8_t ODM[3];//Odometer
    uint8_t DIN;//Digital Input
    uint8_t SRC;//Source
    uint8_t ALT[3];//Altitude
//    uint16_t SRCD;//Source Data
} s_sr_pos_data;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    uint8_t SID;//EGTS_SR_EXT_POS_DATA - 17
    uint16_t Len;//2

    unsigned VFE:1;// (VDOP Field Exists) — определяет наличие поля VDOP
    unsigned HFE:1;// (HDOP Field Exists) — определяет наличие поля HDOP
    unsigned PFE:1;// (PDOP Field Exists) — определяет наличие поля PDOP
    unsigned SFE:1;// (Satellites Field Exists) — определяет наличие данных о текущем количестве видимых спутников SAT и
                   // типе используемой навигационной спутниковой системы NS:
    unsigned NSFE:1;// (Navigation System Field Exists) — определяет наличие данных о типах используемых навигационных спутниковых систем:
    unsigned none:3;

    uint8_t SAT;// (Satellites)
    uint16_t NS;// (Navigation System)
} s_sr_ext_pos_data;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct {
    uint8_t SID;//EGTS_SR_AD_SENSORS_DATA - 18
    uint16_t Len;//3 + 1 + 8

    uint8_t DIOE;
    uint8_t DOUT;
    uint8_t ASFE;
//    unit8_t ADIO1;// (Additional Digital Inputs Octet 1)
//    unit8_t ADIO2;// (Additional Digital Inputs Octet 2)
//    unit8_t ADIO3;// (Additional Digital Inputs Octet 3)
//    unit8_t ADIO4;// (Additional Digital Inputs Octet 4)
//    unit8_t ADIO5;// (Additional Digital Inputs Octet 5)
//    unit8_t ADIO6;// (Additional Digital Inputs Octet 6)
//    unit8_t ADIO7;// (Additional Digital Inputs Octet 7)
//    unit8_t ADIO8;// (Additional Digital Inputs Octet 8)
    uint8_t ANS[8][3];// (Analog Sensor 1..8)
} s_sr_ad_sensors_data;//EGTS_SR_AD_SENSORS_DATA
#pragma pack(pop)

/*
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
    unsigned VLD:1;//cor valid
    unsigned MV:1;//1-moving, 0-stop
    unsigned LAHS:1;//0-north latitude, 1-south latitude
    unsigned LOHS:1;//0-ost longitude, 1-west longitude
    unsigned unused:4;
} s_loc_data;
#pragma pack(pop)
*/

enum {
    EGTS_PC_OK = 0,// успешно обработано
    EGTS_PC_IN_PROGRESS,// в процессе обработки (результат обработки ещё не известен)
    EGTS_PC_UNS_PROTOCOL = 128,//неподдерживаемый протокол
    EGTS_PC_DECRYPT_ERROR,// ошибка декодирования
    EGTS_PC_PROC_DENIED,// обработка запрещена
    EGTS_PC_INC_HEADERFORM,// неверный формат заголовка
    EGTS_PC_INC_DATAFORM,//  неверный формат данных
    EGTS_PC_UNS_TYPE,// неподдерживаемый тип
    EGTS_PC_NOTEN_PARAMS,//  неверное количество параметров
    EGTS_PC_DBL_PROC,// попытка повторной обработки
    EGTS_PC_PROC_SRC_DENIED,// обработка данных от источника запрещена
    EGTS_PC_HEADERCRC_ERROR,// ошибка контрольной суммы заголовка
    EGTS_PC_DATACRC_ERROR,// ошибка контрольной суммы данных
    EGTS_PC_INVDATALEN,//139 некорректная длина данных
    EGTS_PC_ROUTE_NFOUND,// маршрут не найден
    EGTS_PC_ROUTE_CLOSED,// маршрут закрыт
    EGTS_PC_ROUTE_DENIED,//  маршрутизация запрещена
    EGTS_PC_INVADDR,// неверный адрес
    EGTS_PC_TTLEXPIRED,//  превышено количество ретрансляции данных
    EGTS_PC_NO_ACK,// нет подтверждения
    EGTS_PC_OBJ_NFOUND,// объект не найден
    EGTS_PC_EVNT_NFOUND,// событие не найдено
    EGTS_PC_SRVC_NFOUND,// сервис не найден
    EGTS_PC_SRVC_DENIED,// сервис запрещён
    EGTS_PC_SRVC_UNKN,// неизвестный тип сервиса
    EGTS_PC_AUTH_DENIED,// авторизация запрещена
    EGTS_PC_ALREADY_EXISTS,//  объект уже существует
    EGTS_PC_ID_NFOUND,// идентификатор не найден
    EGTS_PC_INC_DATETIME,// неправильная дата и время
    EGTS_PC_IO_ERROR,// ошибка ввода/вывода
    EGTS_PC_NO_RES_AVAIL,// недостаточно ресурсов
    EGTS_PC_MODULE_FAULT,// внутренний сбой модуля
    EGTS_PC_MODULE_PWR_FLT,// сбой в работе цепи питания модуля
    EGTS_PC_MODULE_PROC_FLT,// сбой в работе микроконтроллера модуля
    EGTS_PC_MODULE_SW_FLT,// сбой в работе программы модуля
    EGTS_PC_MODULE_FW_FLT,// сбой в работе внутреннего ПО модуля
    EGTS_PC_MODULE_IO_FLT,// сбой в работе блока ввода/вывода модуля
    EGTS_PC_MODULE_MEM_FLT,// сбой в работе внутренней памяти модуля
    EGTS_PC_TEST_FAILED// тест не пройден
};



//***********************************************************************************

char dev[imei_size + 1];
s_loc_data loc_data;
volatile bool egts_connect;

uint32_t termID;

extern char uNameFile[64];
extern const uint8_t maxPackName;
extern uint32_t tmr_egts_msg;
extern int fd_elog;
//extern bool gps_valid;


//***********************************************************************************

int msgGetEgts(sens_data_t *msg);
int msgPutEgts(void *str, int len);

//***********************************************************************************

extern void Report(const uint8_t addTime, const char *fmt, ...);
extern uint32_t get_tmr_sec(uint32_t tm);
extern int check_tmr_sec(uint32_t tm);

void *egts_cli_loop(void *arg);
//***********************************************************************************


#endif /* __EGTS_H__ */
