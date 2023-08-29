#include "forces.h"
#include "body.h"
#include "collision.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "vector.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const int MIN_DISTANCE = 10;
const double NON_CONSTANT = -1;
const double ELASTICITY_CONSTANT = 0.5;
const double EARTH_GRAVITY = 9.8;
const double WATER_DENSITY = 1.0;


// definition and functions for aux
typedef struct aux {
  list_t *bodies;
  double constant;
  void *other_aux;
  collision_handler_t handler;
  free_func_t freer;
  bool collided_or_not;
} aux_t;

aux_t *aux_init(list_t *bodies, double constant) {
  aux_t *aux = malloc(sizeof(aux_t));
  aux->bodies = bodies;
  aux->constant = constant;
  aux->handler = NULL;
  aux->freer = NULL;
  aux->collided_or_not = false;
  return aux;
}

body_t *aux_get_body(aux_t *aux, size_t index) {
  return list_get(aux->bodies, index);
}

double aux_get_constant(aux_t *aux) { return aux->constant; }

void *aux_get_other_aux(aux_t *aux) { return aux->other_aux; }

void aux_set_aux(aux_t *aux, void *info) { aux->other_aux = info; }

void aux_add_body(aux_t *aux, body_t *body) { list_add(aux->bodies, body); }

void aux_free(void *ptr) {
  aux_t *aux = (aux_t *)ptr;
  if (aux->freer != NULL && aux->other_aux != NULL) {
    aux->freer(aux->other_aux);
  }
  list_free(aux->bodies);
  free(aux);
}

void aux_set_freer(aux_t *aux, free_func_t fc) { aux->freer = fc; }

void aux_set_handler(aux_t *aux, collision_handler_t handler) {
  aux->handler = handler;
}

// 3  easy forcers
//force_creator_t for gravity
void gravity_creator(void *aux) {
  body_t *body1 = aux_get_body(aux, 0);
  body_t *body2 = aux_get_body(aux, 1);
  double G = aux_get_constant(aux);
  double m1 = body_get_mass(body1);
  double m2 = body_get_mass(body2);
  vector_t pos1 = body_get_centroid(body1);
  vector_t pos2 = body_get_centroid(body2);

  vector_t displacement = (vec_subtract(pos1, pos2));
  double distance_squared = vec_dot(displacement, displacement);

  if (distance_squared < MIN_DISTANCE * MIN_DISTANCE) {
    distance_squared = MIN_DISTANCE * MIN_DISTANCE;
  }
    double force_magnitude = G * m1 * m2 / distance_squared;
    vector_t force1 = vec_multiply(force_magnitude, normalize(displacement));
    body_add_force(body1, vec_negate(force1));
    body_add_force(body2, (force1));
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2) {
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, G);

  scene_add_bodies_force_creator(scene, (force_creator_t)gravity_creator, aux,
                                 bodies, (free_func_t)aux_free);
}

void earth_gravity_creator(void *aux) {
  body_t *body = aux_get_body(aux, 0);
  double g = aux_get_constant(aux);
  double m = body_get_mass(body);
  vector_t gravity = {0, - m * g};
  body_add_force(body, gravity);
}


void create_earth_gravity(scene_t *scene, double g, body_t *body) {
  list_t *bodies = list_init(1, NULL);
  list_add(bodies, body);
  aux_t *aux = aux_init(bodies, g);
  scene_add_bodies_force_creator(scene, (force_creator_t)earth_gravity_creator, aux,
                                 bodies, (free_func_t)aux_free);                                 
}


// force_creator_t for spring
void spring_creator(void *aux) {
  body_t *body1 = aux_get_body(aux, 0);
  body_t *body2 = aux_get_body(aux, 1);
  double k = aux_get_constant(aux);
  vector_t displacement =
      vec_subtract(body_get_centroid(body1), body_get_centroid(body2));
  vector_t force = vec_multiply(-k, displacement);
  body_add_force(body1, force);
  body_add_force(body2, vec_negate(force));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {

  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, k);

  scene_add_bodies_force_creator(scene, (force_creator_t)spring_creator, aux,
                                 bodies, (free_func_t)aux_free);
}

// force_creator_t for drag
void drag_creator(void *aux) {
  double gamma = aux_get_constant(aux);
  body_t *body = aux_get_body(aux, 0);
  vector_t force = vec_multiply(-gamma, body_get_velocity(body));
  body_add_force(body, force);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
  list_t *bodies = list_init(1, (free_func_t)NULL);
  list_add(bodies, body);
  aux_t *aux = aux_init(bodies, gamma);
  scene_add_bodies_force_creator(scene, (force_creator_t)drag_creator, aux,
                                 bodies, (free_func_t)aux_free);
}

