.POSIX:
.SUFFIXES:

CC=cc
CFLAGS=-g -Wall -Werror

all: broadcaster pack selectserver select pollserver poll talker listener client server showip

broadcaster: broadcaster.c
	$(CC) $(CFLAGS) -o $@ $^

pack: pack.o util.o
	$(CC) $(CFLAGS) -o $@ $^

selectserver: selectserver.o util.o
	$(CC) $(CFLAGS) -o $@ $^

select: select.c
	$(CC) $(CFLAGS) -o $@ $^

pollserver: pollserver.o util.o
	$(CC) $(CFLAGS) -o $@ $^

poll: poll.c
	$(CC) $(CFLAGS) -o $@ $^

talker: talker.o util.o
	$(CC) $(CFLAGS) -o $@ $^

listener: listener.o util.o
	$(CC) $(CFLAGS) -o $@ $^

client: client.o util.o
	$(CC) $(CFLAGS) -o $@ $^

server: server.o util.o
	$(CC) $(CFLAGS) -o $@ $^

showip: showip.c
	$(CC) $(CFLAGS) -o $@ $^

util.o: util.c util.h

clean:
	rm -f broadcaster pack selectserver select pollserver poll talker listener client server showip *.o

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<

