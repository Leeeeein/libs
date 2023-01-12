#ifndef NETWORK_LIBRARY_H
#define NETWORK_LIBRARY_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_PACKET_SIZE 65536

enum message_type{
	UNDEFINE = -1,
	RAW = 0,
	RPC = 1,
	COMMON,
};

typedef struct {
	size_t size;
	int type;
} dataHeader;

typedef struct {
    dataHeader header;
    void *data;
} packet;


#ifdef __cplusplus
extern "C" {
#endif

int create_server(const char *ip, int port);
int create_client(const char *ip, int port);
packet *read_packet(int fd);
int write_packet(int fd, packet *pkt);
int send_packet(int sockfd, int type, const void *buf, size_t len, int flags);
int recv_packet(int sockfd, int* type, void *buf, size_t len, int flags);
void free_packet(packet *pkt);

#ifdef __cplusplus
}
#endif

#endif  // NETWORK_LIBR
