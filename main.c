#include "func.h"
#include "egts.h"
#include "tcp.h"



int main(int argc, char *argv[])
{
fd_set Fds;
struct sockaddr_in srv_conn, cli_conn;
struct timeval mytv;
char chap[256] = {0};
pthread_t tid, tid_egts;
pthread_attr_t threadAttr, threadAttr_egts;
struct sigaction Act, OldAct;
int connsocket = -1, client = -1, resa, Vixod = 0, on = 1, ret = 0;
socklen_t srvlen = sizeof(struct sockaddr_in);
socklen_t clilen = srvlen;;


    //-------------------   for example:  ./nmea2egts tcp=9876 egts=127.0.0.1:6789:77770299   ---------------------


    for (int8_t i = 1; i < argc; i++) {
        strcpy(chap, argv[i]);
        if (parse_param_start(chap) == -1) return 1;
                                      else usleep(1000);
    }
    if (!strlen(ip)) {
        Report(1, "No egts server params. Example : ./%s tcp=<server_port> egts=<server_addr:server_port:termID>\n", argv[0]);
        return 1;
    }


    Report(1, "Ver.%s Start %s server\n", version, argv[0]);

    strcpy(dev, IMEI_DEF);
    strncpy(uNameFile, argv[0], sizeof(uNameFile) - 1);

    //--------------------  set Signals route function ------------------

    memset((uint8_t *)&Act,    0, sizeof(struct sigaction));
    memset((uint8_t *)&OldAct, 0, sizeof(struct sigaction));
    Act.sa_handler = &GetSignal_;
    Act.sa_flags   = 0;
    sigaction(SIGPIPE, &Act, &OldAct);
    sigaction(SIGHUP,  &Act, &OldAct);
    sigaction(SIGSEGV, &Act, &OldAct);
    sigaction(SIGTERM, &Act, &OldAct);
    sigaction(SIGABRT, &Act, &OldAct);
    sigaction(SIGINT,  &Act, &OldAct);
    sigaction(SIGSYS,  &Act, &OldAct);
    sigaction(SIGKILL, &Act, &OldAct);
    sigaction(SIGTRAP, &Act, &OldAct);

    //--------------------------------------------------------------------

    if (tcp_flag) {
        pthread_attr_init(&threadAttr);
        pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    } else {
        fd = init_uart(dev_name);
        if (fd < 0) {
            if (dbg) Report(1, "[%s] Error open device '%s' (fd=%d)\n", __func__, dev_name, fd);
            goto done;
        } else {
            if (dbg) Report(1, "[%s] Open device '%s' Ok (fd=%d)\n", __func__, dev_name, fd);
        }
    }

    pthread_attr_init(&threadAttr_egts);
    pthread_attr_setdetachstate(&threadAttr_egts, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid_egts, &threadAttr_egts, egts_cli_loop, (void *)ip)) {
        if (dbg) Report(1, "Error starting egts_client_loop thread\n");
    } else {
        total_threads++;
    }
    


    while (!QuitAll && !Vixod) {

        Vixod = 0;

        if (tcp_flag) {

            connsocket = socket(AF_INET, SOCK_STREAM, 6);

            if (connsocket < 0) {
                if (dbg) Report(1, "ERROR: open socket (%d)\n", connsocket);
                ret = 1;
                goto done;
            }

            if (setsockopt(connsocket, SOL_SOCKET, SO_REUSEADDR,(const char *) &on, sizeof(on))) {
                if (dbg) Report(1, "ERROR: Setsockopt - SO_REUSEADDR (%d)\n", connsocket);
                ret = 1;
                goto done;
            }

            memset(&srv_conn, 0, srvlen);
            srv_conn.sin_family = AF_INET;
            srv_conn.sin_addr.s_addr = htonl(INADDR_ANY);
            srv_conn.sin_port = htons(tcp_port);

            if (bind(connsocket, (struct sockaddr *) &srv_conn, srvlen) == -1) {
                if (dbg) Report(1, "ERROR: Bind (port %d)\n", tcp_port);
                goto out_of_job;
            }

            if (listen(connsocket, 3) == -1) {
                if (dbg) Report(1, "ERROR: Listen (port %d)\n", tcp_port);
                goto out_of_job;
            }
            fcntl(connsocket, F_SETFL, (fcntl(connsocket, F_GETFL)) | O_NONBLOCK);
            if (dbg) Report(1, "Listen tcp client (port %d)...\n", tcp_port);

            //pthread_attr_init(&threadAttr);
            //pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

            while (!Vixod) {
                if (QuitAll) break;
                resa = 0;
                FD_ZERO(&Fds);   FD_SET(connsocket,&Fds);
                mytv.tv_sec = 0;   mytv.tv_usec = 100000;
                resa = select(connsocket + 1, &Fds, NULL, NULL, &mytv);
                if (resa > 0) {
                    client = accept(connsocket, (struct sockaddr *) &cli_conn, &clilen);
                    if (client > 0) {
                        fcntl(client, F_SETFL, (fcntl(client, F_GETFL)) | O_NONBLOCK);
                        if (dbg) Report(1, "New client %s:%u online (socket %d)\n",
                                            (char *)inet_ntoa(cli_conn.sin_addr),
                                            htons(cli_conn.sin_port),
                                            client);
                        //--------------------------------------------------------------------
                        if (pthread_create(&tid, &threadAttr, loopTCP, &client)) {
                            if (dbg) Report(1, "Error start client thread for socket %d\n", client);
                            if (client) {
                                close(client);
                                client = -1;
                            }
                        }
                        //--------------------------------------------------------------------
                    }//if (client>0)
                }//if (resa>0)
                usleep(1000);
            }//while (!Vixod)

out_of_job:

            pthread_attr_destroy(&threadAttr);

            if (connsocket > 0) {
                shutdown(connsocket, SHUT_RDWR);
                close(connsocket);
            }
        } else {//tcp_flag
            loopDev(&fd);
        }

    }


done:


    resa = 12;
    while (total_threads) {
        usleep(250000);
        resa--; if (!resa) break;
    }

    if (tcp_flag)     
        pthread_attr_destroy(&threadAttr_egts);
    else
        deinit_uart(fd);

    Report(1, "Ver.%s Stop %s server\n", version, argv[0]);

    return ret;
}

//-----------------------------------------------------------------------------------------------------

