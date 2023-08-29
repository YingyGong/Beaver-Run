#include "collision.h"
#include "body.h"
#include "list.h"
#include "scene.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// find the minimum value between 2 doubles
double find_min(double a, double b) {
  double min = a;
  if (b < a) {
    min = b;
  }
  return min;
}

// find the perpendicular line of the line connecting p1 and p2
vector_t find_perpline(vector_t p1, vector_t p2) {
  vector_t line = vec_subtract(p1, p2);
  vector_t perpline = {line.y, -line.x};
  return perpline;
}

// Given two points (a line), project them to a line
vector_t line_project_to_line(vector_t p1, vector_t p2, vector_t line) {
  vector_t vec = vec_subtract(p1, p2);
  vector_t toreturn =
      vec_multiply(vec_dot(vec, line) / vec_dot(line, line), line);
  return toreturn;
}

// Given a point, project it to a line
vector_t point_project_to_line(vector_t p, vector_t line) {
  vector_t toreturn =
      vec_multiply(vec_dot(p, line) / vec_dot(line, line), line);
  return toreturn;
}

// return the magnitude of a vector
double vec_magnitude(vector_t vec) { return sqrt(vec_dot(vec, vec)); }

typedef struct projected_line {
  vector_t max;
  vector_t min;
} projected_line_t;

projected_line_t *shape_project_to_line(list_t *shape, vector_t line) {
  projected_line_t *toreturn = malloc(sizeof(projected_line_t));
  assert(toreturn != NULL);
  vector_t min;
  vector_t max;
  for (size_t i = 0; i < list_size(shape); i++) {
    vector_t *point = (vector_t *)list_get(shape, i);
    vector_t after_projected = point_project_to_line(*point, line);
    if (i == 0) {
      max = after_projected;
      min = after_projected;
    }
    // If the "line" is parallel to the y axis, then any line projected onto
    // this "line" will all have x_value = 0, so we should compare the y_values
    // of each projected point.
    if (line.x == 0) {
      if (after_projected.y > max.y) {
        max = after_projected;
      }
      if (after_projected.y < min.y) {
        min = after_projected;
      }
    }
    // If the "line" isn't parallel to the y axis, then we can just compare the
    // x values of each point.
    else {
      if (after_projected.x > max.x) {
        max = after_projected;
      }
      if (after_projected.x < min.x) {
        min = after_projected;
      }
    }
  }
  toreturn->min = min;
  toreturn->max = max;
  return toreturn;
}

// Checks if two projected lines overlap
bool two_segments_not_overlap(projected_line_t *projline1,
                              projected_line_t *projline2) {
  bool result;
  if (projline1->max.x == 0) {
    result = projline1->max.y < projline2->min.y ||
             projline2->max.y < projline1->min.y;
  } else {
    result = projline1->max.x < projline2->min.x ||
             projline2->max.x < projline1->min.x;
  }
  return result;
}

collision_info_t find_collision(list_t *shape1, list_t *shape2) {
  double min_overlap_len = INFINITY;
  vector_t min_overlap_axis = VEC_ZERO;
  bool overlap = 1;
  for (size_t i = 0; i < list_size(shape1); i++) {
    vector_t p1 = *(vector_t *)list_get(shape1, i);
    vector_t p2 = *(vector_t *)list_get(shape1, (i + 1) % list_size(shape1));
    vector_t perp = find_perpline(p1, p2);
    projected_line_t *projline1 = shape_project_to_line(shape1, perp);
    projected_line_t *projline2 = shape_project_to_line(shape2, perp);
    // the projections onto this perpline do not overlap
    if (two_segments_not_overlap(projline1, projline2)) {
      overlap = 0;
      free(projline1);
      free(projline2);
      break;
    }
    // the projections onto this perpline do not overlap
    else {
      double overlap_len = find_min(distance(projline1->max, projline2->min),
                                    distance(projline2->max, projline1->min));
      if (overlap_len < min_overlap_len && overlap_len != 0) {
        min_overlap_len = overlap_len;
        min_overlap_axis = perp;
      }
    }
    free(projline1);
    free(projline2);
  }
  for (size_t i = 0; i < list_size(shape2); i++) {
    vector_t p1 = *(vector_t *)list_get(shape2, i);
    vector_t p2 = *(vector_t *)list_get(shape2, (i + 1) % list_size(shape2));
    vector_t perp = find_perpline(p1, p2);
    projected_line_t *projline1 = shape_project_to_line(shape1, perp);
    projected_line_t *projline2 = shape_project_to_line(shape2, perp);
    // the projections onto this perpline do not overlap
    if (two_segments_not_overlap(projline1, projline2)) {
      overlap = 0;
      free(projline1);
      free(projline2);
      break;
    }
    // the projections onto this perpline do not overlap
    else {
      double overlap_len = find_min(distance(projline1->max, projline2->min),
                                    distance(projline2->max, projline1->min));
      if (overlap_len < min_overlap_len && overlap_len != 0) {
        min_overlap_len = overlap_len;
        min_overlap_axis = perp;
      }
    }
    free(projline1);
    free(projline2);
  }

  // unit overlap_axis
  min_overlap_axis = normalize(min_overlap_axis);

  // create a struct to store collision info
  collision_info_t collision_info;
  collision_info.collided = overlap;
  collision_info.axis = min_overlap_axis;

  return collision_info;
}
