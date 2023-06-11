TESTS=$(patsubst %.cc,%,$(sort $(wildcard test[0-9][0-9].cc test[0-9][0-9][0-9a-z].cc test[0-9][0-9][0-9][a-z].cc)))

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

test%: protocol.o test%.o
	$(CC) $(LDFLAGS) $^ -o $@

server: server.o protocol.o handler.o
	$(CC) $(LDFLAGS) $^ -lpthread -o $@

clean:
	rm -rf *.o $(BINS) $(TESTS)
