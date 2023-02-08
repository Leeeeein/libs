#include <stdio.h>
#include <string.h>
#include "network.h"
#include "rpc.h"

int main(int argc, char *argv[]) {
	// 检查参数
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}

	// 创建客户端
	int sockfd = create_client(argv[1], atoi(argv[2]));
	if (sockfd < 0) {
		return 1;
	}
	
  // char request_buf[1024];
  char response_buf[1024*1024];

  // 发起远程调用
  rpc_request_t request2 = {
    .method = "join_cluster",
    .params = "45,20"
  };
  char* request2_str = rpc_serialize_request(&request2);
  int typ2 = 0;
  send_packet(sockfd, RPC, request2_str, strlen(request2_str)+1, 0);
  printf("test 1.\n");
  recv_packet(sockfd, &typ2, response_buf, sizeof(response_buf), 0);
  printf("test 2.\n");
  rpc_response_t* response2 = rpc_deserialize_response(response_buf);
  printf("request line is: %s, Result2: %s\n", request2_str, response2->result);
  
  recv_packet(sockfd, &typ2, response_buf, 1024*1024, 0);
  recv_packet(sockfd, &typ2, response_buf, 1024*1024, 0);
  printf("%s", response_buf);
  send_packet(sockfd, RES, response_buf, strlen(response_buf)+1, 0);
  
  while(1){}
  close(sockfd);

  return 0;
}
