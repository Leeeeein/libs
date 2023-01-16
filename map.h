#ifndef UMAP_H
#define UMAP_H

#define INITIAL_CAPACITY 16

typedef struct {
  char* key;
  int value;
} MapEntry;

typedef struct {
  MapEntry* entries;
  int size;
  int capacity;
} Map;

typedef struct {
  Map* map;
  int index;
} MapIterator;

void init(Map* map);
void insert(Map* map, const char* key, int value);
int get(Map* map, const char* key);
void free_map(Map* map);
MapIterator get_iterator(Map* map);
int has_next(MapIterator* iterator);
MapEntry next(MapIterator* iterator);
void map_clean(Map* map);

#endif
