#include "vector.h"
#include "test_util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const vector_t VEC_ZERO = {0, 0};

vector_t vec_add(vector_t v1, vector_t v2) {

  double x_add = v1.x + v2.x;
  double y_add = v1.y + v2.y;
  vector_t toreturn;
  toreturn.x = x_add;
  toreturn.y = y_add;
  return toreturn;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  vector_t neg_v2 = vec_negate(v2);
  return vec_add(v1, neg_v2);
}

vector_t vec_negate(vector_t v) { return vec_multiply(-1, v); }

vector_t vec_multiply(double scalar, vector_t v) {
  vector_t toreturn;
  toreturn.x = v.x * scalar;
  toreturn.y = v.y * scalar;
  return toreturn;
}

double vec_dot(vector_t v1, vector_t v2) {
  double x_add = v1.x * v2.x;
  double y_add = v1.y * v2.y;
  return x_add + y_add;
}

double vec_cross(vector_t v1, vector_t v2) { return v1.x * v2.y - v1.y * v2.x; }

vector_t vec_rotate(vector_t v, double angle) {
  double sintheta = sin(angle);
  double costheta = cos(angle);
  vector_t toreturn;
  toreturn.x = v.x * costheta - v.y * sintheta;
  toreturn.y = v.x * sintheta + v.y * costheta;
  return toreturn;
}

void vec_free(vector_t *vector) { free(vector); }

vector_t normalize(vector_t vec) {
  vector_t result = vec;
  double norm = sqrt(vec_dot(vec, vec));
  if (norm != 0) {
    result = vec_multiply(1.0 / norm, vec);
  }
  return result;
}

double distance(vector_t v1, vector_t v2) {
  return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

vector_t vec_project(vector_t line1, vector_t line2){
    return vec_multiply(vec_dot(line1, line2) / vec_dot(line2, line2), line2);
}

bool vec_opposite(vector_t v1, vector_t v2){
  v1 = normalize(v1);
  v2 = vec_negate(normalize(v2));
  return distance(v1, v2) < 0.001;
}
