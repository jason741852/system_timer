CC=gcc
CFLAGS=

all: hr-timer
program: hr-timer.o
proc_parse.o: hr-timer.c

clean:
	rm -f hr-timer hr-timer.o
run: hr-timer
	./hr-timer
