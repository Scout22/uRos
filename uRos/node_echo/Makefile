GENFLAGS=-g -Wall -std=gnu99
LDFLAGS=
COMFILEDIR=../common_file

all: node_echo


node_echo: $(COMFILEDIR)/node.o $(COMFILEDIR)/signal_function.o node_echo_main.o
	gcc $(COMFILEDIR)/node.o $(COMFILEDIR)/signal_function.o node_echo_main.o -o node_echo $(GENFLAGS)

node.o: $(COMFILEDIR)/node.c $(COMFILEDIR)/node.h
	gcc -c $(COMFILEDIR)/node.c $(GENFLAGS)

signal_function.o: $(COMFILEDIR)/signal_function.c $(COMFILEDIR)/signal_function.h
	gcc -c $(COMFILEDIR)/signal_function.c $(GENFLAGS)

node_echo_main.o: node_echo_main.c $(COMFILEDIR)/node.h $(COMFILEDIR)/signal_function.h 
	gcc -c node_echo_main.c $(GENFLAGS) -D_POSIX_C_SOURCE

clean: 
	rm -fv *.o $(COMFILEDIR)/node.o $(COMFILEDIR)/signal_function.o node_echo  *.fifo 