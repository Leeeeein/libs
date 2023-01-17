#include <string.h>
#include <stdlib.h>

#include "node.h"
#include "network.h"
#include "config.h"
#include "log.h"
#include "message_queue.h"

struct node {
    int port;
    int socket;
    char id[32];
    char address[32];
};

static struct node Node;
static IniConfigManager* cfg = ini_config_manager_create("cfg_node.ini");
static message_queue taskQueue;
static char taskList[atoi(ini_config_manager_get(cfg, "max_task_num"))][atoi(ini_config_manager_get(cfg, "max_task_id_length"))];

void* thread_command()
{
	char buf[64];
	while(1)
	{
		scanf("%s", buf);
		size_t l = sizeof(buf);
		buf[l-1] = buf[l];
		LOG_INPUT("\033[31m%s\033[0m", buf);
		if(0 == strcmp(buf, "EXIT"))
		{
			return NULL;
		}
	}
	return NULL;
}

void* thread_ipc()
{
	int messType = 0;
	int buffSz = atoi(ini_config_manager_get(cfg, "buffer_size"));
	char taskId[64];
	char recvBuf[buffSz];
	message curMessage;
	init_queue(&taskQueue);
	while(1)
	{
		int recvSize = recv_packet(Node.socket, messType, recvBuf, buffSz, 0);
		switch(messType)
		{
			case TASK:
			{
				memncpy(taskId, recvBuf, recvSize);
				for(int i = 0; i < atoi(ini_config_manager_get(cfg, "max_task_num")); i++)
				{
					if(0 == strcmp(taskId, taskList[i]))
					{
						curMessage.id = i;
					}
				}
				break;
			}
			case DATA:
			{
				curMessage.next = NULL;
				curMessage.message_size = recvSize;
				memncpy(curMessage.data, recvBuf, recvSize);
				pthread_mutex_lock(&queue->lock);
				
				if (taskQueue->tail == NULL) {
					taskQueue->head = curMessage;
					taskQueue->tail = curMessage;
				} else {
					taskQueue->tail->next = curMessage;
					taskQueue->tail = curMessage;
					
				}
				++taskQueue->size;
				break;
			}
			case LAUNCH:
			{
				memncpy(taskId, recvBuf, recvSize);
				char dataToComp[4096];
				dequeue(&taskQueue, dataToComp);
				if(0 == strcmp(taskId, "LinearRegression"))
				{
					//int resSize = LinearRegression(dataToComp);
				}
				if(0 == strcmp(taskId, "LogisticRegression"))
				{
					//int resSize = LogisticRegression(dataToComp);
				}
				if(0 == strcmp(taskId, "SupportVectorMachine"))
				{
					//int resSize = SupportVectorMachine(dataToComp);
				}
				else
				{
					// ...
				}
				// ...
				// int sendSize = send_packet(Node.socket, RES, dataToComp, resSize, 0);
			}
			default:
			{
				break;
			}
		}
	}
}

int connect_manager()
{
	Node.id = ini_config_manager_get(cfg, "id");
	Node.addr = ini_config_manager_get(cfg, "ip");
	Node.port = atoi(ini_config_manager_get(cfg, port));
	Node.socket = create_client(Node.addr, Node.port);	
}

