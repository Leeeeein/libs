#ifndef DISTRIBUTED_SCHEDULER_H
#define DISTRIBUTED_SCHEDULER_H

#define MAX_NODES 1000
#define MAX_TASKS 10000
#define INT_MAX 5
// 定义负载均衡策略结构体
typedef struct {
  int type;	// 负载均衡策略类型
  void* params;	// 相关参数
} LoadBalancingStrategy;

typedef struct node {
    int fd;
} node;

typedef struct task {
    char id[32];
    char command[1024];
    int num_dependencies;
    char** dependencies;
    int status;
} task;

typedef struct scheduler {
    int num_nodes;
    struct node* nodes[MAX_NODES];
    int num_tasks;
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
int scheduler_remove_node(scheduler* sche, const char* node_id);

// 向调度机提交一个任务
void scheduler_submit_task(scheduler* sche, const char* task_id, const char* command, const char* dependencies, int num_dependencies);

// 从调度机中撤销一个任务
void scheduler_cancel_task(scheduler* sche, const char* task_id);

// 获取任务的执行状态
int scheduler_get_task_status(scheduler* sche, const char* task_id, int* status);

// 释放调度机资源
void scheduler_cleanup(scheduler* sche);

// 设置负载均衡策略
void scheduler_set_load_balancing_strategy(scheduler* sche, LoadBalancingStrategy* strategy, int num_nodes);
#endif
