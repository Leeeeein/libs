#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "scheduler.h"

static scheduler* sche = NULL;

scheduler* get_scheduler()
{
	return sche;
}

int scheduler_init() {
    if (sche != NULL) {
        return -1;  // already initialized
    }
    sche = malloc(sizeof(scheduler));
    sche->num_nodes = 0;
    return 0;
}

int scheduler_add_node(int node_id) {
    if (sche == NULL) {
        return -1;  // sche not initialized
    }
    if (sche->num_nodes == MAX_NODES) {
        return -2;  // too many nodes
    }
    node* new_node = malloc(sizeof(node));
    new_node->fd = node_id;
    new_node->status = 0;
    sche->nodes[sche->num_nodes] = new_node;
    sche->num_nodes++;
    return 0;
}

int scheduler_remove_node(int node_id) {
    if (sche == NULL) {
        return -1;  // sche not initialized
    }
    if(sche->num_nodes == 0)
    {
    	return -2;
    }

    int found = 0;
    for (int i = 0; i < sche->num_nodes; i++) {
        if (sche->nodes[i]->fd == node_id) {
            found = 1;
            //free(sche->nodes[i]);
            //sche->nodes[i] = sche->nodes[sche->num_nodes - 1];
            // 两种情况，1是i就是最后一个元素，这时直接删除就行，2是i不是最后一个元素，那就覆盖
            if(i == sche->num_nodes-1)
            {
            	free(sche->nodes[i]);
            }
            else
            {
            	sche->nodes[i]->fd = sche->nodes[sche->num_nodes - 1]->fd;
            	sche->nodes[i]->status = sche->nodes[sche->num_nodes - 1]->status;
            	free(sche->nodes[sche->num_nodes-1]);
            }
            sche->num_nodes--;
            break;
        }
        printf("node exist: %d", sche->nodes[i]->fd);
    }
    printf(" node number: %d\n", sche->num_nodes);
    printf("\n");
    if (!found) {
        return -2;  // node not found
    }
    return 0;
}

int scheduler_get_nodes_num() {
	return sche->num_nodes;
}

int scheduler_get_usable_nodes(int* nodes_fd, int max_nodes_num) {
	int num_nodes = sche->num_nodes;
	int occu_cnt = 0;
	int idx = 0;
	for(int i = 0; i < num_nodes; i++)
	{
		if(occu_cnt == max_nodes_num)
		{
			break;
		}
		node* cur_node = sche->nodes[i];
		if(1 == cur_node->status)
		{
			continue;
		}
		nodes_fd[idx] = cur_node->fd;
		cur_node->status = 1;
		++idx;
		++occu_cnt;
	}
	return occu_cnt;
}

void scheduler_cleanup(scheduler* sche) {
    if (sche == NULL) {
        return;  // sche not initialized
    }
    for (int i = 0; i < sche->num_nodes; i++) {
        free(sche->nodes[i]);
    }
    free(sche);
    sche = NULL;
}

int scheduler_submit_task(const char* task_id, const char* command, const char* dependencies, int num_dependencies) {
    return 0;
}

int scheduler_cancel_task(const char* task_id) {
    return 0;
}

int scheduler_get_task_status(scheduler* sche, const char* task_id, int* status) {
    return 0;
}

void scheduler_set_load_balancing_strategy(scheduler* scheduler, LoadBalancingStrategy* strategy, int num_nodes) {
  // 根据策略类型进行不同的处理
  /*switch (strategy->type) {
    case ROUND_ROBIN: {
      // 使用轮询策略，相关参数为当前轮询到的节点编号
      int* current_node = (int*)strategy->params;
      // 将任务分配给下一个节点
      *current_node = (*current_node + 1) % num_nodes;
      break;
    }
    case MIN_LOAD: {
      // 使用最小负载策略，相关参数为各个节点的负载信息
      int* node_loads = (int*)strategy->params;
      // 找到负载最小的节点并将任务分配给它
      int min_load = INT_MAX;
      int min_load_node = -1;
      for (int i = 0; i < num_nodes; i++) {
        if (node_loads[i] < min_load) {
          min_load = node_loads[i];
          min_load_node = i;
        }
      }
      // 将任务分配给负载最小的节点
      if (min_load_node >= 0) {
        *current_node = min_load_node;
      }
      break;
    }
    default: {
      // 未知的负载均衡策略，输出错误信息并退出
      fprintf(stderr, "Error: Unknown load balancing strategy.\n");
      exit(1);
    }
 	}*/
 }
