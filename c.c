#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"

int main(int argc, char *argv[]) {
    // 检查参数
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <IP address> <port>\n", argv[0]);
        return 1;
    }

    // 创建客户端
    int sockfd = create_client(argv[1], atoi(argv[2]));
    if (sockfd < 0) {
        return 1;
    }

    // 循环读取键盘输入并发送数据包
    char buf[1024];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        /*packet *pkt = (packet *)malloc(sizeof(packet));
        pkt->size = strlen(buf);
        pkt->data = malloc(pkt->size);
        buf[pkt->size-1] = '\0';
        memcpy(pkt->data, buf, pkt->size);
        printf("a test line: %s\n", (const char*)pkt->data);
        if (write_packet(sockfd, pkt) < 0) {
            break;
        }
        free(pkt->data);
        free(pkt);
        */
        send_packet(sockfd,0, buf,strlen(buf)+1, 0);
    }
    // 关闭客户端套接字
    close(sockfd);

    return 0;
}
