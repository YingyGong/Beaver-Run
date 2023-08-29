#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "vector.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct body {
  list_t *shape;
  vector_t forces;
  vector_t impulses;
  double mass;
  double friction_coeff;
  vector_t velocity;
  rgb_color_t color;
  vector_t center;
  void *info;
  free_func_t info_freer;
  bool remove;
  double score;
  size_t remaining_lives;
  picture_t *picture;
  bool slow;
  bool double_points;
  bool magnet;
} body_t;

typedef struct info {
  size_t type;
} info_t;

typedef struct picture {
  size_t width;
  size_t length;
  size_t index;
} picture_t;


info_t *info_init() {
  info_t *info = malloc(sizeof(info_t));
  return info;
}

size_t info_type(info_t *info) { return info->type; }
void set_info_type(info_t *info, size_t type) { info->type = type; }

picture_t *picture_init(size_t index, size_t length, size_t width) {
  picture_t *picture = malloc(sizeof(picture_t));
  picture -> index = index;
  picture -> length = length;
  picture -> width = width;
  return picture;
}

size_t pic_index(picture_t *picture) { return picture->index;}
size_t pic_length(picture_t *picture) { return picture->length; }
size_t pic_width(picture_t *picture) { return picture->width; }


body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  return body_init_with_info(shape, mass, color, NULL, NULL, NULL);
}

body_t *body_init_with_velocity(list_t *shape, double mass, rgb_color_t color, void *info, free_func_t info_freer, vector_t initial_velocity) {
  body_t *body = body_init_with_info(shape, mass, color, info, info_freer, NULL);
  body_set_velocity(body, initial_velocity);
  return body;
}


// new constructor for body->info
body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer, void *picture) {

  body_t *body = malloc(sizeof(body_t));
  assert(body != NULL);
  body->shape = shape;
  body->mass = mass;
  body->velocity = VEC_ZERO;
  body->forces = VEC_ZERO;
  body->impulses = VEC_ZERO;
  body->color = color;
  body->center = polygon_centroid(shape);
  // add info and info_freer
  body->info = info;
  body->info_freer = info_freer;
  body->remove = false;
  body->score = 0.0;
  body->remaining_lives = 0;
  body->friction_coeff = 0.0;
  // add picture and picture_freer
  body->picture= picture;
  body->slow = false;
  body->magnet = false;
  return body;
}

void body_free(body_t *body) {
  list_free(body->shape);
  // add freer for info
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  if (body->picture != NULL) {
   free(body->picture);
  }
  free(body);
}


picture_t *body_get_picture(body_t *body)
{
  return body->picture;
}


list_t *body_get_shape(body_t *body) {
  list_t *r_shape = list_init(list_size(body->shape), free);

  for (size_t i = 0; i < list_size(body->shape); i++) {
    vector_t *new_vec = malloc(sizeof(vector_t));
    assert(new_vec != NULL);
    new_vec->x = ((vector_t *)list_get(body->shape, i))->x;
    new_vec->y = ((vector_t *)list_get(body->shape, i))->y;
    list_add(r_shape, new_vec);
  }

  return r_shape;
}

vector_t body_get_centroid(body_t *body) { return body->center; }

vector_t body_get_velocity(body_t *body) { return body->velocity; }

rgb_color_t body_get_color(body_t *body) { return body->color; }

double body_get_mass(body_t *body) { return body->mass; }

double body_get_mu(body_t *body) {return body->friction_coeff;}

void body_set_centroid(body_t *body, vector_t vec) {
  vector_t translate = vec_subtract(vec, body->center);
  polygon_translate(body->shape, translate);
  body->center.x = vec.x;
  body->center.y = vec.y;
}

void *body_get_info(body_t *body) { return body->info; };

void body_set_velocity(body_t *body, vector_t v) { body->velocity = v; 
}

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->shape, angle, body_get_centroid(body));
}

void body_set_score(body_t *body, double score){
  body->score = score;
}

double body_get_score(body_t *body){
  return body->score;
}

void body_add_score(body_t *body, double score){
  double new_score = body_get_score(body) + score;
  body->score = new_score;
}

void body_set_lives(body_t *body, size_t remaining_lives){
  body->remaining_lives = remaining_lives;
}

size_t body_get_lives(body_t *body){
  return body->remaining_lives;
}

void body_add_lives(body_t *body, size_t added_lives){
  body->remaining_lives = body_get_lives(body) + added_lives;
}

void body_reduce_live(body_t *body){
  body->remaining_lives --;
}

void body_set_friction_c(body_t *body, double friction_c){
  body->friction_coeff = friction_c;
}

void body_negate_score(body_t *body){
  body->score = - body->score;
}

void body_add_force(body_t *body, vector_t force) {
  body->forces = vec_add(body->forces, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulses = vec_add(body->impulses, impulse);
}

void body_tick(body_t *body, double dt) {
  if (!body_is_removed(body)) {

    // F = ma
    vector_t a = vec_multiply(1.0 / (body_get_mass(body)), body->forces);
    // J (impulse) = delta_v * m if m is constant
    vector_t delta_v =
        vec_multiply(1.0 / (body_get_mass(body)), body->impulses);
    vector_t old_v = body->velocity;

    body->velocity = vec_add(delta_v, vec_add(old_v, vec_multiply(dt, a)));
    // The body should be translated at the *average* of the
    // velocities before and after the tick
    vector_t distance = vec_multiply(dt / 2, vec_add(old_v, body->velocity));
    vector_t old_center = body_get_centroid(body);
    vector_t new_center = vec_add(old_center, distance);
    body_set_centroid(body, new_center);
    body->forces = VEC_ZERO;
    body->impulses = VEC_ZERO;
  }
}

vector_t body_get_force(body_t *body) { return body->forces; }

vector_t body_get_impulse(body_t *body) { return body->impulses; }

void body_remove(body_t *body) { body->remove = 1; }

bool body_is_removed(body_t *body) { return body->remove; }

void body_add_velocity(body_t *body, vector_t v) {body->velocity = vec_add(body->velocity, v);}

void body_set_slow(body_t *body, bool true_or_false){
  body->slow = true_or_false;
}

bool body_get_slow(body_t *body){
  return body->slow;
}

void body_set_double_points(body_t *body, bool true_or_false){
  body->double_points = true_or_false;
}

bool body_get_double_points(body_t *body){
  return body->double_points;
}

void body_set_magnet(body_t *body, bool true_or_false){
  body->magnet = true_or_false;
}

bool body_get_magnet(body_t *body){
  return body->magnet;
}
