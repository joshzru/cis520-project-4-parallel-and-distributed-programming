CC = gcc
CFLAGS = -Wall -O2 -pthread

all: max_ascii_pthread

max_ascii_pthread: max_ascii_pthread.c
	$(CC) $(CFLAGS) -o max_ascii_pthread max_ascii_pthread.c

clean:
	rm -f max_ascii_pthread