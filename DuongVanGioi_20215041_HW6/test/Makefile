CC = gcc
CFLAGS = -Wall -g

all: server client

server: server.o
	$(CC) $(CFLAGS) -o $@ $^

client: client.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f server client server.o client.o