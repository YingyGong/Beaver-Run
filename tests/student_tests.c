#include "body.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

list_t *make_shape() {
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

double gravity_potential(double G, body_t *body1, body_t *body2) {
  vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
  return -G * body_get_mass(body1) * body_get_mass(body2) / sqrt(vec_dot(r, r));
}
double kinetic_energy(body_t *body) {
  vector_t v = body_get_velocity(body);
  return body_get_mass(body) * vec_dot(v, v) / 2;
}
double get_distance(body_t *body1, body_t *body2) {
  vector_t displacement =
      vec_subtract(body_get_centroid(body1), body_get_centroid(body2));
  double distance = sqrt(vec_dot(displacement, displacement));
  return distance;
}

// Tests that the distance between two masses satisfies the gravity laws
void test_gravity() {
  // define the physical constants
  const double M1 = 400, M2 = 600;
  const double G = 10;
  const double DT = 1e-3;
  const int STEPS = 10;
  const vector_t VEC_ZERO = {0, 0};
  // initialize the scene and bodies
  scene_t *scene = scene_init();
  body_t *mass1 = body_init(make_shape(), M1, (rgb_color_t){0, 0, 0});
  scene_add_body(scene, mass1);
  body_set_centroid(mass1, (vector_t){0, 0});
  body_t *mass2 = body_init(make_shape(), M2, (rgb_color_t){0, 0, 0});
  body_set_centroid(mass2, (vector_t){10, 20});
  scene_add_body(scene, mass2);
  create_newtonian_gravity(scene, G, mass1, mass2);
  // initialize the position and velocity variables
  vector_t old_v1 = VEC_ZERO;
  vector_t old_v2 = VEC_ZERO;
  vector_t old_exp_pos1 = body_get_centroid(mass1);
  vector_t old_exp_pos2 = body_get_centroid(mass2);
  vector_t exp_pos1 = old_exp_pos1;
  vector_t exp_pos2 = old_exp_pos2;
  for (size_t i = 0; i < STEPS; i++) {
    // initialize velocity
    vector_t average_v1 = old_v1;
    vector_t average_v2 = old_v2;
    // ``assert'' is put in the beginning of the loop
    // test whether velocity is correct
    assert(vec_isclose(body_get_velocity(mass1), average_v1));
    assert(vec_isclose(body_get_velocity(mass2), average_v2));

    // test positions of mass1 and mass2
    assert(vec_isclose(exp_pos1, body_get_centroid(mass1)));
    assert(vec_isclose(exp_pos2, body_get_centroid(mass2)));

    // compute the expected di
    double dist = get_distance(mass1, mass2);
    vector_t pos1 = body_get_centroid(mass1);
    vector_t pos2 = body_get_centroid(mass2);
    vector_t displacement = vec_subtract(pos1, pos2);
    // compute the expected force
    double exp_force_magnitude = G * M1 * M2 / (pow(dist, 2));
    vector_t exp_force = vec_multiply(exp_force_magnitude / dist, displacement);
    // compute the velocity of body 1
    vector_t acceleration1 = vec_multiply(-1 / M1, exp_force);
    vector_t delta_v1 = vec_multiply(DT, acceleration1);
    vector_t velocity1 = vec_add(old_v1, delta_v1);
    average_v1 = vec_multiply(0.5, vec_add(old_v1, velocity1));

    // compute the velocity of body 2
    vector_t acceleration2 = vec_multiply(1 / M2, exp_force);
    vector_t delta_v2 = vec_multiply(DT, acceleration2);
    vector_t velocity2 = vec_add(old_v2, delta_v2);
    average_v2 = vec_multiply(0.5, vec_add(old_v2, velocity2));

    // compute the translation of body 1
    vector_t new_dis1 = vec_multiply(DT, average_v1);
    exp_pos1 = vec_add(old_exp_pos1, new_dis1);
    old_exp_pos1 = exp_pos1;

    // compute the translation of body 2
    vector_t new_dis2 = vec_multiply(DT, average_v2);
    exp_pos2 = vec_add(old_exp_pos2, new_dis2);
    old_exp_pos2 = exp_pos2;

    // update velocity
    old_v1 = velocity1;
    old_v2 = velocity2;

    scene_tick(scene, DT);
  }
  scene_free(scene);
}

void test_spring() {
  const double M = 100;
  const double K = 1;
  const double DT = 1e-6;
  const double D = 100;
  const int STEPS = 1000000;
  vector_t force;
  vector_t acceleration;
  vector_t velocity;
  vector_t displacement;
  vector_t final_translation;

  scene_t *scene = scene_init();
  body_t *mass = body_init(make_shape(), M, (rgb_color_t){0, 0, 0});
  body_set_centroid(mass, (vector_t){D, 0});
  scene_add_body(scene, mass);
  body_t *anchor = body_init(make_shape(), INFINITY, (rgb_color_t){0, 0, 0});
  scene_add_body(scene, anchor);
  body_set_centroid(anchor, (vector_t){0, 0});

  create_spring(scene, K, mass, anchor);
  for (size_t i = 0; i < STEPS; i++) {
    force = vec_multiply(
        -K, vec_subtract(body_get_centroid(mass), body_get_centroid(anchor)));
    acceleration = vec_multiply(1.0 / M, force);
    velocity = vec_add(vec_multiply(DT, acceleration), body_get_velocity(mass));
    vector_t average_v =
        vec_multiply(0.5, vec_add(velocity, body_get_velocity(mass)));
    displacement = vec_multiply(DT, average_v);
    final_translation = vec_add(displacement, body_get_centroid(mass));
    scene_tick(scene, DT);
    // test displacement of the body
    assert(vec_isclose(body_get_centroid(mass), final_translation));
    // test displacement of the anchor
    assert(vec_isclose(body_get_centroid(anchor), VEC_ZERO));

    // test velocity of the body
    assert(vec_isclose(body_get_velocity(mass), velocity));
    // test velocity of the anchor
    assert(vec_isclose(body_get_velocity(anchor), VEC_ZERO));
  }
  scene_free(scene);
}

void test_drag() {
  const double M = 100; // mass
  const double GAMMA = 0.1;
  const double DT = 1e-6;
  const double D = 100;
  const size_t STEPS = 1000000;

  vector_t initial_speed = {.x = 20, .y = 10};
  vector_t force;
  vector_t acceleration;
  vector_t velocity;
  vector_t displacement;
  vector_t final_translation;

  scene_t *scene = scene_init();
  body_t *mass = body_init(make_shape(), M, (rgb_color_t){0, 0, 0});
  body_set_centroid(mass, (vector_t){D, 0});
  body_set_velocity(mass, initial_speed);
  scene_add_body(scene, mass);
  create_drag(scene, GAMMA, mass);
  for (size_t i = 0; i < STEPS; i++) {

    force = vec_multiply(-GAMMA, body_get_velocity(mass));
    acceleration = vec_multiply(1.0 / M, force);
    velocity = vec_add(vec_multiply(DT, acceleration), body_get_velocity(mass));
    vector_t average_v =
        vec_multiply(0.5, vec_add(velocity, body_get_velocity(mass)));
    displacement = vec_multiply(DT, average_v);
    final_translation = vec_add(displacement, body_get_centroid(mass));

    scene_tick(scene, DT);

    // test displacement of the body
    assert(vec_isclose(body_get_centroid(mass), final_translation));

    // test velocity of body
    assert(vec_isclose(body_get_velocity(mass), velocity));
  }
  scene_free(scene);
}

int main(int argc, char *argv[]) {
  // Run all tests if there are no command-line arguments
  bool all_tests = argc == 1;
  // Read test name from file
  char testname[100];
  if (!all_tests) {
    read_testname(argv[1], testname, sizeof(testname));
  }

  DO_TEST(test_spring);
  DO_TEST(test_drag);
  DO_TEST(test_gravity);

  puts("forces_test PASS");
}
