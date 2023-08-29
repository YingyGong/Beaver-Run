#include "polygon.h"
#include "list.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

double polygon_area(list_t *polygon) {
  double area = 0.0;
  size_t num = list_size(polygon);

  // use the shoelace formula to compute the area
  for (size_t i = 0; i < num; i++) {
    vector_t p1 = *(vector_t *)list_get(polygon, i % num);
    vector_t p2 = *(vector_t *)list_get(polygon, (i + 1) % num);
    area += (1.0 / 2.0) * vec_cross(p1, p2);
  }
  return fabs(area);
}

vector_t polygon_centroid(list_t *polygon) {
  double A = polygon_area(polygon);
  size_t num = list_size(polygon);
  vector_t toreturn = {0.0, 0.0};

  // use equation to compute the center of mass of a polygon
  for (size_t i = 0; i < num; i++) {
    vector_t p1 = *(vector_t *)list_get(polygon, i);
    vector_t p2 = *(vector_t *)list_get(polygon, (i + 1) % num);
    toreturn.x += (p1.x + p2.x) * (vec_cross(p1, p2));
    toreturn.y += (p1.y + p2.y) * (vec_cross(p1, p2));
  }
  toreturn = vec_multiply(1 / (6.0 * A), toreturn);
  return toreturn;
}

void polygon_translate(list_t *polygon, vector_t translation) {
  size_t num = list_size(polygon);
  for (size_t i = 0; i < num; i++) {
    vector_t *p = list_get(polygon, i);
    *p = vec_add(*p, translation);
  }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
  // translate the polygon so that it rotates around the origin
  vector_t neg_point = vec_multiply(-1, point);
  polygon_translate(polygon, neg_point);

  // then rotate every vector
  size_t num = list_size(polygon);
  for (size_t i = 0; i < num; i++) {
    vector_t *p = list_get(polygon, i);
    *p = vec_rotate(*p, angle);
  }

  // then translate the polygon back so that it rotates around the given point
  polygon_translate(polygon, point);
}
