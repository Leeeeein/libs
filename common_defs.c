#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common_defs.h"

void file_to_str(char* buf, const char* file_name, int* line_count, int* line_length)
{
	*line_count = 0;
    *line_length = 0;
    
    FILE* fp = fopen(file_name, "r");
    if (!fp) {
        perror("fopen error");
        return;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek error");
        fclose(fp);
        return;
    }
    long size = ftell(fp);
    if (size == -1) {
        perror("ftell error");
        fclose(fp);
        return;
    }
    rewind(fp);
    // char* buf = (char*)malloc(size + 1);
    if (!buf) {
        perror("malloc error");
        fclose(fp);
        return;
    }
    size_t read_size = fread(buf, 1, size, fp);
    if (read_size != size) {
        perror("fread error");
        free(buf);
        fclose(fp);
        return;
    }
    buf[size] = '\0';
    
    int buf_len = strlen(buf);
    for (int i = 0; i < buf_len; ++i) {
        if (buf[i] == '\n') {
            ++(*line_count);
            *line_length = (*line_length == 0) ? i : *line_length;
        }
    }

    *line_length = (*line_length == 0) ? buf_len : (*line_length + 1);
    
    fclose(fp);
    return;
}

void write_to_file(const char *array, const char *filePath, int phase)
{
    FILE *file;
    file = fopen(filePath, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fwrite(array, sizeof(char), sizeof(array), file);
    fclose(file);
}

void write_to_file_with_phase(const char *array, const char *file_path, int phase)
{
    char file_name[100];
    sprintf(file_name, "%sphase%d.txt", file_path, phase);
    FILE* file;
    file = fopen(file_name, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fwrite(array, sizeof(char), strlen(array), file);
    fclose(file);
}

void swap_pointers(char **data, char **result)
{
    char *temp = *data;
    *data = *result;
    *result = temp;
}
