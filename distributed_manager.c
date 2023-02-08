#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <string.h>
#include <math.h>

#include "map.h"
#include "distributed_manager.h"
#include "message_queue.h"

static message_queue mq;
static pthread_t pFetch;
static Map map;
static int server_fd;


char* rpc_join_cluster(const char* params) {
	int a, b, cfd;
	printf("outter params: %s, length: %d\n", params, strlen(params));
	sscanf(params, "%d, %d, %d", &a, &b, &cfd);
	int ret = distributed_manager_add_node(cfd);
	static char result[16];
	sprintf(result, "%d", ret);
	return result;
}

char* rpc_exit_cluster(const char* params) {
	int a,b,cfd;
	sscanf(params, "%d, %d, %d", &a, &b, &cfd);
	int ret = distributed_manager_remove_node(cfd);
	static char result[16];
	sprintf(result, "%d", ret);
	return result;
}

/* 
 *  Section : 线程函数
 *  Purpose: 集中定义文件中的线程函数
 */


void message_process(int type, int fd, int recv_size, char* request_buf)
{
	switch(type)
	{
		case UNDEFINE:
		{
			LOG_DEBUG("message not defined.\n");
			break;
		}
		case RAW:
		{
			process_raw_message(fd, recv_size, request_buf);
			break;
		}
		case RPC:
		{
			process_rpc_message(fd, request_buf);
			break;
		}
		case RES:
		{
			process_res_message(fd, request_buf);
			break;
		}
		case COMMON:
		{
			//todo
			//
			break;
		}
		default:
		{
			break;
		}
	}
}

void* thread_client()
{	
	int sockfd = server_fd;
	int max_fd = sockfd;
	fd_set fds;
	FD_SET(sockfd, &fds);
	char request_buf[1024*1024];
	while (1) {
		fd_set read_fds = fds;
		int ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
		if (ret <= 0) {
			break;
		}
		if(FD_ISSET(sockfd, &read_fds) && -1 == create_connect(sockfd, &fds, &max_fd)){
			continue;
		}
		for (int fd = 0; fd <= max_fd; ++fd) {
			if (FD_ISSET(fd, &read_fds) && fd != sockfd) {
				// 从客户端接收请求
				int type = 0;
				ret = recv_packet(fd, &type, request_buf, sizeof(request_buf), 0);
				if(ret <= 0)
				{
				   close(fd);
				   FD_CLR(fd, &fds);
				   memset(request_buf, 0, sizeof(request_buf));
				   LOG_INFO("客户端退出连接.[fd:%d]", fd);
				   distributed_manager_remove_node(fd);
				   int nodes[10];
				   int num_usable = scheduler_get_usable_nodes(nodes, 10);
				   LOG_INFO("当前可用节点数量：%d %d\n", num_usable, scheduler_get_nodes_num());
				   continue;
				}
				message_process(type, fd, ret, request_buf);
			}
		}	
	}
	return NULL;	
}

void* thread_fetch(thread_fetch_paras* args)//int* num, char* result_cache, HMM_PHASES* enumStatus)
{
	int recv_count = 0;
	int size_recv = 0;
	while(1){
		if(mq.size != 0)
		{
			char tmp[2048];
			dequeue(&mq, tmp);
			recv_count += 1;
			// 把tmp中的data拷贝到result_cache中来,
			// 这个地方还有一个处理，根据节点的编号把结果放在相应的位置，而不是直接从前往后放
			memcpy((char*)args->result_cache + size_recv, ((message*)tmp)->data, ((message*)tmp)->message_size);
			size_recv += ((message*)tmp)->message_size;
			if(recv_count == args->num_nodes)
			{	
				args->enumStatus += 1;
				break;
			}
		}
	}
	return NULL;
}

/* 
 *  Section : 数据处理函数
 *  Purpose: 对收到的数据包根据类别进行处理
 */
 
void process_raw_message(int socket_fd, int recv_size, char* message_buf)
{
	// 因为在scheduler中每个计算节点是用文件描述符表示的，所以收到数据后，使用文件描述符在map中可以定位这是第几个节点的任务，就可以放回相应的位置
	enqueue(&mq, socket_fd, recv_size, message_buf);
	LOG_DEBUG("queue size: %d\n", mq.size);
}

