#include "node.h"
#include <string.h>
#include <stdlib.h>

struct node {
    char id[32];
    int sockfd;
};

static struct node* node = NULL;

void node_init(const char* node_id, int sockfd) {
    if (node != NULL) {
        return;  // already initialized
    }

    node = malloc(sizeof(struct node));
    strncpy(node->id, node_id, sizeof(node->id));
    node->sockfd = sockfd;
}

int node_report_task_status(const char* task_id, int status) {
    if (node == NULL) {
        return -1;  // node not initialized
    }
    if (send(node->sockfd, &status, sizeof(int), 0) < 0) {
        perror("send");
        return -2;
    }
    int task_id_len = strlen(task_id) + 1;
    if (send(node->sockfd, &task_id_len, sizeof(int), 0) < 0) {
        perror("send");
        return -3;
    }
    if (send(node->sockfd, task_id, task_id_len, 0) < 0) {
        perror("send");
        return -4;
    }

    return 0;
}

int node_get_next_task(char** task_id, char** command) {
    if (node == NULL) {
        return -1;  // node not initialized
    }
    int task_id_len;
    if (recv(node->sockfd, &task_id_len, sizeof(int), 0) < 0) {
        perror("recv");
        return -2;
    }
    if (task_id_len == 0) {
        return 1;  // no more tasks
    }
    *task_id = malloc(task_id_len);
    if (recv(node->sockfd, *task_id, task_id_len, 0) < 0) {
        perror("recv");
        return -3;
    }
    int command_len;
    if (recv(node->sockfd, &command_len, sizeof(int), 0) < 0) {
        perror("recv");
        return -4;
    }
    *command = malloc(command_len);
    if (recv(node->sockfd, *command, command_len, 0) < 0) {
        perror("recv");
        return -5;
    }

    return 0;
}

int node_cleanup() {
    if (node == NULL) {
        return -1;  // node not initialized
    }
    close(node->sockfd);
    free(node);
    node = NULL;
    return 0;
}
