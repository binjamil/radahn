TESTS=$(patsubst %.cc,%,$(sort $(wildcard test[0-9][0-9].cc test[0-9][0-9][0-9a-z].cc test[0-9][0-9][0-9][a-z].cc)))

CC=g++
CFLAGS=-O0 -Wall -g

.PHONY: all clean

BINS=server
# LDFLAGS=-lstdc++
all: $(BINS) $(TESTS)

%.o: %.cc
	$(CC) $(CFLAGS) $^ -c

test%: protocol.o handler.o test%.o
	$(CC) $(LDFLAGS) $^ -o $@

check: $(TESTS)
	set -e; for t in $(TESTS); do ./$$t; done

server: server.o protocol.o handler.o
	$(CC) $(LDFLAGS) $^ -lpthread -o $@

clean:
	rm -rf *.o $(BINS) $(TESTS)
