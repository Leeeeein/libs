# 分布式模型的通用代码

这个代码包含了以下功能：
- 计算节点管理
- 多阶段任务分发
- 任务汇总
- 通过 RPC 进行加入集群和退出集群操作
- 日志管理
- 配置文件管理
- 消息队列、字典、有序数组等类 C++ 的数据结构

## 编译
```make```

## 启动服务端实例
```server_example.c <ip> <port>```

## 启动计算节点
```worker_example.c <ip> <port>```

## 服务端可以通过向控制台发送命令来启动任务。现在仅设置了两个命令：
- `cmd1`：查看已连接的节点数量
- `cmd2`：启动测试任务

`data.txt` 包含了生成的测试数据，计算节点收到数据后会直接发回，服务端收到后汇总，完成测试的流程。

通过 `config.ini` 配置服务端，通过 `cfg_node.ini` 配置计算节点。目前大多数参数仍写在代码中，后面会整理到配置文件中。

运行时日志被保存在 `application.log` 中。
目录结构：
```
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
```

