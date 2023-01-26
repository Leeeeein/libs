#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "work.h"
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
static IniConfigManager* cfg;
static message_queue* taskQueue;
static char** taskList;

void work_init()
{
	cfg = ini_config_manager_create("cfg_node.ini");
	ini_config_manager_read(cfg);
	taskList = (char **)malloc(atoi(ini_config_manager_get(cfg, "max_task_num")) * sizeof(char *));
    // 为每个字符串分配内存
    for (int i = 0; i < atoi(ini_config_manager_get(cfg, "max_task_num")); i++) {
        taskList[i] = (char *)malloc((atoi(ini_config_manager_get(cfg, "max_task_id_length"))+ 1) * sizeof(char));
    }
}

void* thread_command()
{
	char buf[64];
	while(1)
	{
		printf("a\n");
		scanf("%s", buf);
		size_t l = sizeof(buf);
		buf[l-1] = buf[l];
		LOG_INPUT("\033[33m%s\033[0m", buf);
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
	taskQueue = (message_queue*)malloc(sizeof(message_queue));
	init_queue(taskQueue);
	while(1)
	{
		int recvSize = recv_packet(Node.socket, &messType, recvBuf, buffSz, 0);
		printf("recvsize: %d\n", recvSize);
		switch(messType)
		{
			case TASK:
			{
				memcpy(taskId, recvBuf, recvSize);
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
				memcpy(curMessage.data, recvBuf, recvSize);
				pthread_mutex_lock(&(taskQueue->lock));
				
				if (taskQueue->tail == NULL) {
					taskQueue->head = &curMessage;
					taskQueue->tail = &curMessage;
				} else {
					taskQueue->tail->next = &curMessage;
					taskQueue->tail = &curMessage;
					
				}
				++taskQueue->size;
				break;
			}
			case LAUNCH:
			{
				memcpy(taskId, recvBuf, recvSize);
				char dataToComp[4096];
				dequeue(taskQueue, dataToComp);
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

void connect_manager()
{
	strcpy(Node.id, ini_config_manager_get(cfg, "id"));
	strcpy(Node.address, ini_config_manager_get(cfg, "ip"));
	Node.port = atoi(ini_config_manager_get(cfg, "port"));
	Node.socket = create_client(Node.address, Node.port);
	printf("id: %s, ip: %s, port: %d, socket: %d \n", Node.id, Node.address, Node.port, Node.socket);	
}

