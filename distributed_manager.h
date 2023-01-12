// 集成scheduler、netowrk、rpc三个模块

// scheduler用来进行node和task的管理

// network用于本模块和distributed_nodes之间通信

// rpc用于服务端处理客户端请求

// 在manager中，首先初始化一个scheduler，提供对node和task管理的容器

// 其次建立好网络连接，以等待node的请求

// 最后启动rpc服务，并发布可以远程调用的接口清单


/*
具体操作为：
	1.使用RPC将scheduler的add、remove的接口发布
	2.submit task、cancel task的接口不需要发布，在本地的业务代码中，调用这两个接口时，对任务进行操作
	3.使用某种机制，对submit后的任务进行处理，可以使用的资源为nodes提供的资源
	
DM收到消息，用RPC提供的序列化反序列化机制处理，然后操纵调度机
*/
#include "log.h"
#include "network.h"
#include "rpc.h"
#include "scheduler.h"
#include "config.h"
#ifndef DISTRIBUTED_MANAGER_H
#define DISTRIBUTED_MANAGER_H

typedef struct distributed_manager
{
	IniConfigManager* m_cfg;	//manager配置文件管理器
	int socket_fd;	//manager占用的文件描述符
} DM;

// 客户端消息处理线程
void* thread_client(DM* dm);

// 接受输入的命令，方便调试
void* thread_command();

// 处理rpc消息分支的函数
void process_rpc_message(int socket_fd, char* request_buf);

// 处理raw消息分支的函数
void process_raw_message(int socket_fd, char* request_buf);

// 订阅的rpc函数
char* rpc_join_cluster();

// 初始化分布式管理机
int distributed_manager_init(DM* dm);

// 向分布式管理机添加一个子节点
int distributed_manager_add_node(int node_id);

// 从分布式管理机中移除一个子节点
int distributed_manager_remove_node(int node_id);

// 向分布式管理机提交一个任务
void distributed_manager_submit_task(DM* dm, const char* task_id, const char* command, const char* dependencies, int num_dependencies);

// 从分布式管理机中撤销一个任务
void distributed_manager_cancel_task(DM* dm, const char* task_id);

// 获取任务的执行状态
int distributed_manager_get_task_status(DM* dm, const char* task_id);

// 设置负载均衡策略
void distributed_manager_set_load_balancing_strategy(DM* dm, const char* strategy);

// 释放分布式管理机资源
void distributed_manager_cleanup(DM* dm);

#endif
