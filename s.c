#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "network.h"

#define MAX_CLIENTS 10

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
        return 1;
    }
    // Create server socket
    int sockfd = create_server(argv[1], atoi(argv[2]));
    if (sockfd < 0) {
        fprintf(stderr, "Failed to create server\n");
        return 1;
    }

    // Set up file descriptor sets for select
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    int maxfd = sockfd;

    // Set up client file descriptor array
    int clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = -1;
    }
	char read_buf[256];
    while (1) {
    	FD_ZERO(&readfds);
    	FD_SET(sockfd, &readfds);
    	for (int i = 0; i < MAX_CLIENTS; i++) {
      		if (clients[i] != -1) {
        		FD_SET(clients[i], &readfds);
        		if(clients[i] > maxfd)
        		{
        			maxfd = clients[i];
        		}
      		}
    	}
        // Wait for activity on any file descriptor
        int nfds = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (nfds < 0) {
            perror("select");
            break;
        }
        // Check for activity on server socket
        if (FD_ISSET(sockfd, &readfds)) {
            // Accept client connection
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
            if (clientfd < 0) {
                perror("accept");
                continue;
            }
            else
            {
            	printf("accept succes. ");
            }

            // Add client to client array and update maxfd
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] < 0) {
                	printf("clients[%d] = %d\n", i, clientfd);
                    clients[i] = clientfd;
                    if (clientfd > maxfd) {
                        maxfd = clientfd;
                    }
                    break;
                }
            }
        }
        // Check for activity on client sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int clientfd = clients[i];
            if (clientfd > 0 && FD_ISSET(clientfd, &readfds)) {
                // Read packet from client
                /*packet *pkt = read_packet(clientfd);
                if (pkt == NULL) {
                    // Disconnect client
                    close(clientfd);
                    clients[i] = -1;
                } else {
                    // Process packet...
                    printf("Received packet from client with size %ld, content is: %s", pkt->size, (char*)pkt->data);
                    free_packet(pkt);
                }*/
                int type = 0;
                int read_ret = recv_packet(clientfd, &type, read_buf, 256, 0);
                read_ret = 0;
                printf("收到数据内容：%s [%d]\n", read_buf, read_ret);
            }
        }
    }

    // Close server socket and client sockets
	close(sockfd);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] > 0) {
            close(clients[i]);
        }
    }

    return 0;
}
