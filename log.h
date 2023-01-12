#include <stdio.h>

#ifndef LOG_H
#define LOG_H

#define LOG_DEBUG(fmt, ...) log_message(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__);
#define LOG_WARNING(fmt, ...) log_message(LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__);
#define LOG_INFO(fmt, ...) log_message(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...) log_message(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__);
#define LOG_UNKNOWN(fmt, ...) log_message(LOG_LEVEL_UNKNOWN, fmt, ##__VA_ARGS__);
#define LOG_INPUT(fmt, ...) log_message(LOG_LEVEL_INPUT, fmt, ##__VA_ARGS__);

typedef enum {
    LOG_LEVEL_DEBUG=0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INPUT
} log_level_t;

typedef struct {
	log_level_t level;
	FILE *outputs[10];
	int output_count;
} logger_t;

int log_init();

void log_uninit();

void add_log_output(FILE *output);

void set_log_level(log_level_t level);

void log_message(log_level_t level, const char *fmt, ...);

#endif // LOG_H
