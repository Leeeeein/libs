#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include "distributed_manager.h"
#include "common_defs.h"

void fit(HMM_ARGS* args)
{
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
				distributed_manager_launch_specified_task("hmm_1", 3, dataBuf, args->resultBuf, &enumStatus, &desc);
				break;
			}
			case PHASE1_COMPLETE:
			{
				write_to_file_with_phase(args->resultBuf, args->filePath, 1);
				swap_pointers(&dataBuf, &(args->resultBuf));
				distributed_manager_launch_specified_task("hmm_2", args->model->max_nodes_num, dataBuf, args->resultBuf, &enumStatus, &desc);
				break;
			}
			case PHASE2_COMPLETE:
			{
				write_to_file_with_phase(args->resultBuf, args->filePath, 2);
				swap_pointers(&dataBuf, &(args->resultBuf));
				distributed_manager_launch_specified_task("hmm_3", args->model->max_nodes_num, dataBuf, args->resultBuf, &enumStatus, &desc);
				break;
			}
			case PHASE3_COMPLETE:
			{
				write_to_file_with_phase(args->resultBuf, args->filePath, 3);
				swap_pointers(&dataBuf, &(args->resultBuf));
				distributed_manager_launch_specified_task("hmm_4", args->model->max_nodes_num, dataBuf, args->resultBuf, &enumStatus, &desc);
				break;
			}
			case PHASE4_COMPLETE:
			{
				write_to_file_with_phase(args->resultBuf, args->filePath, 4);
				swap_pointers(&dataBuf, &(args->resultBuf));
				distributed_manager_launch_specified_task("hmm_5", args->model->max_nodes_num, dataBuf, args->resultBuf, &enumStatus, &desc);
				break;
			}
			case PHASE5_COMPLETE:
			{
				write_to_file_with_phase(args->resultBuf, args->filePath, 5);
				swap_pointers(&dataBuf, &(args->resultBuf));
				distributed_manager_launch_specified_task("hmm_6", args->model->max_nodes_num, dataBuf, args->resultBuf, &enumStatus, &desc);
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


int main(int argc, char** argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}
	HmmModel model = {1, NULL, NULL, 0, 0};
	HMM_ARGS args;
	char result[1024*1024];
	launch_server(argv);
	args.model = &model;
	strcpy(args.filePath, "/home/liyinzhe/Workspace/FolderForLibs/inc/data.txt");
	args.resultBuf = result;
	strcpy(args.resultPath, "/home/liyinzhe/Workspace/FolderForLibs/inc");
	
	pthread_t pid;
	pthread_create(&pid, NULL, (void*)fit, (void*)&args);
	pthread_detach((pthread_t)(&pid));
	while(1)
	{
		printf("程序运行中：等待接受命令或请求\n");
		sleep(10);
	}
	log_uninit();
	return 0;
}
