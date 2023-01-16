#include <stdlib.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "map.h"
#include "distributed_manager.h"
#include "message_queue.h"

static message_queue mq;
static pthread_t pFetch;
static Map map;
static int server_fd;
static char result_cache[4096];

/* 
 *  Section : 服务函数
 *  Purpose: 集中定义rpc的服务端服务函数
 */
 
char* rpc_join_cluster(const char* params) {
	int a, b, cfd;
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
void* thread_command()
{
	char buf[64];
	while(1)
	{
		scanf("%s", buf);
		size_t l = sizeof(buf);
		buf[l-1] = buf[l];
		LOG_INPUT("\033[31m%s\033[0m", buf);
		if(0 == strcmp(buf, "NumOfNodes"))
		{
			LOG_INFO("%d", get_scheduler()->num_nodes);
		}
		else
		{
			LOG_WARNING("\033[35m 输入的命令不存在：%s \n 当前可以使用的命令有：\n NumOfNodes（查看计算节点数量）\033[0m", buf)
		}
	}
}

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
	char request_buf[1024];
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
				sprintf(request_buf + strlen(request_buf), "socket[%d].", fd);
				LOG_INFO("recv ret: %d, message type: %d\n", ret, type);
				if(ret <= 0)
				{
				   close(fd);
				   FD_CLR(fd, &fds);
				   memset(request_buf, 0, sizeof(request_buf));
				   LOG_INFO("客户端退出连接.[fd:%d].\n", fd);
				   distributed_manager_remove_node(fd);
				   continue;
				}
				message_process(type, fd, ret, request_buf);
			}
		}	
	}
	return NULL;	
}

void* thread_fetch(int* num)
{
	int recv_count = 0;
	int size_recv = 0;
	while(1){
		if(mq.size != 0)
		{
			char tmp[2048];
			recv_count += 1;
			// 把tmp中的data拷贝到result_cache中来
			memcpy(result_cache + size_recv, ((message*)tmp)->data, ((message*)tmp)->message_size);
			size_recv += ((message*)tmp)->message_size;
			if(recv_count == *num)
			{	
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
 
// 以raw规则处理收到的消息
void process_raw_message(int socket_fd, int recv_size, char* message_buf)
{
	// 因为在scheduler中每个计算节点是用文件描述符表示的，所以收到数据后，使用文件描述符在map中可以定位这是第几个节点的任务，就可以放回相应的位置
	enqueue(&mq, socket_fd, recv_size, message_buf);
	LOG_DEBUG("queue size: %d\n", mq.size);
}

// 以rpc规则处理收到的消息
void process_rpc_message(int socket_fd, char* request_buf)
{
	int ret = 0;
	printf("get request line : %s\n", request_buf);
	rpc_request_t* request = rpc_deserialize_request(request_buf);
	printf("method name: |%s|, method parameters: |%s|\n", request->method, request->params);
	rpc_response_t* response = rpc_execute_request(socket_fd, request);
	char* response_str = rpc_serialize_response(response);
	printf("response string: |%s| \n", response_str);
	ret = send_packet(socket_fd, RAW, response_str, sizeof(response_str), 0);
	printf("send finish. length:[%d]\n", ret);
}

void process_res_message(int socket_fd, char* request_buf)
{
	
}

// 向分布式管理机添加一个子节点
int distributed_manager_add_node(int node_id)
{
	int ret = scheduler_add_node(node_id);
	return ret;
}

// 从分布式管理机中移除一个子节点
int distributed_manager_remove_node(int node_id)
{
	int ret = scheduler_remove_node(node_id);
	return ret;
}

// 向分布式管理机提交一个任务
int distributed_manager_submit_task(const char* task_id, const char* command, const char* dependencies, int num_dependencies)
{
	int ret = scheduler_submit_task(task_id, command, dependencies, num_dependencies);
	return ret;
}

// 从分布式管理机中撤销一个任务
int distributed_manager_cancel_task(const char* task_id)
{
	int ret = scheduler_cancel_task(task_id);
	return ret;
}

// 获取任务的执行状态
int distributed_manager_get_task_status(const char* task_id);

// 启动指定编号的任务
void distributed_manager_launch_specified_task(const char* task_id, int max_nodes)
{
	int nodes[max_nodes];
	int num_usable = scheduler_get_usable_nodes(nodes, max_nodes);
	map_clean(&map);
	for(int i = 0; i < num_usable; i++)
	{
		int ret = send_packet(nodes[i], LAUNCH, task_id, sizeof(task_id), 0);
		if(ret < 0)
		{
			// LOG_DEBUG
		}
		char tmp[32];
		sprintf(tmp, "%d", nodes[i]);
		insert(&map, tmp, i); // map保存了节点编号（即连接时的fd）和执行顺序的pair，这样就可以知道每个节点执行的是第几部分任务
	}
	pthread_create(&pFetch, NULL, (void*)thread_fetch, (void*)&num_usable);
	pthread_detach((pthread_t)(&pFetch));
	return;	
}

// 设置负载均衡策略
void distributed_manager_set_load_balancing_strategy(const char* strategy);

// 释放分布式管理机资源
void distributed_manager_cleanup();

// logger_t logger;

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}
	
	init(&map);
	init_queue(&mq);
	log_init();
	rpc_init();
	scheduler_init();
	IniConfigManager* cfg = ini_config_manager_create("config.ini");
	if (cfg == NULL) {
    	LOG_DEBUG("创建配置文件失败：cfg == NULL.\n");
    	return -2;
    }
    if (ini_config_manager_read(cfg) != 0) {
     	LOG_DEBUG("读取配置文件失败.\n");
 	 	ini_config_manager_free(cfg);
 	 	return -2;
	}
	int ret = create_server(argv[1], atoi(argv[2]));
	if(ret <= 0)
	{
		//LOG_ERROR(dm.m_log, "监听服务端创建失败. ret:[%d]", ret);
		return -5;
	}
    LOG_INFO("监听服务端创建完成.");
	server_fd = ret;
	
	rpc_publish("join_cluster", rpc_join_cluster);
	rpc_publish("exit_cluster", rpc_exit_cluster);
	
	pthread_t id1, id2;
	pthread_create(&id1, NULL, (void*)thread_client, NULL);
	pthread_create(&id2, NULL, (void*)thread_command, NULL);
    pthread_detach((pthread_t)(&id1));
    pthread_detach((pthread_t)(&id2));
    
    distributed_manager_submit_task("12","12","3",1);
	while(1)
	{
		LOG_INFO("程序运行中：等待接受命令或请求");
		sleep(10);
	}
	log_uninit();
	return 0;
}


