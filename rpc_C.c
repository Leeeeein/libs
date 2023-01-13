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
  char response_buf[1024];

  // 发起第一次远程调用
  /*rpc_request_t request1 = {
    .method = "add",
    .params = "15,25"
  };
  char* request1_str = rpc_serialize_request(&request1);
  int typ1 = 0;
  send_packet(sockfd, RPC, request1_str, strlen(request1_str)+1, 0);
  recv_packet(sockfd, &typ1, response_buf, sizeof(response_buf), 0);
  rpc_response_t* response1 = rpc_deserialize_response(response_buf);
  printf("request line is: %s, Result1: %s\n", request1_str, response1->result);
  */
  
  // 发起第二次远程调用
  rpc_request_t request2 = {
    .method = "join_cluster",
    .params = "45,20"
  };
  char* request2_str = rpc_serialize_request(&request2);
  int typ2 = 0;
  send_packet(sockfd, RPC, request2_str, strlen(request2_str)+1, 0);
  recv_packet(sockfd, &typ2, response_buf, sizeof(response_buf), 0);
  rpc_response_t* response2 = rpc_deserialize_response(response_buf);
  printf("request line is: %s, Result2: %s\n", request2_str, response2->result);
  
  
  // 发起第三次远程调用
  rpc_request_t request3 = {
    .method = "exit_cluster",
    .params = "30,35"
  };
  int typ3 = 0;
  char* request3_str = rpc_serialize_request(&request3);
  send_packet(sockfd, RPC, request3_str, strlen(request3_str)+1, 0);
  recv_packet(sockfd, &typ3, response_buf, sizeof(response_buf), 0);
  rpc_response_t* response3 = rpc_deserialize_response(response_buf);
  printf("request line is: %s, Result1: %s\n", request3_str, response3->result);

  

  //while(1){}
  close(sockfd);

  return 0;
}
