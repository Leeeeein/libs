#ifndef UVECTOR_H
#define UVECTOR_H

#define INITIAL_CAPACITY 4

typedef struct {
  int *array;
  int capacity;
  int size;
} Vector;

typedef struct {
  Vector *vector;
  int index;
} VectorIterator;

void init(Vector *vector);
void append(Vector *vector, int value);
VectorIterator get_iterator(Vector *vector);
int has_next(VectorIterator *iterator);
int next(VectorIterator *iterator);
int erase(Vector *vector, VectorIterator *iterator);

#endif // UVECTOR_H
