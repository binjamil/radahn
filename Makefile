CC=gcc
CFLAGS=-O0 -Wall -g

.PHONY: all clean

BINS=server client
LDFLAGS=
all: $(BINS)

%.o: %.cc
	$(CC) $(CFLAGS) $^ -c

client: client.o
	$(CC) $(LDFLAGS) $^ -o $@

server: server.o
	$(CC) $(LDFLAGS) $^ -lpthread -o $@

clean:
	rm -rf *.o $(BINS)
