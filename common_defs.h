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
} HMM_PHASES;

typedef struct
{
	int data_items_num;
	int single_item_length;
} TASK_DESCRIPTION;