void process_rpc_message(int socket_fd, char* request_buf)
{
	sprintf(request_buf+strlen(request_buf), "socket[%d].", socket_fd);
	printf("get request line : %s\n", request_buf);
	rpc_request_t* request = rpc_deserialize_request(request_buf);
	printf("method name: |%s|, method parameters: |%s|\n", request->method, request->params);
	rpc_response_t* response = rpc_execute_request(socket_fd, request);
	char* response_str = rpc_serialize_response(response);
	send_packet(socket_fd, RAW, response_str, sizeof(response_str), 0);
}

void process_res_message(int socket_fd, char* request_buf)
{
	LOG_DEBUG("\n%s\n\n", request_buf);
}

scheduler* distributed_manager_get_scheduler()
{
	return get_scheduler();
}

int distributed_manager_add_node(int node_id)
{
	int ret = scheduler_add_node(node_id);
	return ret;
}

int distributed_manager_remove_node(int node_id)
{
	int ret = scheduler_remove_node(node_id);
	return ret;
}

int distributed_manager_submit_task(const char* task_id, const char* command, const char* dependencies, int num_dependencies)
{
	int ret = scheduler_submit_task(task_id, command, dependencies, num_dependencies);
	return ret;
}

int distributed_manager_cancel_task(const char* task_id)
{
	int ret = scheduler_cancel_task(task_id);
	return ret;
}

int distributed_manager_get_task_status(const char* task_id);

int distributed_manager_launch_specified_task(const char* task_id, int max_nodes, char* file, char* result, HMM_PHASES* enumStatus, TASK_DESCRIPTION* desc)
{
	int nodes[max_nodes];
	int num_usable = scheduler_get_usable_nodes(nodes, max_nodes);
	if(num_usable <= 0)
	{
		// printf("usable nodes are not exist.\n");
		return -1;
	}
	map_clean(&map);
	int part_items_num = desc->data_items_num / num_usable;
	for(int i = 0; i < num_usable; i++)
	{
		int ret = send_packet(nodes[i], LAUNCH, task_id, sizeof(task_id), 0);
		if(i != num_usable-1)
			ret = send_packet(nodes[i], DATA, &file[i*part_items_num*desc->single_item_length], part_items_num*desc->single_item_length, 0);
		else
			ret = send_packet(nodes[i], DATA, &file[i*part_items_num*desc->single_item_length], desc->data_items_num*desc->single_item_length - (num_usable-1)*part_items_num*desc->single_item_length, 0);
		printf("items_num: %d, num_usable: %d, single_item_length: %d, big_ass: %d, ret: %d\n",\
		 desc->data_items_num, num_usable, desc->single_item_length,\
		 desc->data_items_num*desc->single_item_length - (num_usable-1)*part_items_num*desc->single_item_length, ret);
		if(ret < 0)
		{
			return -1;
		}
		char tmp[32];
		sprintf(tmp, "%d", nodes[i]);
		insert(&map, tmp, i); // map保存了节点编号（即连接时的fd）和执行顺序的pair，这样就可以知道每个节点执行的是第几部分任务
	}
    thread_fetch_paras args = {num_usable, result, enumStatus};
	pthread_create(&pFetch, NULL, (void*)thread_fetch, (void*)&args);
	pthread_detach((pthread_t)(&pFetch));
	return 0;	
}

void distributed_manager_set_load_balancing_strategy(const char* strategy);

void distributed_manager_cleanup();

// logger_t logger;


/*int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}*/
	
void launch_server(char** argv)
{   
	init(&map);
	init_queue(&mq);
	log_init();
	rpc_init();
	scheduler_init();
	IniConfigManager* cfg = ini_config_manager_create("config.ini");
	if (cfg == NULL) {
    	LOG_DEBUG("创建配置文件失败：cfg == NULL.\n");
    	return;
    }
    if (ini_config_manager_read(cfg) != 0) {
     	LOG_DEBUG("读取配置文件失败.\n");
 	 	ini_config_manager_free(cfg);
 	 	return;
	}
	int ret = create_server(argv[1], atoi(argv[2]));
	if(ret <= 0)
	{
		//LOG_ERROR(dm.m_log, "监听服务端创建失败. ret:[%d]", ret);
		return;
	}
    LOG_INFO("监听服务端创建完成.");
	server_fd = ret;
	
	rpc_publish("join_cluster", rpc_join_cluster);
	rpc_publish("exit_cluster", rpc_exit_cluster);
	
}


