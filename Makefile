CC=clang++
#CFLAGS=-std=c++98 -Wall -O2
CFLAGS = -std=c++17 -Wall -O2
CLIBS = -lm

all : main.cpp
		$(CC) $(CFLAGS) -o dutch main.cpp

.PHONY: clean

clean :
		-rm dutch