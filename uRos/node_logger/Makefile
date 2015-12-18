GENFLAGS=-g -Wall -std=gnu99
LDFLAGS=
COMFILEDIR=../common_file

all: node_logger

node_logger: $(COMFILEDIR)/node.o $(COMFILEDIR)/signal_function.o node_logger_main.o
	gcc $(COMFILEDIR)/node.o $(COMFILEDIR)/signal_function.o node_logger_main.o -o node_logger $(GENFLAGS)

node.o: $(COMFILEDIR)/node.c $(COMFILEDIR)/node.h
	gcc -c $(COMFILEDIR)/node.c $(GENFLAGS)

signal_function.o: $(COMFILEDIR)/signal_function.c $(COMFILEDIR)/signal_function.h
	gcc -c $(COMFILEDIR)/signal_function.c $(GENFLAGS)

node_logger_main.o: node_logger_main.c $(COMFILEDIR)/node.h $(COMFILEDIR)/signal_function.h 
	gcc -c node_logger_main.c $(GENFLAGS) -D_POSIX_C_SOURCE

clean: 
	rm -fv *.o $(COMFILEDIR)/node.o node_logger  *.fifo *.txt
