#include "body.h"
#include "collision.h"
#include "list.h"
#include "scene.h"
#include "test_util.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

list_t *make_shape_1() {
  list_t *shape = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){-1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+1, +1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-1, +1};
  list_add(shape, v);
  return shape;
}

list_t *make_shape_3() {
  list_t *shape = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){1, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){3, -1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){3, +1};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){1, +1};
  list_add(shape, v);
  return shape;
}

list_t *make_shape_2() {
  list_t *shape = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = (vector_t){-2, -2};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+2, -2};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){+2, +2};
  list_add(shape, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){-2, +2};
  list_add(shape, v);
  return shape;
}

void test_collision() {
  list_t *shape1 = make_shape_1();
  list_t *shape2 = make_shape_2();
  list_t *shape3 = make_shape_3();
  collision_info_t collision_info = find_collision(shape1, shape3);
  assert(collision_info.collided == 1);
  list_free(shape1);
  list_free(shape2);
  list_free(shape3);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }
  DO_TEST(test_collision);
  puts("collision_test PASS");
}
