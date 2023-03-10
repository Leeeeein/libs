#ifndef DISTRIBUTED_SCHEDULER_H
#define DISTRIBUTED_SCHEDULER_H

#define MAX_NODES 1000
#define MAX_TASKS 3
#define MAX_TASK_ID_SIZE 128
#define MAX_DATAFILE_SIZE 1024*1024
#define INT_MAX 5

#include "map.h"

// 定义负载均衡策略结构体
typedef struct {
  int type;	// 负载均衡策略类型
  void* params;	// 相关参数
} LoadBalancingStrategy;

typedef struct node {
    int fd;
    int status;
} node;

typedef struct task {
    char id[32];
    //char command[1024];
    //int num_dependencies;
    //char** dependencies;
    char dataFile[MAX_DATAFILE_SIZE];
    int status;
    int priority;
} task;

typedef struct scheduler {
    int num_nodes;
    int num_tasks;
    struct node* nodes[MAX_NODES];
    struct task* tasks[MAX_TASKS];
} scheduler;

enum strategy
{
	ROUND_ROBIN = 0,
	MIN_LOAD
};


scheduler* get_scheduler();

// 初始化调度机
int scheduler_init();

// 向调度机添加一个子节点
int scheduler_add_node(int node_id);

// 从调度机中移除一个子节点
int scheduler_remove_node(int node_id);

// 向调度机提交一个任务
int scheduler_submit_task(const char* task_id, const char* command, const char* dependencies, int num_dependencies);

// 从调度机中撤销一个任务
int scheduler_cancel_task(const char* task_id);

// 获取任务的执行状态
int scheduler_get_task_status(scheduler* sche, const char* task_id, int* status);

// 查看当前在线节点数量
int scheduler_get_nodes_num();

// 将一个节点的状态设置为可用
int scheduler_set_node_usable(int fd);

// 获取当前可以使用的计算节点fd数组
int scheduler_get_usable_nodes(int* nodes_fd, int max_nodes_num);

// 释放调度机资源
void scheduler_cleanup(scheduler* sche);

// 设置负载均衡策略
void scheduler_set_load_balancing_strategy(scheduler* sche, LoadBalancingStrategy* strategy, int num_nodes);
#endif
