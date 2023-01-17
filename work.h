#ifndef DISTRIBUTED_NODE_H
#define DISTRIBUTED_NODE_H

// 初始化子节点
void node_init(const char* node_id, const char* scheduler_hostname, int scheduler_port);

// 向调度机汇报任务执行状态
void node_report_task_status(const char* task_id, int status);

// 获取当前待执行的任务
char* node_get_next_task();

// 释放子节点资源
void node_cleanup();

// 接受命令
void* thread_command();

#endif
