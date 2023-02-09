#分布式模型的通用代码，包括计算节点管理、多阶段任务分发、任务汇总的功能，使用rpc进行加入集群和推出集群的操作，实现了日志管理、配置文件管理，以及消息队列、字典、有序数组等类C++的数据结构。
##
#编译：
''' make '''
#启动服务端实例:
'''server_example.c <ip> <port> '''
#启动计算节点:
'''worker_example.c <ip> <port> '''

#服务端通过向控制台发送命令来启动任务，现在只设置了两个指令分别是{cmd1：查看连接的节点数量， cmd2：启动测试任务}

#data.txt中包含了生成的测试数据，计算节点收到数据后会直接发回，服务端收到并汇总后完成测试的流程

#通过config.ini配置服务端，通过cfg_node.ini配置计算节点，目前大多数参数还是写死在代码中，后面会整理到配置文件中

#在application.log 保存运行时日志

├── application.log
├── cfg_node.ini
├── common_defs.c
├── common_defs.h
├── config.c
├── config.h
├── config.ini
├── data.txt
├── distributed_manager.c
├── distributed_manager.h
├── distributed_worker.h
├── examples
│   └── test_py.py
├── libdisMana.so
├── log.c
├── log.h
├── makefile
├── map.c
├── map.h
├── message_queue.c
├── message_queue.h
├── mtest.c
├── network.c
├── network.h
├── ptest.c
├── push.sh
├── README.MD
├── result
├── rpc.c
├── rpc.h
├── scheduler.c
├── scheduler.h
├── server_example
├── server_example.c
├── unused
│   ├── string.c
│   ├── string.h
│   ├── vector.c
│   └── vector.h
├── work.c
├── worker_example
├── worker_example.c
└── work.h

