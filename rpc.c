#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpc.h"


rpc_service_t* services = NULL;
int num_services;

int rpc_init()
{	
	if (services != NULL) {
        return -1;  // already initialized
    }

    services = malloc(MAX_SERVICES*sizeof(rpc_service_t));
    num_services = 0;
    
    return 0;
}

void rpc_publish(const char* name, rpc_function_t func) {
  if (num_services >= MAX_SERVICES) {
    fprintf(stderr, "Error: Too many RPC services\n");
    exit(1);
  }
  services[num_services].name = strdup(name);
  services[num_services].func = func;
  ++num_services;
}

void rpc_withdraw(const char* name) {
  for (int i = 0; i < num_services; ++i) {
    if (strcmp(services[i].name, name) == 0) {
      free(services[i].name);
      for (int j = i; j < num_services - 1; ++j) {
        services[j] = services[j + 1];
      }
      --num_services;
      return;
    }
  }
  fprintf(stderr, "Error: RPC service not found: %s\n", name);
  exit(1);
}

rpc_response_t* rpc_execute_request(int fd, const rpc_request_t* request) {
  rpc_response_t* response = (rpc_response_t*)malloc(sizeof(rpc_response_t));
  response->error = NULL;
  for (int i = 0; i < num_services; ++i) {
    if (strcmp(services[i].name, request->method) == 0) {
      response->result = services[i].func(request->params);
      return response;
    }
  }
  response->error = "Error: RPC service not found";
  return response;
}

char* rpc_serialize_response(const rpc_response_t* response)
{
  if (response->error != NULL) {
    return strdup(response->error);
  }
  else {
    return strdup(response->result);
  }
}

rpc_response_t* rpc_deserialize_response(const char* response_str) {
  rpc_response_t* response = (rpc_response_t*)malloc(sizeof(rpc_response_t));
  response->error = NULL;
  response->result = strdup(response_str);
  return response;
}

char* rpc_serialize_request(const rpc_request_t* request) {
  char* request_str = (char*)malloc(1024);
  sprintf(request_str, "func:{%s},params:{%s}.", request->method, request->params);
  return request_str;
}

rpc_request_t* rpc_deserialize_request(const char* request_str) {
  	printf("server received a request to parse: %s.\n", request_str);
	rpc_request_t* request = (rpc_request_t*)malloc(sizeof(rpc_request_t));

	char* start = strstr(request_str, "func:{");
	char* end = strstr(request_str, "},params:{");
	int len = end - start - 6;  // 6是"func:{"的长度
	request->method = (char*)malloc(len + 1);  // +1是留出结尾的'\0'
	strncpy(request->method, start + 6, len);
	request->method[len] = '\0';

	char* s_start = strstr(request_str, "socket[");
	char* s_end = strstr(request_str, "].");
	int s_len = s_end-s_start-7;
	
	start = strstr(request_str, "params:{");
	end = strstr(request_str, "}.");
	len = end - start - 8;  // 8是"params:{"的长度
	
	request->params = (char*)malloc(len + s_len + 1 + 1);  // 例如params:{1,2}.socket[11].   生成参数字符串“1,2,11”    len对应着“1,2”,s_len对应着“11”，1对应着“\0”, 第二个1对应着“,”
	strncpy(request->params, start + 8, len);
	strcat(request->params, ",");
	strncpy(request->params + len + 1, s_start + 7 , s_len);
	request->params[len + s_len + 1] = '\0';
	
	printf("request's method: %s, request's params: %s\n", request->method, request->params);


  	return request;
}

int rpc_service_exists(const char* name) {
  for (int i = 0; i < num_services; ++i) {
    if (strcmp(services[i].name, name) == 0) {
      return 1;
    }
  }
  return 0;
}

void rpc_client_call(int client_sockfd, const char* method, const char* params, rpc_callback_t callback, void* data) {
  char* result = (char*)malloc(1024);
  sprintf(result, "Result: %s(%s)", method, params);
  callback(data, NULL, result);
  free(result);
}
