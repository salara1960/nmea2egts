
NAME=nmea2egts
proc=main
func_lib=func
egts_lib=egts
tcp_lib=tcp

CC=${CROSS_COMPILE}gcc
RM=rm

EXTRA_CFLAGS += -Wall -D_GNU_SOURCE
ifeq ($(TARGET), arm-buildroot-uclinux-uclibcgnueabi)
    NAME=nmea2egts_arm
    EXTRA_CFLAGS += -Os -mcpu=cortex-m7 -DARM
    LIB_DIR=
else
    NAME=nmea2egts
    EXTRA_CFLAGS += -O2 -DINTEL
    LIB_DIR=-lpthread -ldl
endif
export ${EXTRA_CFLAGS}


$(NAME):  $(func_lib).o $(egts_lib).o $(tcp_lib).o $(proc).o
	$(CC) -o $(NAME) $(func_lib).o $(egts_lib).o $(tcp_lib).o $(proc).o $(LIB_DIR)
$(proc).o: $(proc).c
	$(CC) -c $(proc).c ${EXTRA_CFLAGS}
$(func_lib).o: $(func_lib).c
	$(CC) -c $(func_lib).c ${EXTRA_CFLAGS}
$(egts_lib).o: $(egts_lib).c
	$(CC) -c $(egts_lib).c ${EXTRA_CFLAGS}
$(tcp_lib).o: $(tcp_lib).c
	$(CC) -c $(tcp_lib).c ${EXTRA_CFLAGS}

clean:
	$(RM) -f *.o $(NAME) $(NAME).*
