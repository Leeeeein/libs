#include <stdio.h>
#include <string.h>
#include "network.h"
#include "rpc.h"
#include <sys/select.h>

// 服务端的加法服务
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

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}
	rpc_init();
	// Create server socket
	int sockfd = create_server(argv[1], atoi(argv[2]));
	if (sockfd < 0) {
		fprintf(stderr, "Failed to create server\n");
		return 1;
	}
	
	// 在服务端发布服务
	rpc_publish("add", add);
	rpc_publish("subtract", subtract);
	
	
	char request_buf[1024];
	// char response_buf[1024];
	
	// 用于 select 的文件描述符集合
	fd_set fds;
	// 将服务端 socket 加入到文件描述符集合中
	FD_SET(sockfd, &fds);
	// 记录最大文件描述符
	int max_fd = sockfd;
	
	// 循环处理客户端的请求
	while (1) {
		// 每次调用 select 前都要将 fds 重置为原来的值
		fd_set read_fds = fds;
		int ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
		if (ret < 0) {
			fprintf(stderr, "select error\n");
			break;
		}
		
		// 检查服务端 socket 是否有新的连接
		if (FD_ISSET(sockfd, &read_fds)) {
			struct sockaddr_in client_addr;
			socklen_t addrlen = sizeof(client_addr);
			int client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
			if (client_sock < 0) {
				fprintf(stderr, "accept error\n");
				continue;
			}
			// 将新的客户端 socket 加入到文件描述符集合中
			FD_SET(client_sock, &fds);
			// 更新最大文件描述符
			if (client_sock > max_fd) {
				max_fd = client_sock;
			}
		}
		
		// 检查已连接的客户端是否有数据需要读取
		for (int fd = 0; fd <= max_fd; ++fd) {
			if (FD_ISSET(fd, &read_fds) && fd != sockfd) {
				// 从客户端接收请求
				printf("wait for recv.\n");
				int type = 0;
				int recv_len = recv_packet(fd, &type, request_buf, sizeof(request_buf), 0);
				printf("!!!!!!!!!!!!!!!!!!!!!!!!!! recv length: %d\n", recv_len);
				//if (recv_len <= 0) {
				//  close(fd);
				//  continue;
				//}
				printf("get a request to process: %s\n", request_buf);
				// 解析请求
				rpc_request_t* request = rpc_deserialize_request(request_buf);
		
				// 执行请求
				rpc_response_t* response = rpc_execute_request(request);
		
				// 序列化响应
				char* response_str = rpc_serialize_response(response);
		
				// 将响应发送给客户端
				send_packet(fd, 0,response_str, sizeof(response_str), 0);
				printf("send finish.\n");
			}
		}
	}
	return 0;
}

/*
#include <stdio.h>
#include <string.h>
#include "network.h"
#include "rpc.h"

// 服务端的加法服务
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
	// 在服务端发布服务
	rpc_publish("add", add);
	rpc_publish("subtract", subtract);

	char request_buf[1024];
	char response_buf[1024];
	
	// 循环接收客户端的请求
	while (1) {
		struct sockaddr_in client_addr;
		socklen_t addrlen = sizeof(client_addr);
		int client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
		// 从客户端接收请求
		printf("wait for recv.\n");
		recv(client_sock, request_buf, sizeof(request_buf), 0);
		printf("get a request to process: %s\n", request_buf);
		// 解析请求
		rpc_request_t* request = rpc_parse_request(request_buf);

		// 执行请求
		rpc_response_t* response = rpc_execute_request(request);

		// 序列化响应
		char* response_str = rpc_serialize_response(response);

		// 将响应发送给客户端
		send(client_sock, response_str, sizeof(response_str), 0);
		printf("send finish.\n");
	}
}*/
