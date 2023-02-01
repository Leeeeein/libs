#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "distributed_manager.h"
#include "common_defs.h"

void write_to_file(const char *array, const char *filePath, int phase)
{
    FILE *file;
    file = fopen(file_path, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fwrite(array, sizeof(char), sizeof(array), file);
    fclose(file);
}

void write_to_file_with_phase(const char *array, const char *file_path, int phase)
{
    char file_name[100];
    sprintf(file_name, "%sphase%d.txt", file_path, phase);
    FILE *file;
    file = fopen(file_name, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fwrite(array, sizeof(char), strlen(array), file);
    fclose(file);
}

void swap_pointers(char **data, char **result)
{
    char *temp = *data;
    *data = *result;
    *result = temp;
}

void get_task_description(char** dataBuf, int* items_num, int* item_length)
{
    *items_num = 0;
    while (dataBuf[*items_num]) {
        *item_length = max(*item_length, (int)strlen(dataBuf[*items_num]));
        (*items_num)++;
    }
}

int fit(HmmModel* model, char** dataBuf, char** resultBuf, const char* filePath)
{
	HMM_PHASES enumStatus = BEGIN;
	int items_num = 0, item_length = 0;
	get_task_description(dataBuf, &items_num, &item_length);
	TASK_DESCRIPTION desc;
	desc.data_items_num = items_num;
	desc.single_item_length = item_length;
	while(1)
	{
		switch(enumStatus)
		{
			case BEGIN:
			{
				distributed_manager_launch_specified_task("hmm_1", model.max_nodes_num, dataBuf, resultBuf, enumStatus, &desc);
				break;
			}
			case PHASE1_COMPLETE:
			{
				write_to_file_with_phase(resultBuf, filePath, 1);
				swap_pointers(&dataBuf, &resultBuf);
				distributed_manager_launch_specified_task("hmm_2", model.max_nodes_num, dataBuf, resultBuf, enumStatus, &desc);
				break;
			}
			case PHASE2_COMPLETE:
			{
				write_to_file_with_phase(resultBuf, filePath, 2);
				swap_pointers(&dataBuf, &resultBuf);
				distributed_manager_launch_specified_task("hmm_3", model.max_nodes_num, dataBuf, resultBuf, enumStatus, &desc);
				break;
			}
			case PHASE3_COMPLETE:
			{
				write_to_file_with_phase(resultBuf, filePath, 3);
				swap_pointers(&dataBuf, &resultBuf);
				distributed_manager_launch_specified_task("hmm_4", model.max_nodes_num, dataBuf, resultBuf, enumStatus, &desc);
				break;
			}
			case PHASE4_COMPLETE:
			{
				write_to_file_with_phase(resultBuf, filePath, 4);
				swap_pointers(&dataBuf, &resultBuf);
				distributed_manager_launch_specified_task("hmm_5", model.max_nodes_num, dataBuf, resultBuf, enumStatus, &desc);
				break;
			}
			case PHASE5_COMPLETE:
			{
				write_to_file_with_phase(resultBuf, filePath, 5);
				swap_pointers(&dataBuf, &resultBuf);
				distributed_manager_launch_specified_task("hmm_6", model.max_nodes_num, dataBuf, resultBuf, enumStatus, &desc);
				break;
			}
			case PHASE6_COMPLETE:
			{
				// 在这个地方， resultBuf收到的就是最终的计算结果
				write_to_file_with_phase(resultBuf, filePath, 6);
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
}


int main(int argc, char** argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}
	launch_server(argv);
	return 0;
}
