
#ifndef DISTRIBUTED_MANAGER_H
#define DISTRIBUTED_MANAGER_H

#include "log.h"
#include "network.h"
#include "rpc.h"
#include "scheduler.h"
#include "config.h"
#include "message_queue.h"
#include "common_defs.h"

typedef void (*CallbackFunc)(int);
typedef struct 
{
	int num_nodes;
	char* result_cache;
	HMM_PHASES* enumStatus;
} thread_fetch_paras;

// 绑定消息通知回调
void bindCallback(CallbackFunc func);

// 客户端消息处理线程
void* thread_client();

// 接受输入的命令，方便调试
void* thread_command();

// 从消息队列中取结果数据的线程
void* thread_fetch(thread_fetch_paras* args);

// 信息处理
void message_process(int type, int fd, int recv_size, char* request_buf);

// 处理rpc消息的分支函数
void process_rpc_message(int socket_fd, char* request_buf);

// 处理raw消息的分支函数
void process_raw_message(int socket_fd, int recv_size, char* request_buf);

// 处理result消息的分支函数
void process_res_message(int socket_fd, char* request_buf);

// 订阅的rpc函数
char* rpc_join_cluster(const char* params);

char* rpc_exit_cluster(const char* params);

scheduler* distributed_manager_get_scheduler();
// 向分布式管理机添加一个子节点
int distributed_manager_add_node(int node_id);

// 从分布式管理机中移除一个子节点
int distributed_manager_remove_node(int node_id);

// 向分布式管理机提交一个任务
int distributed_manager_submit_task(const char* task_id, const char* command, const char* dependencies, int num_dependencies);

// 从分布式管理机中撤销一个任务
int distributed_manager_cancel_task(const char* task_id);

// 获取任务的执行状态
int distributed_manager_get_task_status(const char* task_id);

// 启动指定编号的任务
int distributed_manager_launch_specified_task(const char* task_id, int max_nodes, char* file, TASK_DESCRIPTION* desc);

// 启动指定编号的任务, file需要进行处理，提取公共部分
int distributed_manager_launch_specified_task2(const char* task_id, int max_nodes, char* commData, int commDataLen, char* file, TASK_DESCRIPTION* desc);

// 启动指定编号的任务，但是数据的分发和节点的选择在外部完成，在函数中直接发送
int distributed_manager_launch_specified_task_divide(const char* task_id, int node_id, char* file, int dataLen);

// 设置负载均衡策略
void distributed_manager_set_load_balancing_strategy(const char* strategy);

// 释放分布式管理机资源
void distributed_manager_cleanup();

// manager作为模块启动
void launch_server(char** argv);
#endif
