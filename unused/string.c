#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

typedef struct {
  char *array;
  int capacity;
  int size;
} String;

void init(String *str) {
  str->capacity = INITIAL_CAPACITY;
  str->size = 0;
  str->array = malloc(sizeof(char) * str->capacity);
}

void append(String *str, char c) {
  if (str->size == str->capacity) {
    str->capacity *= 2;
    str->array = realloc(str->array, sizeof(char) * str->capacity);
  }

  str->array[str->size] = c;
  str->size++;
}

void clear(String *str) {
  str->size = 0;
}

int size(const String *str) {
  return str->size;
}

int capacity(const String *str) {
  return str->capacity;
}

int empty(const String *str) {
  return str->size == 0;
}

void reserve(String *str, int new_capacity) {
  if (new_capacity > str->capacity) {
    str->capacity = new_capacity;
    str->array = realloc(str->array, sizeof(char) * str->capacity);
  }
}

void shrink_to_fit(String *str) {
  str->capacity = str->size;
  str->array = realloc(str->array, sizeof(char) * str->capacity);
}

char *c_str(const String *str) {
  char *c_str = malloc(sizeof(char) * (str->size + 1));
  strcpy(c_str, str->array);
  c_str[str->size] = '\0';
  return c_str;
}

void assign(String *str, const char *c_str) {
  int c_str_len = strlen(c_str);
  if (c_str_len > str->capacity) {
    str->capacity = c_str_len;
    str->array = realloc(str->array, sizeof(char) * str->capacity);
  }
  str->size = c_str_len;
  strcpy(str->array, c_str);
}

int compare(const String *str, const char *c_str) {
  return strcmp(str->array, c_str);
}

void insert(String *str, int index, char c) {
if (str->size == str->capacity) {
str->capacity *= 2;
str->array = realloc(str->array, sizeof(char) * str->capacity);
}

for (int i = str->size; i > index; i--) {
str->array[i] = str->array[i - 1];
}
str->array[index] = c;
str->size++;
}

void erase(String *str, int index) {
for (int i = index; i < str->size - 1; i++) {
str->array[i] = str->array[i + 1];
}
str->size--;
}

void push_back(String *str, char c) {
append(str, c);
}

void pop_back(String *str) {
str->size--;
}

char at(const String *str, int index) {
return str->array[index];
}

void replace(String *str, int index, char c) {
str->array[index] = c;
}
