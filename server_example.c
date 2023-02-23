#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include "distributed_manager.h"
#include "common_defs.h"

void fit(HMM_ARGS* args)
{
	LOG_INFO("Begin to exec");
	int isRunning = 0;
	int items_num, item_length;
	char* dataBuf = (char*)malloc(1024*1024);
	file_to_str(dataBuf, args->filePath, &items_num, &item_length);
	HMM_PHASES enumStatus = BEGIN;
	TASK_DESCRIPTION desc;
	desc.data_items_num = items_num;
	desc.single_item_length = item_length;
	while(1)
	{
		switch(enumStatus)
		{
			case BEGIN:
			{	
				if(0 == isRunning)
				{
					int ret = distributed_manager_launch_specified_task("hmm_1", 8, dataBuf, &desc);
					LOG_INFO("stage 1, retVal is: %d", ret);
					if(0 == ret)
					{
						isRunning = 1;
					}
					else
					{
						enumStatus = FAILURE;
					}	
				}
				break;
			}
			case PHASE1_COMPLETE:
			{
				isRunning = 0;
				write_to_file_with_phase(args->resultBuf, args->filePath, 1);
				swap_pointers(&dataBuf, &(args->resultBuf));
				if(0 == isRunning)
				{
					LOG_INFO("stage 2");
					if(0 == distributed_manager_launch_specified_task("hmm_2", args->model->max_nodes_num, dataBuf, &desc))
					{
						isRunning = 1;
					}
				}
				break;
			}
			case PHASE2_COMPLETE:
			{
				isRunning = 0;
				write_to_file_with_phase(args->resultBuf, args->filePath, 2);
				swap_pointers(&dataBuf, &(args->resultBuf));
				if(0 == isRunning)
				{
					LOG_INFO("stage 3");
					if(0 == distributed_manager_launch_specified_task("hmm_3", args->model->max_nodes_num, dataBuf, &desc))
					{
						isRunning = 1;
					}
				}
				break;
			}
			case PHASE3_COMPLETE:
			{
				isRunning = 0;
				write_to_file_with_phase(args->resultBuf, args->filePath, 3);
				swap_pointers(&dataBuf, &(args->resultBuf));
				if(0 == isRunning)
				{
					LOG_INFO("stage 4");
					if(0 == distributed_manager_launch_specified_task("hmm_4", args->model->max_nodes_num, dataBuf, &desc))
					{
						isRunning = 1;
					}
				}
				break;
			}
			case PHASE4_COMPLETE:
			{
				isRunning = 0;
				write_to_file_with_phase(args->resultBuf, args->filePath, 4);
				swap_pointers(&dataBuf, &(args->resultBuf));
				if(0 == isRunning)
				{
					LOG_INFO("stage 5");
					if(0 == distributed_manager_launch_specified_task("hmm_5", args->model->max_nodes_num, dataBuf, &desc))
					{
						isRunning = 1;
					}
				}
				break;
			}
			case PHASE5_COMPLETE:
			{
				isRunning = 0;
				write_to_file_with_phase(args->resultBuf, args->filePath, 5);
				swap_pointers(&dataBuf, &(args->resultBuf));
				if(0 == isRunning)
				{
					LOG_INFO("stage 6");
					if(0 == distributed_manager_launch_specified_task("hmm_6", args->model->max_nodes_num, dataBuf, &desc))
					{
						isRunning = 1;
					}
				}
				break;
			}
			case PHASE6_COMPLETE:
			{
				// 在这个地方， resultBuf收到的就是最终的计算结果
				write_to_file_with_phase(args->resultBuf, args->filePath, 6);
				enumStatus = COMPLETE;
				break;
			}
			default:
			{
				break;
			}
		}
		if(enumStatus == COMPLETE)
			break;
	}
	return;
}

void* thread_command()
{
	char buf[64];
	while(1)
	{
		scanf("%s", buf);
		size_t l = sizeof(buf);
		buf[l-1] = buf[l];
		LOG_INPUT("\033[31m%s\033[0m", buf);
		if(0 == strcmp(buf, "cmd1"))
		{
			LOG_INFO("%d", distributed_manager_get_scheduler()->num_nodes);
		}
		else if(0 == strcmp(buf, "cmd2"))
		{
			HmmModel model = {1, NULL, NULL, 0, 0};
			HMM_ARGS args;
			char result[1024*1024];
			args.model = &model;
			strcpy(args.filePath, "./data.txt");
			args.resultBuf = result;
			strcpy(args.resultPath, "./result/");
			
			pthread_t pid;
			pthread_create(&pid, NULL, (void*)fit, (void*)&args);
			pthread_detach((pthread_t)(&pid));
		}
		else
		{
			LOG_WARNING("\033[35m 输入的命令不存在：%s \n 当前可以使用的命令有：\n NumOfNodes（查看计算节点数量）\033[0m", buf)
		}
	}
}
int main(int argc, char** argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}
	launch_server(argv);
	
	pthread_t id1;
	pthread_create(&id1, NULL, (void*)thread_client, NULL);
    pthread_detach((pthread_t)(&id1));
    
    
	pthread_t pid2;
	pthread_create(&pid2, NULL, (void*)thread_command, NULL);
    pthread_detach((pthread_t)(&pid2));
	while(1)
	{
		printf("程序运行中：等待接受命令或请求\n");
		sleep(5);
	}
	log_uninit();
	return 0;
}
