.PHONY:    all clean
CC        = gcc
GENFLAGS=-g -Wall -std=gnu99
LDFLAGS=
COMFILEDIR=../common_file

all: master

master:  $(COMFILEDIR)/signal_function.o master_main.o master.o
	gcc master.o $(COMFILEDIR)/signal_function.o master_main.o -o master $(GENFLAGS)

master.o: master.c master.h $(COMFILEDIR)/signal_function.h
	gcc -c master.c $(GENFLAGS) -D_POSIX_C_SOURCE

signal_function.o: $(COMFILEDIR)/signal_function.c $(COMFILEDIR)/signal_function.h
	gcc -c $(COMFILEDIR)/signal_function.c $(GENFLAGS)

master_main.o: master_main.c master.h $(COMFILEDIR)/signal_function.h 
	gcc -c master_main.c $(GENFLAGS) -D_POSIX_C_SOURCE

clean: 
	rm -fv *.o $(COMFILEDIR)/signal_function.o master
