#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_PACKET_SIZE 65536

typedef struct {
	size_t size;
	int type;
} dataHeader;

typedef struct {
    dataHeader header;
    void *data;
} packet;

int create_server(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    
    // set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // bind address with socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -2;
    }
    
    // set port reuse
	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // listen socket
    if (listen(sockfd, 10) < 0) {
        perror("listen");
        close(sockfd);
        return -3;
    }

    return sockfd;
}

int create_client(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("socket:");
        return -1;
    }

    // set client address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // connect server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int create_connect(int sockfd, fd_set* p_fds, int* max_fd)
{
	struct sockaddr_in client_addr;
	socklen_t addrlen = sizeof(client_addr);
	int client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
	if(client_sock < 0)
	{
		return -1;
	}

	FD_SET(client_sock, p_fds);
	if(client_sock > *max_fd){
		*max_fd = client_sock ;
	}

	return client_sock;
}
		
int send_packet(int sockfd, int type, const void *buf, size_t len, int flags) {
    packet p;
    p.header.size = len;
    p.header.type = type;
    p.data = (void *)buf;
    void *buffer = malloc(len + sizeof(dataHeader));
    memcpy(buffer, &p.header, sizeof(dataHeader));
    memcpy(((char*)buffer) + sizeof(dataHeader), p.data, p.header.size);
    int res = send(sockfd, buffer, len + sizeof(dataHeader), flags);
    free(buffer);
    return res;
}

int recv_packet(int sockfd, int* type, void *buf, size_t len, int flags)
{	
	dataHeader header;
	header.size = 0;
	header.type = 0;
	if(0 == recv(sockfd, &header, sizeof(dataHeader), 0))
	{
		return 0;
	}
	*type = header.type;
	return recv(sockfd, buf, header.size, flags);
}

void free_packet(packet *pkt) {
    free(pkt->data);
    free(pkt);
}
