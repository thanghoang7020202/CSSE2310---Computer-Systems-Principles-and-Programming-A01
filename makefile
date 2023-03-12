all: uqwordiply
uqwordiply: uqwordiply.c support.h
	gcc -Wall -pedantic -std=gnu99 -o $@ $<
clean:
	rm -f uqwordiply