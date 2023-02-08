#ifndef RPC_H
#define RPC_H

#include <stdint.h>

#define RPC_REQUEST_MAX_LEN 128
#define RPC_RESPONSE_MAX_LEN 128
#define MAX_SERVICES 128

/// Define the type of RPC service function
typedef char* (*rpc_function_t)(const char* params);

typedef struct {
	char* name;
	rpc_function_t func;
} rpc_service_t;

// Define the structure of RPC request and response
typedef struct {
	char* method; // Name of the called method
	char* params; // Parameters of the call 
} rpc_request_t;

typedef struct {
	char* result; // Result of the call 
	char* error; // Error message of the call (if any)
} rpc_response_t;

rpc_service_t* get_rpc();
int rpc_init();
// Define the publish and withdraw functions of RPC service
void rpc_publish(const char* name, rpc_function_t func);
void rpc_withdraw(const char* name);

// Define the function to execute RPC request 
rpc_response_t* rpc_execute_request(int fd, rpc_request_t* request);

// Define the serialization and deserialization functions of RPC response 
char* rpc_serialize_response(const rpc_response_t* response);
rpc_response_t* rpc_deserialize_response(const char* response_str);

// Define the serialization and deserialization functions of RPC request 
char* rpc_serialize_request(const rpc_request_t* request);
rpc_request_t* rpc_deserialize_request(const char* request_str);

// Define the function to query RPC service 
int rpc_service_exists(const char* name);

// Type of callback function for asynchronous RPC call /
typedef void (*rpc_callback_t)(void* data, const char* err, const char* result);

// Asynchronously call RPC service
void rpc_client_call(int client_sockfd, const char* method, const char* params, rpc_callback_t callback, void* data);
#endif
