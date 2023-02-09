CC = gcc
CFLAGS = -Wall -fPIC -lpthread

all: disMana server_example worker_example

LIB_PATH := $(PWD)
LIB_FILE := $(LIB_PATH)
	
worker_example: worker_example.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o worker_example worker_example.c network.c rpc.c $(wildcard *.so)
	
disMana: distributed_manager.c $(wildcard *.so)
	$(CC) $(CFLAGS) -shared -o libdisMana.so network.c rpc.c config.c log.c message_queue.c map.c scheduler.c distributed_manager.c

ptest: ptest.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o ptest config.c network.c log.c message_queue.c work.c ./ptest.c
	
server_example: server_example.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o server_example server_example.c -L. -ldisMana message_queue.c log.c rpc.c network.c scheduler.c config.c map.c common_defs.c -Wl,-rpath=.

%.so: %.c
	$(CC) $(CFLAGS) -shared -o $@ $^
	
