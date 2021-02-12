CC := gcc
CFLAGS := -g -Wall -Werror

all: dataPar

clean:
	rm -rf dataPar

dataPar: main.c
	$(CC) $(CFLAGS) -o dataPar main.c matrix.c mreduce.c -lm -pthread
