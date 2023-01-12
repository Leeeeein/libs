#include <stdlib.h>
#include "distributed_manager.h"
#include <pthread.h>
#include <sys/ioctl.h>

int distributed_manager_init(DM* dm)
{
	return 0;
}

// 服务端的加法服务
char* add(const char* params) {
  int a, b, c;
  sscanf(params, "%d,%d,%d", &a, &b, &c);
  static char result[16];
  sprintf(result, "%d", a + b);
  return result;
}

// 服务端的减法服务
char* subtract(const char* params) {
  int a, b;
  sscanf(params, "%d,%d", &a, &b);
  static char result[16];
  sprintf(result, "%d", a - b);
  return result;
}

// 服务端提供的节点加入集群的服务
char* rpc_join_cluster(const char* params) {
	int a, b, cfd;
	sscanf(params, "%d, %d, %d", &a, &b, &cfd);
	int ret = distributed_manager_add_node(cfd);
	static char result[16];
	sprintf(result, "%d", ret);
	return result;
}

// 服务端提供的节点退出集群的服务
char* rpc_exit_cluster(const char* params) {
	int a,b,cfd;
	sscanf(params, "%d, %d, %d", &a, &b, &cfd);
	int ret = distributed_manager_remove_node(cfd);
	static char result[16];
	sprintf(result, "%d", ret);
	return result;
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

void* thread_client(DM* dm)
{	
	int sockfd = dm->socket_fd;
	char request_buf[1024];
	// char response_buf[1024];

	fd_set fds;
	FD_SET(sockfd, &fds);
	int max_fd = sockfd;
	// 循环处理客户端的请求
	while (1) {
		fd_set read_fds = fds;
		int ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
		if (ret <= 0) {
			fprintf(stderr, "select error\n");
			break;
		}
		if (FD_ISSET(sockfd, &read_fds)) {
			struct sockaddr_in client_addr;
			socklen_t addrlen = sizeof(client_addr);
			int client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
			if (client_sock < 0) {
				fprintf(stderr, "accept error\n");
				continue;
			}
			FD_SET(client_sock, &fds);
			if (client_sock > max_fd) {
				max_fd = client_sock;
			}
		}
		for (int fd = 0; fd <= max_fd; ++fd) {
			if (FD_ISSET(fd, &read_fds) && fd != sockfd) {
				// 从客户端接收请求
				int type = 0;
				ret = recv_packet(fd, &type, request_buf, sizeof(request_buf), 0);

				int len = strlen(request_buf);
				sprintf(request_buf + len, "socket[%d].", fd);

				printf("recv ret: %d, message type: %d\n", ret, type);
				if(ret <= 0)
				{
				   close(fd);
				   FD_CLR(fd, &fds);
				   memset(request_buf, 0, sizeof(request_buf));
				   printf("Client disconnected, fd closed.\n");
				   distributed_manager_remove_node(fd);
				   continue;
				}
				switch(type)
				{
					case UNDEFINE:
					{
						LOG_DEBUG("message not defined.\n");
						break;
					}
					case RAW:
					{
						process_raw_message(fd, request_buf);
						break;
					}
					case RPC:
					{
						process_rpc_message(fd, request_buf);
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
		}	
	}
	return NULL;	
}

// 以raw规则处理收到的消息
void process_raw_message(int socket_fd, char* request_buf)
{

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
	ret = send_packet(socket_fd, COMMON, response_str, sizeof(response_str), 0);
	printf("send finish. length:[%d]\n", ret);
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
void distributed_manager_cancel_task(DM* dm, const char* task_id);

// 获取任务的执行状态
int distributed_manager_get_task_status(DM* dm, const char* task_id);

// 启动指定编号的任务
void distributed_manager_launch_specified_task(const char* task_id, int max_nodes)
{
	int nodes[max_nodes];
	int num_usable = scheduler_get_usable_nodes(nodes, max_nodes);
	for(int i = 0; i < num_usable; i++)
	{
		int ret = send_packet(nodes[i], COMMON, task_id, sizeof(task_id), 0);
	}
	return;
	
}

// 设置负载均衡策略
void distributed_manager_set_load_balancing_strategy(DM* dm, const char* strategy);

// 释放分布式管理机资源
void distributed_manager_cleanup(DM* dm);

// logger_t logger;

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
		return 1;
	}
	
	DM dm;
	// dm.m_log = get_logger();
	distributed_manager_init(&dm);
	log_init();
	scheduler_init();
	dm.m_cfg = ini_config_manager_create("config.ini");
	if (dm.m_cfg == NULL) {
    	//LOG_DEBUG(dm.m_log, "Error creating INI config manager.\n");
    	return -2;
    }
     
    if (ini_config_manager_read(dm.m_cfg) != 0) {
     	//LOG_DEBUG(dm.m_log, "Error reading INI config file.\n");
 	 	ini_config_manager_free(dm.m_cfg);
 	 	return -2;
	}
	for (int i = 0; i < dm.m_cfg->num_items; i++) {
	     //LOG_DEBUG(dm.m_log, "%s = %s\n", (char*)dm.m_cfg->items[i].key, (char*)dm.m_cfg->items[i].value);
	}
	
	int ret = create_server(argv[1], atoi(argv[2]));
	if(ret <= 0)
	{
		//LOG_ERROR(dm.m_log, "监听服务端创建失败. ret:[%d]", ret);
		return -5;
	}
    LOG_INFO("监听服务端创建完成.");
	dm.socket_fd = ret;
	rpc_init();
	rpc_publish("add", add);
	rpc_publish("subtract", subtract);
	rpc_publish("join_cluster", rpc_join_cluster);
	rpc_publish("exit_cluster", rpc_exit_cluster);
	pthread_t id1, id2;
	pthread_create(&id1, NULL, (void*)thread_client, &dm);
	pthread_create(&id2, NULL, (void*)thread_command, NULL);
    pthread_detach((pthread_t)(&id1));
	while(1)
	{
		LOG_INFO("程序运行中：等待接受命令或请求");
		sleep(10);
	}
	//close(dm.socket_fd);
	log_uninit();
	return 0;
}


