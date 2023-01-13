#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#define MAX_MESSAGE_LENGTH 256

typedef struct message_queue message_queue;
typedef struct message message;
struct message {
    char data[MAX_MESSAGE_LENGTH];
    struct message *next;
};

typedef struct message_queue {
    struct message *head;
    struct message *tail;
    size_t size;
    pthread_mutex_t lock;
} message_queue;

void init_queue(struct message_queue *queue);

int enqueue(struct message_queue *queue, char *data);

int dequeue(struct message_queue *queue, char *data);

#endif
