CC = gcc
CFLAGS = -Wall -fPIC -lpthread

all:disMana 


LIB_PATH := $(PWD)
LIB_FILE := $(LIB_PATH)

#LOG: log.c $(wildcard *.so)
#	$(CC) $(CFLAGS) -o LOG log.c $(wildcard *.so)
	
c: c.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o c c.c network.c $(wildcard *.so)

s: s.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o s s.c network.c $(wildcard *.so)
	
rpc_S: rpc_S.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o rpc_SS rpc_S.c network.c rpc.c $(wildcard *.so)
	
rpc_C: rpc_C.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o rpc_CS rpc_C.c network.c rpc.c $(wildcard *.so)
	
disMana: distributed_manager.c $(wildcard *.so)
	$(CC) $(CFLAGS) -o disMana network.c rpc.c config.c log.c message_queue.c map.c scheduler.c ./distributed_manager.c

	
%.so: %.c
	$(CC) $(CFLAGS) -shared -o $@ $^
