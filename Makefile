TESTS=$(patsubst %.cc,%,$(sort $(wildcard test[0-9][0-9].cc test[0-9][0-9][0-9a-z].cc test[0-9][0-9][0-9][a-z].cc)))
CC=g++
CFLAGS=-O0 -Wall -g
BINS=server

.PHONY: all clean

all: $(BINS)

%.o: %.cc
	$(CC) $(CFLAGS) $^ -c

test%: protocol.o handler.o test%.o testutil.o
	$(CC) $(LDFLAGS) $^ -o $@

check: $(TESTS)
	set -e; for t in $(TESTS); do ./$$t; done

server: server.o protocol.o handler.o
	$(CC) $(LDFLAGS) $^ -lpthread -o $@

clean:
	rm -rf *.o $(BINS) $(TESTS)
