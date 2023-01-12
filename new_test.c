#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "rpc.h"

#define BUF_SIZE 1024

int create_servers(const char* addr, int port) {
  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(addr);
  serv_addr.sin_port = htons(port);
  bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  listen(serv_sock, 20);
  return serv_sock;
}

char* add(const char* params) {
  int a, b;
  sscanf(params, "%d,%d", &a, &b);
  static char result[16];
  sprintf(result, "%d", a + b);
  return result;
}

// 服务端的减法服务
char* subtract(const char* params) {
  int a, b;
  sscanf(params, "%d,%d", &a, &b);
  static char result[16];
  sprintf(result, "%d", a - b);
  return result;
}

// 测试代码开始
int main(int argc, char *argv[]) {

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}

	// 创建客户端
	int serv_sock = create_servers(argv[1], atoi(argv[2]));
	if (serv_sock < 0) {
		return 1;
	}

  if (rpc_init() != 0) {
    fprintf(stderr, "RPC library initialization failed\n");
    exit(1);
  }

  rpc_publish("add", &add);
  rpc_publish("subtract", &subtract);

	struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  int clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
  while (1) {
    

    char request_str[BUF_SIZE];
    int recv_len = recv(clnt_sock, request_str, BUF_SIZE - 1, 0);
    if (recv_len <= 0) {
      close(clnt_sock);
      continue;
    }
    request_str[recv_len] = '\0';

    rpc_request_t* request = rpc_deserialize_request(request_str);
    rpc_response_t* response = rpc_execute_request(request);

    char* response_str = rpc_serialize_response(response);
    send(clnt_sock, response_str, strlen(response_str), 0);
    free(response_str);

    // close(clnt_sock);
  }

  close(serv_sock);

  return 0;
}