// Collision
// Collision handlers
void destroy_handler(body_t *body1, body_t *body2, vector_t axis, void *aux) {
  body_remove(body1);
  body_remove(body2);
}

// general impulse
void physics_handler(body_t *body1, body_t *body2, vector_t axis, void *aux) {
  aux_t *aux_var = (aux_t *)aux;
  double elastity_constant = aux_var->constant;
  double m1 = body_get_mass(body1);
  double m2 = body_get_mass(body2);
  vector_t v1 = body_get_velocity(body1);
  vector_t v2 = body_get_velocity(body2);
  double u1 = vec_dot(v1, axis);
  double u2 = vec_dot(v2, axis);
  double reduced_mass = 0;
  if (m1 != INFINITY && m2 != INFINITY) {
    reduced_mass = (m1 * m2) / (m1 + m2);
  } else if (m1 == INFINITY) {
    reduced_mass = m2;
  } else if (m2 == INFINITY) {
    reduced_mass = m1;
  }
  vector_t impulse =
      vec_multiply(reduced_mass * (1 + elastity_constant) * (u2 - u1), axis);
  impulse = (vector_t) {.x = 0, .y = impulse.y};
  if (info_type(body_get_info(body1)) != BLOCK_INFO &&
      info_type(body_get_info(body2)) != BLOCK_INFO) {
    body_add_impulse(body1, impulse);
    body_add_impulse(body2, vec_negate(impulse));

  } else if (info_type(body_get_info(body1)) == BLOCK_INFO) {
    body_add_impulse(body2, vec_negate(impulse));
    body_remove(body1);
  } else if (info_type(body_get_info(body2)) == BLOCK_INFO) {
    body_add_impulse(body1, impulse);
    body_remove(body2);
  }
}

void half_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                            void *aux) {
  aux_t *aux_var = (aux_t *)aux;
  double elastity_constant = aux_var->constant;
  double m1 = body_get_mass(body1);
  double m2 = body_get_mass(body2);
  vector_t v1 = body_get_velocity(body1);
  vector_t v2 = body_get_velocity(body2);
  double u1 = vec_dot(v1, axis);
  double u2 = vec_dot(v2, axis);
  double reduced_mass = 0;
  if (m1 != INFINITY && m2 != INFINITY) {
    reduced_mass = (m1 * m2) / (m1 + m2);
  } else if (m1 == INFINITY) {
    reduced_mass = m2;
  } else if (m2 == INFINITY) {
    reduced_mass = m1;
  }
  vector_t impulse =
      vec_multiply(reduced_mass * (1 + elastity_constant) * (u2 - u1), axis);
  body_add_impulse(body1, impulse);
  body_add_score(body1, body_get_score(body2));
  if (body_get_score(body2) < 0){
    body_reduce_live(body1);
  }
  body_remove(body2);
}

void remove_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                            void *aux) {
  body_add_score(body1, body_get_score(body2));
  body_add_lives(body1, body_get_lives(body2));
  if (body_get_score(body2) < 0){
    body_reduce_live(body1);
  }  
  body_remove(body2);                              
                            }   

void slow_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                            void *aux) {
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t collision_info = find_collision(shape1, shape2);
  list_free(shape1);
  list_free(shape2);
  body_remove(body2);
  //body_set_slow(body1, true);
  if (collision_info.collided){
      body_set_slow(body1, true);
    } else{
      body_set_slow(body1, false);
    }
  }

void double_point_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                            void *aux) {
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t collision_info = find_collision(shape1, shape2);
  list_free(shape1);
  list_free(shape2);
  body_remove(body2);
  if (collision_info.collided){
      body_set_double_points(body1, true);
    } else{
      body_set_double_points(body1, false);
    }
  }

void magnet_collision_handler(body_t *body1, body_t *body2, vector_t axis,
                            void *aux) {
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t collision_info = find_collision(shape1, shape2);
  list_free(shape1);
  list_free(shape2);
  body_remove(body2);
  if (collision_info.collided){
      body_set_magnet(body1, true);
    } else{
      body_set_magnet(body1, false);
    }
  }


//force_creator_t for collision
void collision_creator(void *aux) {
  aux_t *aux_var = (aux_t *)aux;
  collision_handler_t collision_handler = aux_var->handler;

  body_t *body1 = aux_get_body(aux, 0);
  body_t *body2 = aux_get_body(aux, 1);
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t collision_info = find_collision(shape1, shape2);
  vector_t axis = collision_info.axis;

  // because body_get_shape returns copies, we need to free them
  list_free(shape1);
  list_free(shape2);
  
  if (collision_info.collided) {
    if (aux_var->collided_or_not == false) {
      aux_var->collided_or_not = true;
      collision_handler(body1, body2, axis, aux_var->other_aux);
      if (vec_opposite(body_get_velocity(body1), body_get_velocity(body2))){
        // because the beaver's score is 0, we can negate it anyway
        body_negate_score(body1);
        body_negate_score(body2);
      }
    }
  } else {
    aux_var->collided_or_not = false;
  }
}

