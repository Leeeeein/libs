#include <stdio.h>
#include <stdlib.h>
#include "ustring.h"

int main() {
  String str;
  init(&str);
  append(&str, 'h');
  append(&str, 'e');
  append(&str, 'l');
  append(&str, 'l');
  append(&str, 'o');

  printf("str: %s\n", c_str(&str));
  printf("str size: %d\n", size(&str));
  printf("str capacity: %d\n", capacity(&str));
  printf("str empty: %d\n", empty(&str));

  reserve(&str, 10);
  printf("str capacity: %d\n", capacity(&str));

  shrink_to_fit(&str);
  printf("str capacity: %d\n", capacity(&str));

  assign(&str, "world");
  printf("str: %s\n", c_str(&str));
  printf("str compare: %d\n", compare(&str, "world"));

  insert(&str, 2, 'l');
  printf("str: %s\n", c_str(&str));

  erase(&str, 2);
  printf("str: %s\n", c_str(&str));

  push_back(&str, '!');
  printf("str: %s\n", c_str(&str));

  pop_back(&str);
  printf("str: %s\n", c_str(&str));

  printf("str at index 2: %c\n", at(&str, 2));

  replace(&str, 2, 'L');
  printf("str: %s\n", c_str(&str));

  clear(&str);
  printf("str size: %d\n", size(&str));
  printf("str empty: %d\n", empty(&str));

  return 0;
}
