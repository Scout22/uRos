GENFLAGS=-g -Wall -std=gnu99
LDFLAGS= -lc

all: node_tele
COMFILEDIR=../common_file

node_tele: $(COMFILEDIR)/node.o node_tele_main.o
	gcc node_tele_main.o $(COMFILEDIR)/node.o -o node_tele $(GENFLAGS) $(LDFLAGS)

node.o: $(COMFILEDIR)/node.c $(COMFILEDIR)/node.h
	gcc -c $(COMFILEDIR)/node.c $(GENFLAGS) $(LDFLAGS)

node_tele_main.o: node_tele_main.c $(COMFILEDIR)/node.h
	gcc -c node_tele_main.c $(GENFLAGS) $(LDFLAGS)

clean: 
	rm *.o $(COMFILEDIR)/node.o node_tele
