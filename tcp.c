#include "tcp.h"


bool tcp_connect = false;
javad_gps_t GPS;
static char item[MAX_LIST_ITEM][MAX_ITEM_LEN];
const char *nmea[] = {// cимвольные маркеры NMEA сообщений, которые будут анализироваться
        "GGA",
        "RMC"
};
//
int fd = -1;
uint32_t SPEED = DEF_SPEED;
char from_dev[1024];
struct termios oldtio;
struct termios newtio;
val_speed_t all_speed[] = {
        {9600,    B9600},
        {19200,   B19200},
        {38400,   B38400},
        {57600,   B57600},
        {115200,  B115200},
        {230400,  B230400},
        {460800,  B460800},
        {500000,  B500000},
        {576000,  B576000},
        {921600,  B921600},
        {1000000, B1000000},
        {1152000, B1152000},
        {1500000, B1500000}
};
//-----------------------------------------------------------------------------
int speedShow(uint32_t spd)
{
    for(int8_t i = 0; i < TOTAL_SPEED; i++) {
        if (all_speed[i].out_spd == spd) return all_speed[i].in_spd;
    }

    return 0;
}
//-----------------------------------------------------------------------
int init_uart(char *dev_name)
{

    int fd = open(dev_name, O_RDWR | O_NONBLOCK, 0664);
    if (fd > 0) {
        tcgetattr(fd, &oldtio);
        memset(&newtio, 0, sizeof(newtio));
        memcpy(&newtio, &oldtio, sizeof(oldtio));

        cfmakeraw(&newtio);//set RAW mode

        newtio.c_cflag = SPEED | CS8 | CLOCAL | CREAD;

        tcflush(fd, TCIFLUSH);

        tcsetattr(fd, TCSANOW, &newtio);

        memset(from_dev, 0, sizeof(from_dev));
    }

    return fd;
}
//-----------------------------------------------------------------------
int deinit_uart(int fd)
{
    if (fd < 0) return fd;

    tcsetattr(fd, TCSANOW, &oldtio);

    return close(fd);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//      Преобразует два символа строки из hex-формата в двоичный
//
uint8_t hexToBin(char *sc)
{
char st = 0, ml = 0;

    if ((sc[0] >= '0') && (sc[0] <= '9')) st = (sc[0] - 0x30);
    else
    if ((sc[0] >= 'A') && (sc[0] <= 'F')) st = (sc[0] - 0x37);
    else
    if ((sc[0] >= 'a') && (sc[0] <= 'f')) st = (sc[0] - 0x57);

    if ((sc[1] >= '0') && (sc[1] <= '9')) ml = (sc[1] - 0x30);
    else
    if ((sc[1] >= 'A') && (sc[1] <= 'F')) ml = (sc[1] - 0x37);
    else
    if ((sc[1] >= 'a') && (sc[1] <= 'f')) ml = (sc[1] - 0x57);

    return ((st << 4) | (ml & 0x0f));

}
//-----------------------------------------------------------------------------
//  Функция проверяет на валидность строку на соответствие NMEA формату
//
int gpsValidate(const char *str)
{
//char check[3] = {0};
//char calcCRCstr[3] = {0};
//int i = 0;
//int calcCRC = 0;
//char *uk = NULL;


    if (!strstr(str, "$G")) return 0;

    int8_t j = -1;
    for (int8_t k = 0; k < MAX_NMEA_MSG; k++) {
        if (strstr(str, nmea[k])) {
            j = k;
            break;
        }
    }
    //if (dbg >= logDebug) Report(0, "\tidx=%d - %s", j, str);
    if (j == -1) return 0; else return 1;
    
/*
    while ((str[i] != 0) && (str[i] != '*') && (i < 75)) calcCRC ^= str[i++];


    if (dbg >= logDebug) Report(0, "\ti=%d - %s", i, str);
    if (i >= 75) return 0;

    if ((uk = strchr(str, '*')))  {
        memcpy(check, uk + 1, 2);
        //check[0] = *(uk + 1);
        //check[1] = *(uk + 2);
        sprintf(calcCRCstr,"%02X", calcCRC);

        return ((calcCRCstr[0] == check[0]) && (calcCRCstr[1] == check[1])) ? 1 : 0 ;
    } else return 0;
*/
}
//-----------------------------------------------------------------------------
//            Пересчет данных геолокации в градусы
//
double gpsToDec(double deg, char nsew)
{
    int degree = (int)(deg / 100);
    double minutes = deg - degree * 100;
    double dec_deg = minutes / 60;
    double decimal = degree + dec_deg;
    if (nsew == 'S' || nsew == 'W') decimal *= -1;

    return decimal;
}
//-----------------------------------------------------------------------------
void prnMsgList(int it)
{
char *tmp = (char *)calloc(1, 32 + (MAX_ITEM_LEN + 8) * MAX_LIST_ITEM);//1024);

    if (!tmp) return;

    sprintf(tmp, "items=%d  ", it);

    for (int i = 0; i < MAX_LIST_ITEM; i++)
        if (strlen(item[i])) sprintf(tmp+strlen(tmp), ", %02d:'%s'", i, item[i]);
    Report(0, "\t%s\n", tmp);

    free(tmp);
}
//-----------------------------------------------------------------------------
int splitMsg(const char *str)
{ 
int ret = -1, dl = 0;

    char *us = strchr(str, ',');
    if (!us) return ret;
    us++;

    char *ue = us;
    char *end = strchr(str, '*');
    if (!end) return ret;

    memset((uint8_t *)&item[0], 0, MAX_ITEM_LEN * MAX_LIST_ITEM);

    bool loop = true;

    while (loop) {
        ue = strchr(us, ',');
        if (!ue) { ue = end; loop = false; }
        if (ue) {
            ret++;
            dl = ue - us;
            if (dl > 0) {
                if (dl > MAX_ITEM_LEN) dl = MAX_ITEM_LEN - 1; 
                memcpy(&item[ret][0], us, dl);
            }
            us = ue + 1;
        } else loop = false;
    } 

    ret++;

    if (dbg > logOn) prnMsgList(ret);

    return ret;
}
//-----------------------------------------------------------------------------
//   Парсер валидных NMEA сообщений и заполнение структуры данными геолокации
//
int gpsParse(const char *str)
{
int ret = -1, idx = -1;
char tmp[16] = {0};


    for (int8_t i = 0; i < MAX_NMEA_MSG; i++) {
        if (strstr(str, nmea[i])) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return ret;

    //  Подсчет контрольной суммы NMEA сообщения
    char sc[2] = {0};
    uint8_t crc_in = 255, crc_calc = 0;
    char *uk = strchr(str, '*');
    if (uk) {
        memcpy(sc, uk + 1, 2);
        crc_in = hexToBin(sc);
        char *us = strchr(str, '$');
        if (us) {
            us++;
            if (uk > us) {
                while(us < uk) crc_calc ^= *us++;
            }
        }
    }
    //  Проверка контрольной суммы
    if (crc_in != crc_calc) {
        Report(1, "[%s] idx=%d CRC Error: 0x%X-0x%X %s\n", __func__, idx, crc_in, crc_calc, str);
        return ret;
    }

    int res = splitMsg(str);
    if (res <= 0) {
        //devError |= devGPS;
        return ret;
    } else {    


        switch (idx) {
            case ixGGA://$GPGGA,081549.00,5550.6198987,N,03732.2783241,E,1,20,1.03,173.2604,M,15.0999,M,,*5C
                       //$GPGGA,115349.00,5550.602007,N,03732.263725,E,1,16,2.3,00146.429,M,0014.442,M,,*6E
                if (strlen(item[0])) GPS.utc_time       = atof(item[0]);
                if (strlen(item[1])) GPS.nmea_latitude  = atof(item[1]);
                if (strlen(item[2])) GPS.ns             = item[2][0];
                if (strlen(item[3])) GPS.nmea_longitude = atof(item[3]);
                if (strlen(item[4])) GPS.ew             = item[4][0];
                if (strlen(item[5])) GPS.quality        = atoi(item[5]);
                if (strlen(item[6])) GPS.satelite       = atoi(item[6]);
                if (strlen(item[7])) GPS.HDOP           = atof(item[7]);
                if (strlen(item[8])) GPS.altitude       = atof(item[8]);
                if (strlen(item[9])) GPS.altitude_units = item[9][0];
                if (strlen(item[10])) GPS.geoid         = atof(item[10]);
                GPS.dec_latitude  = gpsToDec(GPS.nmea_latitude,  GPS.ns);
                GPS.dec_longitude = gpsToDec(GPS.nmea_longitude, GPS.ew);
            break;
            case ixRMC://$GNRMC,081549.00,A,5550.6198987,N,03732.2783241,E,0.0232,003.400,090224,12.054,W,A*3A
                       //$GPRMC,115348.90,A,5550.602007,N,03732.263725,E,000.00000,179.3,190624,,,A*5D
                //BIN_GPS.msec = get_msec();
                if (strlen(item[0])) GPS.utc_time       = atof(item[0]);
                if (strlen(item[1])) GPS.rmc_status     = item[1][0];
                if (GPS.rmc_status == 'A') {
                    gps_valid = true;
                    GPS.valid = 0;
                } else {
                    gps_valid = false;
                    GPS.valid = 1; 
                }
                if (strlen(item[2])) GPS.nmea_latitude  = atof(item[2]);
                if (strlen(item[3])) GPS.ns             = item[3][0];
                if (strlen(item[4])) GPS.nmea_longitude = atof(item[4]);
                if (strlen(item[5])) GPS.ew             = item[5][0];
                if (strlen(item[6])) GPS.speed          = atof(item[6]);
                if (strlen(item[7])) GPS.dir            = atof(item[7]);
                if (strlen(item[8]) == 6) {
                    memcpy(tmp, &item[8][0], 2); GPS.dt.tm_mday = atol(tmp);
                    memcpy(tmp, &item[8][2], 2); GPS.dt.tm_mon  = atol(tmp) - 1;
                    memcpy(tmp, &item[8][4], 2); GPS.dt.tm_year = atol(tmp) + 100;// + 2000 - 1900
                }
                if (strlen(item[0]) >= 8) {
                    memcpy(tmp, &item[0][0], 2); GPS.dt.tm_hour = atol(tmp); 
                    memcpy(tmp, &item[0][2], 2); GPS.dt.tm_min  = atol(tmp);
                    memcpy(tmp, &item[0][4], 2); GPS.dt.tm_sec  = atol(tmp);
                    uk = strchr(item[0], '.');
                    if (uk) GPS.mlsec = atol(uk + 1);

                    GPS.epoch = mktime(&GPS.dt) + TIME_ZONE;
                }
                GPS.dec_latitude  = gpsToDec(GPS.nmea_latitude,  GPS.ns);
                GPS.dec_longitude = gpsToDec(GPS.nmea_longitude, GPS.ew);
            break;
        }

        ret = idx;

    }

    return ret;
}
//----------------------------------------------------------------------------------
void mk_dataEGTS()
{
        memset((uint8_t *)&loc_data, 0 , sizeof(s_loc_data));

        loc_data.TM = loc_data.NTM = GPS.epoch;
        //
        double dbl = GPS.dec_latitude;
        //if (!degree_flag) dbl *= (180 / M_PI);//from radian to degree
        float flo = dbl;
        flo /= 90.0;
        flo *= 0xffffffff;
        loc_data.LAT = flo; 
        //
        dbl = GPS.dec_longitude;
        //if (!degree_flag) dbl *= (180 / M_PI);//from radian to degree
        flo = dbl;
        flo /= 180.0;
        flo *= 0xffffffff;
        loc_data.LONG = flo;
        //
        dbl = GPS.speed;
        dbl /= 1000;
        dbl *= 3600;
        loc_data.SPD = (uint16_t)dbl;
        loc_data.SPD &= 0x3fff;// 14 младших разрядов
        //
        dbl = GPS.dec_dir;
        //if (!degree_flag) dbl *= (180 / M_PI);//from radian to degree
        loc_data.DIR = (uint16_t)dbl;
        //
        dbl = GPS.altitude;
        loc_data.ALT = (int)dbl;
        //
        if (gps_valid) {
            loc_data.SAT = (uint8_t)GPS.satelite;
            loc_data.VLD = 1;
        }
        //-----------------------------------------------------
        if (loc_data.SPD) loc_data.MV = 1;
                     else loc_data.MV = 0;
        //-----------------------------------------------------
        //
        if (loc_data.LAT < 0) loc_data.LAHS = 1;//south
                       //else loc_data.LAHS = 0;//north
        //
        if (loc_data.LONG < 0) loc_data.LOHS = 1;//west
                        //else loc_data.LOHS = 0;//ost
}
//--------------------------------------------------------------------------------------------
void *loopTCP(void *arg)
{
int soc = -1;
char buf[1024];
int lenr = 0, lenr_tmp, ukb = 0, rdy = 0, read_len = 1;//, out = 0;
fd_set Fds;
struct timeval tvs = {0, 2000};
bool rx_next = false;
bool start_msg = false;
char *uk = NULL;
int res = 0;


    if (arg) soc = *(int *)arg;
    if (soc == -1) goto done;

    Report(1, "[%s] Start tcp_client thread for socket %d (pid:%d)\n", __func__, soc, (int)getpid());

    while (!QuitAll) {

        FD_ZERO(&Fds);
        FD_SET(soc, &Fds);

        if (select(soc + 1, &Fds, NULL, NULL, &tvs) > 0) {
            //
            if (FD_ISSET(soc, &Fds)) {
rx_loop:
                lenr_tmp = recv(soc, (uint8_t *)&buf[ukb], read_len, MSG_DONTWAIT);
                if (lenr >= (sizeof(buf) - 1)) {
                    rx_next = true;
                    Report(1, "[%s] Error overload input buffer (lenr=%d)\n", __func__, lenr);
                    lenr = ukb = 0;
                    memset(buf, 0, sizeof(buf));
                } else if (lenr_tmp > 0) {
                    rx_next = true;
                    lenr += lenr_tmp;
                    if (lenr == 1) {
                        //wait_msg = get_tmr_sec(wait_msg_def);
                        //en_pwr_on = false;
                        if (buf[0] == '$') start_msg = true;
                    }
                    if (start_msg) {
                        ukb  += lenr_tmp;
                        uk = strchr(buf, '\n');
                        if (uk && (lenr > 2)) {
                            rdy = 1;
                            //*uk = '\0';
                        }
                    } else {
                        start_msg = false;
                        lenr = ukb = 0;
                        memset(buf, 0, sizeof(buf));
                    }
                } else if (!lenr_tmp) {
                    Report(1, "[%s] Client closed connection\n", __func__);
                    break;
                } else if (lenr_tmp < 0) {
                    rx_next = false;
                }
                if (!rdy) {
                    if (rx_next) goto rx_loop;
                }
            }//if (FD_ISSET(fd, &Fds))
            //
        } else usleep(50);//if (select
        //
        if (rdy) {
            rdy = 0;
            if (dbg != logOff) Report(1, "[%s] %s", __func__, buf);
            //
            if ((res = gpsValidate(buf)) > 0) {
                //if (dbg > logOn) Report(1, "[%s] gpsValidate(buf)=%d\n", __func__, res);
                res = gpsParse(buf);
                if (dbg > logOn) Report(1, "[%s] gpsParse(buf)=%d gps_valid=%d\n", __func__, res, gps_valid);
                if ((res == ixRMC) && gps_valid) {
                    mk_dataEGTS();
                    res = msgPutEgts((void *)&loc_data, sizeof(s_loc_data));
                    if (dbg > logOn) Report(1, "[%s] msgPutEgts(%p, %lu)=%d\n", __func__, (void *)&loc_data, sizeof(s_loc_data), res);
                }
            } else {
		        if (dbg > logOn) Report(1, "[%s] Error gpsValidate(buf)=%d\n", __func__, res);
		        usleep(1000);
            }
	    //
            memset(buf, 0, sizeof(buf));
            lenr = lenr_tmp = 0;
            ukb = 0;  
            start_msg = false; 
        }
        //
    }

    close(soc);
    tcp_connect = false;

done:

    Report(1, "[%s] Done tcp_client thread\n", __func__);

    if (total_threads) total_threads--;

    pthread_exit(NULL);

}
//---------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
void loopDev(void *arg)
{
int desc = -1, lenr = 0, lenr_tmp = 0, ukb = 0, res;
struct timeval mytv = {0, 10000};
fd_set Fdr;
int8_t rdy = 0;



    if (arg) desc = *(int *)arg;
    if (desc == -1) goto done;

    Report(1, "[%s] Start read_serial function for device '%s' (fd=%d)\n", __func__, dev_name, fd);


    while (!QuitAll) {

        FD_ZERO(&Fdr);
        FD_SET(fd, &Fdr);
        
        if (select(desc + 1, &Fdr, NULL, NULL, &mytv) > 0) {
            if (FD_ISSET(desc, &Fdr)) {// event from my device

                lenr_tmp = read(desc, &from_dev[ukb], 1);

                if (!lenr_tmp) {
                    Report(1, "[%s] Can't read from device '%s'\n", __func__, dev_name);
                    break;
                } else if (lenr_tmp > 0) {
                    lenr += lenr_tmp;
                    ukb += lenr_tmp;
                    if (lenr >= sizeof(from_dev) - 1) {
                        lenr = lenr_tmp = 0;
                        ukb = 0;
                        memset(from_dev, 0, sizeof(from_dev));
                    } else {
                        if (lenr > 2) {
                            if (strchr(from_dev, '\n')) rdy = 1;
                        }
                    }
                }
                if (rdy) {
                    rdy = 0;
                    //
                    if (dbg > logOff) Report(1, "[%s] %s", __func__, from_dev);
                    if ((res = gpsValidate(from_dev)) > 0) {
                        res = gpsParse(from_dev);
                        if (dbg > logOn) Report(1, "[%s] gpsParse(buf)=%d gps_valid=%d\n", __func__, res, gps_valid);
                        if ((res == ixRMC) && gps_valid) {
                            mk_dataEGTS();
                            res = msgPutEgts((void *)&loc_data, sizeof(s_loc_data));
                            if (dbg > logOn) Report(1, "[%s] msgPutEgts(%p, %lu)=%d\n", __func__, (void *)&loc_data, sizeof(s_loc_data), res);
                        }
                    } else {
                        if (dbg > logOn) Report(1, "[%s] Error gpsValidate(buf)=%d\n", __func__, res);
                        usleep(1000);
                    }
                    //
                    lenr = lenr_tmp = 0;
                    ukb = 0;
                    memset(from_dev, 0, sizeof(from_dev));
                }
            }//if (FD_ISSET(fd, &Fdr))
        }//if (select
        else
        usleep(4000);    
        //---------------------------------------------------
    }//while (!QuitAll)

    

done:

    Report(1, "[%s] Done read_serial function\n", __func__);

    //if (total_threads) total_threads--;

    //pthread_exit(NULL);


}
//-------------------------------------------------------------------------------------------
