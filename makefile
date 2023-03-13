cc = gcc
cflags = -Wall -pedantic -std=gnu99

all: uqwordiply
uqwordiply: uqwordiply.c support.h
	$(cc) $(cflags) -o $@ $<

clean:
	rm -f uqwordiply