//The general form of collision
void create_collision(scene_t *scene, body_t *body1, body_t *body2,
                      collision_handler_t handler, void *aux,
                      free_func_t freer) {
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux_new = aux_init(bodies, NON_CONSTANT);
  aux_set_aux(aux_new, aux);
  aux_set_freer(aux_new, freer);
  aux_set_handler(aux_new, handler);
  scene_add_bodies_force_creator(scene, (force_creator_t)collision_creator,
                                 aux_new, bodies, (free_func_t)aux_free);
}

void create_destructive_collision(scene_t *scene, body_t *body1,
                                  body_t *body2) {
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, NON_CONSTANT);

  create_collision(scene, body1, body2, (collision_handler_t)destroy_handler,
                   aux, (free_func_t)aux_free);
}

void create_physics_collision(scene_t *scene, double elasticity, body_t *body1,
                              body_t *body2) {
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, elasticity);
  create_collision(scene, body1, body2, (collision_handler_t)physics_handler,
                   aux, (free_func_t)aux_free);
}

void create_half_collision(scene_t *scene, double elasticity, body_t *body1,
                           body_t *body2) {
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, elasticity);
  create_collision(scene, body1, body2,
                   (collision_handler_t)half_collision_handler, aux,
                   (free_func_t)aux_free);
}

void create_remove_collision(scene_t *scene, body_t *body1,
                           body_t *body2) {
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, NON_CONSTANT);
  create_collision(scene, body1, body2,
                   (collision_handler_t)remove_collision_handler, aux,
                   (free_func_t)aux_free);
}

void create_slow_collision(scene_t *scene, body_t *body1,
                           body_t *body2){
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, NON_CONSTANT);
  create_collision(scene, body1, body2,
                   (collision_handler_t)slow_collision_handler, aux,
                   (free_func_t)aux_free);                          
                           }

void create_double_points_collision(scene_t *scene, body_t *body1,
                           body_t *body2){
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, NON_CONSTANT);
  create_collision(scene, body1, body2,
                   (collision_handler_t)double_point_collision_handler, aux,
                   (free_func_t)aux_free);                          
                           }

void create_magnet_collision(scene_t *scene, body_t *body1,
                           body_t *body2){
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, NON_CONSTANT);
  create_collision(scene, body1, body2,
                   (collision_handler_t)magnet_collision_handler, aux,
                   (free_func_t)aux_free);                          
                           }

// assume body1 is the beaver
void normal_force_creator(void *aux){
  aux_t *aux_var = (aux_t *)aux;
  body_t *body1 = aux_get_body(aux, 0);
  body_t *body2 = aux_get_body(aux, 1);
  double G = aux_get_constant(aux);
  double m = body_get_mass(body1);
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);
  collision_info_t collision_info = find_collision(shape1, shape2);
  list_free(shape1);
  list_free(shape2);
  if (collision_info.collided) { 
    vector_t normal_force = (vector_t) {0, m * G};
    body_add_force(body1, vec_multiply(1, normal_force));
    if (aux_var->collided_or_not == false) {
      aux_var->collided_or_not = true;
      body_set_velocity(body1, (vector_t) {0, 0});
    }
   
  } else {
    aux_var->collided_or_not = false;
  }
}
  
// body2 is the ground, no force added
void create_normal_force(scene_t *scene, double g, body_t *body1,
                           body_t *body2) {
  // old and physical way to implement normal force                            
  list_t *bodies = list_init(2, (free_func_t)(NULL));
  list_add(bodies, body1);
  list_add(bodies, body2);
  aux_t *aux = aux_init(bodies, g);
  scene_add_bodies_force_creator(scene, (force_creator_t)normal_force_creator, aux,
                                 bodies, (free_func_t)aux_free);
}

void buoyancy_creator(void *aux){
  body_t *body = aux_get_body(aux, 0);
  double m = body_get_mass(body);
  double a = aux_get_constant(aux);
  vector_t buoyancy = {0, m * a};
  body_add_force(body, buoyancy);
}

void create_buoyancy(scene_t *scene, double constant, body_t *body){
  list_t *bodies = list_init(1, (free_func_t)NULL);
  list_add(bodies, body);
  aux_t *aux = aux_init(bodies, constant);
  scene_add_bodies_force_creator(scene, (force_creator_t)buoyancy_creator, aux,
                                 bodies, (free_func_t)aux_free);
}



