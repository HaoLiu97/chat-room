CFLAGS = -Wall -g -pthread
CC     = gcc $(CFLAGS)
OBJ_DIR = $(CUR_DIR)/bin

all: bl_server bl_client bl_showlog
# bl_server: bl_server
# bl_client: bl_client
# bl_showlog: bl_showlog
demo: simpio_demo

bl_server : bl_server.o util.o server_funcs.o
	$(CC) -o bl_server bl_server.o util.o server_funcs.o

bl_client : bl_client.o util.o simpio.o
	$(CC) -o bl_client bl_client.o util.o simpio.o

simpio_demo: simpio_demo.o simpio.o
	$(CC) -o simpio_demo simpio.o simpio_demo.o

bl_showlog: bl_showlog.o util.o
	$(CC) -o bl_client bl_showlog.o util.o

bl_server.o : bl_server.c
	$(CC) -c bl_server.c

server_funcs.o : server_funcs.c
	$(CC) -c server_funcs.c

bl_client.o : bl_client.c
	$(CC) -c bl_client.c

bl_showlog.o : bl_showlog.c
	$(CC) -c bl_showlog.c

util.o : util.c
	$(CC) -c util.c

simpio.o : simpio.c
	$(CC) -c simpio.c

simpio_demo.o: simpio_demo.c
	$(CC) -c simpio_demo.c

clean :
	rm -f bl_server bl_client bl_showlog simpio_demo *.o *.fifo
	rm -r test-results

include test_Makefile
