#ifndef USTRING_H
#define USTRING_H

typedef struct {
  char *array;
  int capacity;
  int size;
} String;

void init(String *str);
void append(String *str, char c);
void clear(String *str);
int size(const String *str);
int capacity(const String *str);
int empty(const String *str);
void reserve(String *str, int new_capacity);
void shrink_to_fit(String *str);
char *c_str(const String *str);
void assign(String *str, const char *c_str);
int compare(const String *str, const char *c_str);
void insert(String *str, int index, char c);
void erase(String *str, int index);
void push_back(String *str, char c);
void pop_back(String *str);
char at(const String *str, int index);
void replace(String *str, int index, char c);

#endif
