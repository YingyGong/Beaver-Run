#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const size_t RESIZE_RATE = 2;

typedef struct list {
  void **elements;
  size_t size;
  size_t capacity;
  free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
  list_t *list = malloc(sizeof(list_t));
  assert(list != NULL);

  list->elements = malloc(initial_size * sizeof(void *));
  assert(list->elements != NULL);

  list->size = 0;
  list->capacity = initial_size;
  list->freer = freer;
  return list;
}

void list_free(list_t *list) 
{
  if (list != NULL) {
    if (list->freer != NULL) {
      for (size_t i = 0; i < list->size; i++) {
        list->freer(list->elements[i]);
      }
    }
    free(list->elements);
  }
  free(list);
}

size_t list_size(list_t *list) { return list->size; }

void *list_get(list_t *list, size_t index) {
  assert(index < list->size);
  return list->elements[index];
}

void *list_remove(list_t *list, size_t index) {
  assert(list->size > 0);
  assert(index < list->size);
  list->size--;
  void *removed_vector = list->elements[index];
  for (size_t i = index; i < list->size; i++) {
    list->elements[i] = list->elements[i + 1];
  }
  return removed_vector;
}

void list_add(list_t *list, void *value) {
  assert(value != NULL);
  if (list->size == list->capacity) {
    list_resize(list);
  }
  list->elements[list->size] = value;
  list->size++;
}

void list_resize(list_t *list) {
  size_t new_capacity;
  if (list->capacity == 0) {
    new_capacity = 1;
  } else {
    new_capacity = list->capacity * RESIZE_RATE;
  } // double the original size
  void **new_elements = realloc(list->elements, new_capacity * sizeof(void *));
  assert(new_elements != NULL);
  list->elements = new_elements;
  list->capacity = new_capacity;
}

list_t *list_copy(list_t *list)
{
  list_t *to_return = list_init(list_capacity(list), list->freer);
  for (size_t i = 0; i<list_size(list); i++)
  {
    list_add(to_return, list_get(list,i));
  }

  return to_return;
}
size_t list_capacity(list_t *list) { return list->capacity; }
