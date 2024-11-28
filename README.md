# nmea2egts


This util conta

Compile this util on Linux OS :

```
make
gcc -c func.c -Wall -D_GNU_SOURCE -O2 -DINTEL
gcc -c egts.c -Wall -D_GNU_SOURCE -O2 -DINTEL
gcc -c tcp.c -Wall -D_GNU_SOURCE -O2 -DINTEL
gcc -c main.c -Wall -D_GNU_SOURCE -O2 -DINTEL
gcc -o nmea2egts func.o egts.o tcp.o main.o -lpthread -ldl
```


First start :

```
./nmea2egts help
28.11 17:53:10.885 | Start program with params: ./nmea2egts tcp=<server_port>|dev=<serial_port:speed> egts=<server_addr:server_port:termID
  help - show this message
  tcp=<tcp_server_port> - port for receive nmea message (default 9876)
  or
  dev=<serial_port:speed> - serial port and speed for receive nmea message (/dev/ttyUSB0:115200)
  egts=<server_ip:server_port:tid> - activate egts client, where tid - termID
  dbg=<level> - level : 'off', 'on', 'debug', 'dump' (default - on)
```


For example start :

```
./nmea2egts tcp=9876 egts=127.0.0.1:9999:10002 dbg=on
28.11 17:57:49.437 | Ver.0.1.1 28.11.2024 Start ./nmea2egts server
28.11 17:57:49.437 | Listen tcp client (port 9876)...
28.11 17:57:50.437 | [egts_cli_loop] Start egts_client thread for '127.0.0.1:9999' (pid:7231)
.
.
.
^C28.11 17:58:01.788 | [egts_cli_loop] Done egts_client thread
28.11 17:58:01.789 | Ver.0.1.1 28.11.2024 Stop ./nmea2egts server
```

or

```
./nmea2egts dev=/dev/ttyUSB0:115200 egts=127.0.0.1:9999:10003
28.11 18:04:46.796 | Ver.0.1.1 28.11.2024 Start ./nmea2egts server
28.11 18:04:46.798 | [main] Open device '/dev/ttyUSB0' Ok (fd=3)
28.11 18:04:46.798 | [loopDev] Start read_serial function for device '/dev/ttyUSB0' (fd=3)
28.11 18:04:47.799 | [egts_cli_loop] Start egts_client thread for '127.0.0.1:9999' (pid:7537)
.
.
.
^C28.11 18:05:11.029 | [loopDev] Done read_serial function
28.11 18:05:11.030 | [egts_cli_loop] Done egts_client thread
28.11 18:05:11.281 | Ver.0.1.1 28.11.2024 Stop ./nmea2egts server
```

