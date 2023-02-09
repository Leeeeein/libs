#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

typedef struct
{
	int n_components;
	int* startprob_prior;
	int* transmat_prior;
	int n_iter;
	int max_nodes_num;
} HmmModel;

typedef enum
{
	BEGIN,
	PHASE1_COMPLETE,
	PHASE2_COMPLETE,
	PHASE3_COMPLETE,
	PHASE4_COMPLETE,
	PHASE5_COMPLETE,
	PHASE6_COMPLETE,
	COMPLETE,
	FAILURE
} HMM_PHASES;

typedef struct
{
	HmmModel* model;
	char filePath[256];
	char* resultBuf;
	char resultPath[256];
} HMM_ARGS;

typedef struct
{
	int data_items_num;
	int single_item_length;
} TASK_DESCRIPTION;

void file_to_str(char* buf, const char* file_name, int* line_count, int* line_length);
void write_to_file(const char *array, const char *filePath, int phase);
void write_to_file_with_phase(const char *array, const char *file_path, int phase);
void swap_pointers(char **data, char **result);
#endif
