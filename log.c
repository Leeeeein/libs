#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "log.h"
#include <pthread.h>

logger_t *logger = NULL;

int log_init()
{
    if (logger != NULL)
    {
        return -1;  // already initialized
    }
    logger = malloc(sizeof(logger_t));
    pthread_mutex_init(&(logger->mutex), NULL);
    logger->level = LOG_LEVEL_INFO;
    logger->output_count = 0;
    
    add_log_output(stdout);
    FILE *log_file = fopen("application.log", "a");
    add_log_output(log_file);
    set_log_level(LOG_LEVEL_DEBUG);
    
    
    return 0;
}

void log_uninit()
{
    if (logger == NULL)
    {
        return;  // already uninitialized
    }
    pthread_mutex_destroy(&logger->mutex);
    int i;
    for (i = 0; i < logger->output_count; i++)
    {
        fclose(logger->outputs[i]);
    }
    free(logger);
    logger = NULL;
}


// Add an output to the logger
void add_log_output(FILE *output)
{
    if (!logger || !output)
    {
        return;
    }
    pthread_mutex_lock(&(logger->mutex));
    logger->outputs[logger->output_count++] = output;
    pthread_mutex_unlock(&(logger->mutex));
}

// Set the log level for the logger
void set_log_level(log_level_t level)
{
    if (!logger)
    {
        return;
    }
    pthread_mutex_lock(&(logger->mutex));
    logger->level = level;
    pthread_mutex_unlock(&(logger->mutex));
}

void log_message(log_level_t level, const char *fmt, ...)
{
    if (!logger)
    {
        return;
    }
    pthread_mutex_lock(&(logger->mutex));
    if (level < logger->level)
    {
    	pthread_mutex_unlock(&(logger->mutex));
        return;
    }

	time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    va_list args;
    va_start(args, fmt);
    // vfprintf(stderr, fmt, args);
    va_end(args);
	
	char level_str[32];
	switch(level)
	{
		case 0: 
		{
			strcpy(level_str, "DEBUG");
			break;
		}
		case 1:
		{
			strcpy(level_str, "INFO");
			break;
		}
		case 2: 
		{
			strcpy(level_str, "WARNING");
			break;
		}
		case 3:
		{
			strcpy(level_str, "ERROR");
			break;
		}
		case 4:
		{
			strcpy(level_str, "INPUT");
			break;
		}
		default:
		{
			strcpy(level_str, "UNKNOWN");
			break;
		}
	}
    for (int i = 0; i < logger->output_count; i++)
    {
        va_start(args, fmt);
        fprintf(logger->outputs[i], "[%s] [%s] ", time_str, level_str);
        vfprintf(logger->outputs[i], fmt, args);
        fprintf(logger->outputs[i], "\n");
        va_end(args);
   }
   pthread_mutex_unlock(&(logger->mutex));
}

