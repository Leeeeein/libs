#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_MESSAGE_LENGTH 256

typedef struct message {
    char data[MAX_MESSAGE_LENGTH];
    struct message *next;
} message;

typedef struct message_queue {
    struct message *head;
    struct message *tail;
    size_t size;
    pthread_mutex_t lock;
} message_queue;

void init_queue(struct message_queue *queue) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    pthread_mutex_init(&queue->lock, NULL);
}

int enqueue(struct message_queue *queue, char *data) {
    struct message *new_message = (struct message*)malloc(sizeof(struct message));
    if (new_message == NULL) {
        return -1;
    }

    strncpy(new_message->data, data, MAX_MESSAGE_LENGTH);
    new_message->next = NULL;

    pthread_mutex_lock(&queue->lock);
    if (queue->tail == NULL) {
        queue->head = new_message;
        queue->tail = new_message;
    } else {
        queue->tail->next = new_message;
        queue->tail = new_message;
    }
    ++queue->size;
    pthread_mutex_unlock(&queue->lock);

    return 0;
}

int dequeue(struct message_queue *queue, char *data) {
    pthread_mutex_lock(&queue->lock);
    if (queue->head == NULL) {
        pthread_mutex_unlock(&queue->lock);
        return -1;
    }

    struct message *temp = queue->head;
    strncpy(data, temp->data, MAX_MESSAGE_LENGTH);

    queue->head = temp->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    free(temp);
    --queue->size;
    pthread_mutex_unlock(&queue->lock);
    return 0;
}
