CFLAGS = -Wall -g -pthread
CC     = gcc $(CFLAGS)
OBJ_DIR = $(CUR_DIR)/bin

all: bl_server bl_client
demo: simpio_demo

bl_server : bl_server.o util.o server_funcs.o
	$(CC) -o bl_server bl_server.o util.o server_funcs.o

bl_client : bl_client.o util.o simpio.o
	$(CC) -o bl_client bl_client.o util.o simpio.o

simpio_demo: simpio_demo.o simpio.o
	$(CC) -o simpio_demo simpio.o simpio_demo.o

bl_server.o : bl_server.c blather.h
	$(CC) -c $<

server_funcs.o : server_funcs.c blather.h
	$(CC) -c $<

bl_client.o : bl_client.c blather.h
	$(CC) -c $<

util.o : util.c blather.h
	$(CC) -c $<

simpio.o : simpio.c blather.h
	$(CC) -c $<

simpio_demo.o: simpio_demo.c blather.h
	$(CC) -c $<

clean :
	rm -f bl_server bl_client simpio_demo *.o *.fifo
