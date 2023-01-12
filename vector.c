#include <stdio.h>
#include <stdlib.h>
#include "uvector.h"

void init(Vector *vector) {
  vector->capacity = INITIAL_CAPACITY;
  vector->size = 0;
  vector->array = malloc(sizeof(int) * vector->capacity);
}

void append(Vector *vector, int value) {
  if (vector->size == vector->capacity) {
    vector->capacity *= 2;
    vector->array = realloc(vector->array, sizeof(int) * vector->capacity);
  }

  vector->array[vector->size] = value;
  vector->size++;
}

VectorIterator get_iterator(Vector *vector) {
  VectorIterator iterator;
  iterator.vector = vector;
  iterator.index = 0;
  return iterator;
}

int has_next(VectorIterator *iterator) {
  return iterator->index < iterator->vector->size;
}

int next(VectorIterator *iterator) {
  int value = iterator->vector->array[iterator->index];
  iterator->index++;
  return value;
}

int erase(Vector *vector, VectorIterator *iterator) {
  // Check if iterator is valid
  if (iterator->index < 0 || iterator->index >= vector->size) {
    return 1; // Invalid iterator
  }

  // Shift elements to the left to remove the element at the iterator's index
  for (int i = iterator->index; i < vector->size - 1; i++) {
    vector->array[i] = vector->array[i + 1];
  }
  vector->size--;

  return 0; // Success
}
