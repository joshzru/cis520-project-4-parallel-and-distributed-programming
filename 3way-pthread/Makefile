CC = gcc
CFLAGS = -Wall -O2 -pthread

all: max_ascii_pthread

max_ascii_pthread: max_ascii_pthread.c file_io.c file_io.h
	$(CC) $(CFLAGS) -o max_ascii_pthread max_ascii_pthread.c file_io.c

clean:
	rm -f max_ascii_pthread *.o