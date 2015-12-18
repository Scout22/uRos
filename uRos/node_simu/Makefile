GENFLAGS=-g -Wall -std=c99
LDFLAGS=-L fastsim -lfastsim
COMFILEDIR=../common_file

all: node_simu

node_simu: node_simu.o $(COMFILEDIR)/node.o
	gcc node_simu.o $(COMFILEDIR)/node.o -L fastsim -lpthread -lfastsim -o node_simu $(GENFLAGS)

node_simu.o: node_simu.c  $(COMFILEDIR)/node.h
	gcc -c -I fastsim node_simu.c $(GENFLAGS) -D_BSD_SOURCE

node.o: $(COMFILEDIR)/node.c $(COMFILEDIR)/node.h
	gcc -c $(COMFILEDIR)/node.c $(GENFLAGS)

clean: 
	rm *.o $(COMFILEDIR)/node.o node_simu *.fifo *.txt
