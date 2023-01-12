#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umap.h"

void init(Map* map) {
  map->capacity = INITIAL_CAPACITY;
  map->size = 0;
  map->entries = malloc(sizeof(MapEntry) * map->capacity);
}

void insert(Map* map, const char* key, int value) {
  // Check if the key already exists in the map
  for (int i = 0; i < map->size; i++) {
    if (strcmp(map->entries[i].key, key) == 0) {
      // Update the value
      map->entries[i].value = value;
      return;
    }
  }

  // Check if the map needs to be resized
  if (map->size == map->capacity) {
    map->capacity *= 2;
    map->entries = realloc(map->entries, sizeof(MapEntry) * map->capacity);
  }

  // Insert the new entry
  map->entries[map->size].key = strdup(key);
  map->entries[map->size].value = value;
  map->size++;
}

int get(Map* map, const char* key) {
  for (int i = 0; i < map->size; i++) {
    if (strcmp(map->entries[i].key, key) == 0) {
      return map->entries[i].value;
    }
  }
  return 0;
}

void free_map(Map* map) {
  for (int i = 0; i < map->size; i++) {
    free(map->entries[i].key);
  }
  free(map->entries);
}

MapIterator get_iterator(Map* map) {
  MapIterator iterator;
  iterator.map = map;
  iterator.index = 0;
  return iterator;
}

int has_next(MapIterator* iterator) {
  return iterator->index < iterator->map->size;
}

MapEntry next(MapIterator* iterator) {
  MapEntry entry = iterator->map->entries[iterator->index];
  iterator->index++;
  return entry;
}
