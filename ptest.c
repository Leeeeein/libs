#include <stdio.h>
#include "work.h"
#include "log.h"
#include <unistd.h>
#include <pthread.h>

int main() {
    work_init();
    log_init();
    connect_manager();
    // 初始化线程
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_command, NULL);
    pthread_create(&thread2, NULL, thread_ipc, NULL);

    // 等待线程结束
    pthread_detach(thread1);
    pthread_detach(thread2);
    while(1)
    {
    	sleep(10);
    }
    return 0;
}
