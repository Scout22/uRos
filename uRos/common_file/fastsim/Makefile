CC = g++
CC_FLAGS = `sdl-config --cflags` -fPIC -O3 -Wall
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
NAME=fastsim

all: shared #static

static: $(OBJECTS)
	ar rcs lib$(NAME).a $(OBJECTS)

shared: $(OBJECTS)
	$(CC) -shared -Wl,-soname,lib$(NAME).so $^ `sdl-config --libs` -o lib$(NAME).so

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f lib$(NAME).so
	rm -f lib$(NAME).a
