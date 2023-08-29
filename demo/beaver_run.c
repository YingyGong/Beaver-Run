#include "body.h"
#include "collision.h"
#include "color.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "state.h"
#include "vector.h"
#include "test_util.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


#define CIRCLE_POINTS 40
#define G 200
#define M 5E17
#define PI 3.14159265359
const double g = 10e3;

// window constants
const vector_t WINDOW = (vector_t) {.x=1000, .y=500};
const vector_t CENTER = (vector_t) {.x=500, .y=250};

// ball (simulates beaver)
const double BALL_RAD = 30.0;
const rgb_color_t BALL_COLOR = (rgb_color_t){.r = 1.0, .g = 0, .b = 1.0};
const double BALL_SPEED = 200;
const double BALL_MASS = 100;
const double WRAP_DIS = 20;
const size_t INIT_LIVES = 5;
const size_t BEAVER_SURFACE_IDX = 0;
const size_t BEAVER_IDX = 1;
const size_t BEAVER_PIC_LENGTH = 150;
const size_t BEAVER_PIC_WIDTH = 150;

// brick (simulates obstacles)
const double BRICK_WIDTH_BOTTOM = 5.0;
const double BRICK_WIDTH_TOP = 15.0;
const double BRICK_LENGTH = 85.0;
const double MIN_BRICK_HEIGHT = 100.0;
const rgb_color_t BRICK_COLOR = (rgb_color_t){.r = 1.0, .g = 0.6, .b = 1.0};
const size_t BRICK_MASS = INFINITY;
const size_t BRICK_DIST = 150;
const double BRICK_ELASTICITY = 0.1;
const double BRICK_PROB = 0.6;

// door - actually a wall... (used for level transitions)
const double DOOR_WIDTH = 40.0;
const double DOOR_LENGTH = 500;
const rgb_color_t DOOR_COLOR = (rgb_color_t){.r = 0.5, .g = 0.6, .b = 1.0};
const size_t DOOR_MASS = INFINITY;

// deadline - our enemy at Caltech
const double DDL_RAD = 15.0;
const rgb_color_t DDL_COLOR = (rgb_color_t){.r = 0.8, .g = 0, .b = 0};
const double DDL_MASS = 8.0;
const vector_t DDL_SPEED = (vector_t) {.x=-200, .y=0};
const size_t NUM_DDLS = 12;
const double DDL_SCORE = -1;
const size_t DDL_SURFACE_IDX = 16;
const size_t DDL_PIC_LENGTH = 40;
const size_t DDL_PIC_WIDTH = 40;


// bird - another enemy
const double BIRD_RAD = 15.0;
const rgb_color_t BIRD_COLOR = (rgb_color_t){.r = 0.0, .g = 0.0, .b = 0.0};
const double BIRD_MASS = 8.0;
const vector_t BIRD_SPEED = (vector_t) {.x=-200, .y=0};
const size_t NUM_BIRDS = 12;
const double MIN_BIRD_HEIGHT = 100;
const double BIRD_SCORE = -1;
const vector_t ENEMY_SLOW_VELOCITY = (vector_t) {.x=-170, .y=0};
const size_t CROW_SURFACE_IDX = 15;
const size_t BIRD_PIC_LENGTH = 80;
const size_t BIRD_PIC_WIDTH = 80;


// fish - enemy in water
const double FISH_SHORT_SIDE = 10.0;
const double FISH_LONG_SIDE = 20.0;
const rgb_color_t FISH_COLOR = (rgb_color_t){.r = 0.7, .g = 0.1, .b = 0.1};
const size_t FISH_MASS = 10;
const size_t FISH_VTX_NUM = 10;
const double FISH_SCORE = -1;
const size_t NUM_FISH = 20;
const vector_t FISH_SPEED = (vector_t) {.x=-200, .y=0};
const size_t SHARK_SURFACE_IDX = 17;
const size_t SHARK_PIC_LENGTH = 150;
const size_t SHARK_PIC_WIDTH = 50;

// space trash - enemy in sky
const double TRASH_SHORT_SIDE = 10.0;
const double TRASH_LONG_SIDE = 20.0;
const rgb_color_t TRASH_COLOR = (rgb_color_t){.r = 0.6, .g = 0.3, .b = 0.0};
const size_t TRASH_MASS = 10;
const size_t TRASH_VTX_NUM = 12;
const double TRASH_SCORE = -1;
const vector_t TRASH_SPEED = (vector_t) {.x=-250, .y=0};
const size_t NUM_TRASH = 30;
const size_t TRASH_SURFACE_IDX = 18;
const size_t TRASH_PIC_LENGTH = 80;
const size_t TRASH_PIC_WIDTH = 80;

// coin
const double COIN_RAD = 12.0;
const rgb_color_t COIN_COLOR = (rgb_color_t){.r = 1.0, .g = 0.84, .b = 0.0};
const double COIN_MASS = 0.001;
const double COIN_DIST = 15.0;
const double COIN_FLOOR_PROB = 0.1;
const double COIN_SCORE = 20.0;
const size_t TOTAL_RAND_COINS = 200;
const size_t COIN_SURFACE_IDX = 10;

// power up
const double POWER_WIDTH = 30.0;
const rgb_color_t POWER_COLOR_1 = (rgb_color_t){.r = 0.1, .g = 0.5, .b = 0.7};
const rgb_color_t POWER_COLOR_2 = (rgb_color_t){.r = 0.7, .g = 0.3, .b = 0.2};
const rgb_color_t POWER_COLOR_3 = (rgb_color_t){.r = 0.2, .g = 0.7, .b = 0.5};
const rgb_color_t POWER_COLOR_4 = (rgb_color_t){.r = 0.5, .g = 0.5, .b = 0.8};
const double POWER_MASS = 5.0;
const double MIN_POWER_HEIGHT = 40.0;
const double MAX_POWER_HEIGHT = 450.0;
const size_t NUM_SLOW_DOWN = 3;
const size_t NUM_DOUBLE_POINTS = 3;
const size_t NUM_ADD_LIFE = 3;
const size_t NUM_MAGNET = 3;
const size_t MAGNET_DISTANCE = 250;
const size_t BOBA_SURFACE_IDX = 11;
const size_t COFFEE_SURFACE_IDX = 12;
const size_t ICE_SURFACE_IDX = 13;
const size_t OFFER_SURFACE_IDX = 14;

const double ONE_PT = 0.1;
const double DOUBLE_POINTS = 0.2;
const size_t POWER_UP_TIME = 10;

// scene
const size_t SCENE_WIDTH = 10000;
const size_t SCENE_HEIGHT = 50;

// water
const size_t WATER_WIDTH = 1000;
const size_t WATER_HEIGHT = 500;
const rgb_color_t WATER_COLOR = (rgb_color_t){.r = 0.0, .g = 0, .b = 1.0};
const size_t WATER_SCENE_INDEX = 4;
const size_t WATER_SURFACE_IDX = 2;

// ground
const size_t GROUND_WIDTH = 1000;
const size_t GROUND_HEIGHT = 50;
const rgb_color_t GROUND_COLOR = (rgb_color_t){.r = 0.0, .g = 1.0, .b = 0};
const size_t GROUND_SCENE_INDEX = 2;
const size_t GROUND_SURFACE_IDX = 1;

// sky
const size_t SKY_WIDTH = 1000;
const size_t SKY_HEIGHT = 500;
const rgb_color_t SKY_COLOR = (rgb_color_t){.r = 0.0, .g = 0.0, .b = 0.5};
const size_t SKY_SCENE_INDEX = 6;
const size_t SKY_SURFACE_IDX = 3;

// speed of background
const vector_t BACKGROUND_SPEED = (vector_t) {.x=-160, .y=0};

// surface indices of welcome page, transition page, and end page
const size_t WELCOME_SURFACE_INDEX = 4;
const size_t WELCOME_SCENE_INDEX = 0;
const size_t GAMEPLAY_SURFACE_INDEX = 5;
const size_t GAMEPLAY_SCENE_INDEX = 1;
const size_t TRANSITION1_SURFACE_INDEX = 6; 
const size_t TRANSITION1_SCENE_INDEX = 3; 
const size_t TRANSITION2_SURFACE_INDEX = 7; 
const size_t TRANSITION2_SCENE_INDEX = 5; 
const size_t END_SURFACE_INDEX = 8;
const size_t END_SCENE_INDEX = 7;
const size_t LOSE_SURFACE_INDEX = 9; 
const size_t LOSE_SCENE_INDEX = 8; 

const size_t TRANSITION_NUM_FONTS = 1;
const size_t LEVELS_NUM_FONTS = 1;
// transition window size
const size_t TRANSITION_WIDTH = 1000;
const size_t TRANSITION_HEIGHT = 500;

// font indices
const size_t GROUND_FONT_SCORE_IDX = 0;
const size_t GROUND_FONT_LIVE_IDX = 1;
const size_t WATER_FONT_SCORE_IDX = 2;
const size_t WATER_FONT_LIVE_IDX = 3;
const size_t SKY_FONT_SCORE_IDX = 4;
const size_t SKY_FONT_LIVE_IDX = 5;
const size_t TRANSITION_FONT_SCORE_IDX = 6;
const size_t END_FONT_SCORE_IDX = 7;

const size_t NUM_SURFACES = 20;
const size_t NUM_SCENES = 9;
const size_t NUM_FONTS = 8;

const size_t TIME_BETWEEN_SPACE = 1;

typedef enum {
  BACKGROUND,
  BALL,
  BIRD,
  BOARDER,
  BRICK,
  BRICK_TOP,
  COIN,
  DDL,
  DOOR,
  GRAVITY,
  TRASH,
  FISH,
  POWER_SLOW,
  POWER_POINTS,
  POWER_LIVE,
  POWER_MAGNET
} body_type_t;

// define different levels
typedef enum{
  GROUND,
  WATER,
  SKY,
  RAND
}background_type_t;

typedef struct state {
  list_t *scenes;
  size_t curr_scene;
  double last_hit_space;
  background_type_t background;
  double time_elapsed_slow;
  double time_elapsed_double;
  double time_elapsed_magnet;
  bool state_slow;
  bool state_double;
  bool state_magnet;
  double total_points;
} state_t;


body_type_t *make_type_info(body_type_t type) {
  body_type_t *info = malloc(sizeof(*info));
  *info = type;
  return info;
}


body_type_t get_type(body_t *body) {
  return *(body_type_t *)body_get_info(body);
}


list_t *rect_init(double width, double height) {
  vector_t half_width = {.x = width / 2, .y = 0.0},
           half_height = {.x = 0.0, .y = height / 2};
  list_t *rect = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  *v = vec_add(half_width, half_height);
  list_add(rect, v);
  v = malloc(sizeof(*v));
  *v = vec_subtract(half_height, half_width);
  list_add(rect, v);
  v = malloc(sizeof(*v));
  *v = vec_negate(*(vector_t *)list_get(rect, 0));
  list_add(rect, v);
  v = malloc(sizeof(*v));
  *v = vec_subtract(half_width, half_height);
  list_add(rect, v);
  return rect;
}

// returns a list with points of a star in clockwise order
list_t *star_init(vector_t center, double long_side, double short_side,
                  size_t vertex_num) {
  list_t *vertices = list_init(vertex_num, free);
  // initializing list of points
  for (size_t i = 0; i < vertex_num; i++) {
    // initializing each point
    vector_t *point = malloc(sizeof(vector_t));
    // coordinates of the points of the star
    if (i % 2 == 0) {
      point->x = center.x + long_side * sin(PI / 2 - PI * 2.0 / vertex_num * i);
      point->y = center.y + long_side * cos(PI / 2 - PI * 2.0 / vertex_num * i);
    }
    // coordinates of the angled points of the star
    else {
      point->x =
          center.x + short_side * sin(PI / 2 - PI * 2.0 / vertex_num * i);
      point->y =
          center.y + short_side * cos(PI / 2 - PI * 2.0 / vertex_num * i);
    }
    // adding points to list
    list_add(vertices, point);
  }
  return vertices;
}


/** Constructs a circles with the given radius centered at (0, 0) */
list_t *circle_init(double radius) {
  list_t *circle = list_init(CIRCLE_POINTS, free);
  double arc_angle = 2 * M_PI / CIRCLE_POINTS;
  vector_t point = {.x = radius, .y = 0.0};
  for (size_t i = 0; i < CIRCLE_POINTS; i++) {
    vector_t *v = malloc(sizeof(*v));
    *v = point;
    list_add(circle, v);
    point = vec_rotate(point, arc_angle);
  }
  return circle;
}

/** Creates background, either water, ground or sky */
void add_background(scene_t *scene, double length, double width, rgb_color_t color, size_t index) {
  list_t *rect = rect_init(length, width);
  body_t *body = body_init_with_info(rect, M, color,
                                     make_type_info(BACKGROUND), free, picture_init(index, length, width));
  body_set_velocity(body, VEC_ZERO);
  body_set_centroid(body, (vector_t){.x = length/2, .y = width/2});
  scene_add_body(scene, body);

}


/** Adds a beaver to the scene */
void add_beaver(scene_t *scene) {
  // Add the ball to the scene.
  body_t *ball = body_init_with_info(circle_init(BALL_RAD), BALL_MASS, BALL_COLOR, make_type_info(BALL), free, picture_init(BEAVER_SURFACE_IDX, BEAVER_PIC_LENGTH, BEAVER_PIC_WIDTH));
  body_set_velocity(ball, VEC_ZERO);
  body_set_centroid(ball, CENTER);
  body_set_lives(ball, INIT_LIVES);
  body_set_score(ball, 0);
  scene_add_body(scene, ball);
}

/* Adds a transition door to the scene*/
void add_door(scene_t *scene) {
  vector_t centroid = (vector_t) {SCENE_WIDTH - CENTER.x, CENTER.y};
  body_t *door = body_init_with_info(rect_init(DOOR_WIDTH, DOOR_LENGTH), DOOR_MASS, DOOR_COLOR, make_type_info(DOOR), free, NULL);
  body_set_velocity(door, BACKGROUND_SPEED);
  body_set_centroid(door, centroid);
  scene_add_body(scene, door);
}

/* Adds a coin to the scene*/
void add_one_coin(scene_t *scene, vector_t centroid) {
  body_t *coin = body_init_with_info(circle_init(COIN_RAD), COIN_MASS, COIN_COLOR, make_type_info(COIN), free, NULL);
  body_set_velocity(coin, BACKGROUND_SPEED);
  body_set_centroid(coin, centroid);
  body_set_score(coin, COIN_SCORE);
  scene_add_body(scene, coin);
}

/* Adds coins of random location on the ground in the scene*/
void add_floor_coins(scene_t *scene){
  size_t num_coins = (SCENE_WIDTH - CENTER.x) / COIN_DIST;
  for (size_t i = 0; i < num_coins; i++){
    // decide whether to add a coin
    double unif_rv = (double)rand() / (double)RAND_MAX;
    if (unif_rv < COIN_FLOOR_PROB) {
      double pos_y = SCENE_HEIGHT + COIN_RAD;
      double pos_x = i * COIN_DIST;
      vector_t coin_centroid = (vector_t){pos_x, pos_y};
      add_one_coin(scene, coin_centroid);
    }
  }
}

/* Add coins in random positions in water and sky scene*/
void add_random_coins(scene_t *scene){
  for (size_t i = 0; i < TOTAL_RAND_COINS; i++){
    double rand_x = (double)rand() / (double)RAND_MAX;
    double rand_y = (double)rand() / (double)RAND_MAX;
    double pos_x = rand_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = rand_y * WINDOW.y;
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_coin(scene, centroid);
  }
}


/* Adds a "moving" deadline to the scene */
void add_one_ddl(scene_t *scene, vector_t centroid) {
  body_t *ddl = body_init_with_info(circle_init(DDL_RAD), DDL_MASS, DDL_COLOR, make_type_info(DDL), free, picture_init(DDL_SURFACE_IDX, DDL_PIC_LENGTH, DDL_PIC_WIDTH));
  body_set_velocity(ddl, DDL_SPEED);
  body_set_centroid(ddl, centroid);
  body_set_score(ddl, DDL_SCORE);
  scene_add_body(scene, ddl);
}

/* Adds "moving" deadlines of random location on the ground*/
void add_ddls(scene_t *scene){
  for (size_t i = 0; i < NUM_DDLS; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * SCENE_WIDTH;
    double pos_y = SCENE_HEIGHT + DDL_RAD;
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_ddl(scene, centroid);
  }
}

/* Adds a "moving" deadline to the scene*/
void add_one_ddl_non_ground(scene_t *scene, vector_t centroid) {
  body_t *ddl = body_init_with_info(circle_init(DDL_RAD), DDL_MASS, DDL_COLOR, make_type_info(DDL), free, picture_init(DDL_SURFACE_IDX, DDL_PIC_LENGTH, DDL_PIC_WIDTH));
  body_set_velocity(ddl, DDL_SPEED);
  body_set_centroid(ddl, centroid);
  body_set_score(ddl, DDL_SCORE);
  scene_add_body(scene, ddl);
}

/* Adds "moving" deadlines of random location in water/sky*/
void add_ddls_non_ground(scene_t *scene){
  for (size_t i = 0; i < NUM_DDLS; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = ran_y * WINDOW.y;
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_ddl_non_ground(scene, centroid);
  }
}

/* Adds a "moving" bird to the scene*/
void add_one_bird(scene_t *scene, vector_t centroid) {
  body_t *bird = body_init_with_info(circle_init(BIRD_RAD), BIRD_MASS, BIRD_COLOR, make_type_info(BIRD), free, picture_init(CROW_SURFACE_IDX, BIRD_PIC_LENGTH, BIRD_PIC_WIDTH));
  body_set_velocity(bird, BIRD_SPEED);
  body_set_centroid(bird, centroid);
  body_set_score(bird, BIRD_SCORE);
  scene_add_body(scene, bird);
}

/* Adds "moving" birds of random location in the scene*/
void add_birds(scene_t *scene){
  for (size_t i = 0; i < NUM_DDLS; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = ran_y * CENTER.y + MIN_BIRD_HEIGHT;
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_bird(scene, centroid);
  }
}

/* Adds one fish to the scene*/
void add_one_fish(scene_t *scene, vector_t centroid) {
  body_t *fish = body_init_with_info(star_init(centroid, FISH_LONG_SIDE, 
    FISH_SHORT_SIDE, FISH_VTX_NUM), FISH_MASS, FISH_COLOR, make_type_info(FISH),
     free, picture_init(SHARK_SURFACE_IDX, SHARK_PIC_LENGTH, SHARK_PIC_WIDTH));
  body_set_velocity(fish, FISH_SPEED);
  body_set_centroid(fish, centroid);
  body_set_score(fish, FISH_SCORE);
  scene_add_body(scene, fish);
}

/* Adds trash of random locations in the scene*/
void add_many_fish(scene_t *scene){
  for (size_t i = 0; i < NUM_FISH; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = ran_y * WINDOW.y;
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_fish(scene, centroid);
  }
}


/* Adds a piece of trash to the scene*/
void add_one_trash(scene_t *scene, vector_t centroid) {
  body_t *trash = body_init_with_info(star_init(centroid, TRASH_LONG_SIDE, 
    TRASH_SHORT_SIDE, TRASH_VTX_NUM), TRASH_MASS, TRASH_COLOR, make_type_info(TRASH),
     free, picture_init(TRASH_SURFACE_IDX, TRASH_PIC_LENGTH, TRASH_PIC_WIDTH));
  body_set_velocity(trash, TRASH_SPEED);
  body_set_centroid(trash, centroid);
  body_set_score(trash, TRASH_SCORE);
  scene_add_body(scene, trash);
}

/* Adds trash of random locations in the scene*/
void add_many_trash(scene_t *scene){
  for (size_t i = 0; i < NUM_TRASH; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = ran_y * WINDOW.y;
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_trash(scene, centroid);
  }
}


/* Adds bottom part of the brick*/
void add_one_brick_bottom(scene_t *scene, vector_t centroid) {
  // Add the ball to the scene.
  body_t *brick = body_init_with_info(rect_init(BRICK_LENGTH, BRICK_WIDTH_BOTTOM), BRICK_MASS, BRICK_COLOR, make_type_info(BRICK), free, NULL);
  body_set_velocity(brick, BACKGROUND_SPEED);
  body_set_centroid(brick, (vector_t){.x =centroid.x, .y = centroid.y -7.5});
  scene_add_body(scene, brick);
}

/* Add top part of the brick*/
void add_one_brick_top(scene_t *scene, vector_t centroid) {
  // Add the ball to the scene.
  body_t *brick = body_init_with_info(rect_init(BRICK_LENGTH, BRICK_WIDTH_TOP), BRICK_MASS, BRICK_COLOR, make_type_info(BRICK_TOP), free, NULL);
  body_set_velocity(brick, BACKGROUND_SPEED);
  body_set_centroid(brick, (vector_t){.x =centroid.x, .y = centroid.y +2.5});
  scene_add_body(scene, brick);
}

/* Add a brick*/
void add_one_brick(scene_t *scene, vector_t centroid) {
  add_one_brick_bottom(scene, centroid);
  add_one_brick_top(scene, centroid);
}

/* Adds bricks and coins of random location in the scene*/
void add_bricks_and_coins(scene_t *scene){
  size_t num_bricks = SCENE_WIDTH / BRICK_DIST;
  size_t num_after_door = CENTER.x / BRICK_DIST;
  for (size_t i = 0; i < num_bricks - num_after_door; i++){
    // decide whether to add the brick
    double unif_rv = (double)rand() / (double)RAND_MAX;
    if (unif_rv < BRICK_PROB) {
      // add_bricks
      double ran_y = (double)rand() / (double)RAND_MAX;
      double pos_y = ran_y * CENTER.y + SCENE_HEIGHT + MIN_BRICK_HEIGHT;
      double pos_x = i * BRICK_DIST;
      vector_t brick_centroid = (vector_t){pos_x, pos_y};
      add_one_brick(scene, brick_centroid);
      // add_coins
      size_t num_coins = rand() % 4;
      double coin_pos_y = pos_y + 2 * COIN_RAD;
      double coin_pos_x = pos_x + BRICK_LENGTH * ( - 0.5 + (1/(num_coins + 1)));
      for (size_t j = 0; j < num_coins; j++) {
        coin_pos_x += BRICK_LENGTH / (num_coins + 1);
        vector_t coin_centroid = (vector_t){coin_pos_x, coin_pos_y};
        add_one_coin(scene, coin_centroid);
      }
    }
  }
}

/* Adds power up: slowing down background speed*/
void add_one_slow_down(scene_t *scene, vector_t centroid, rgb_color_t color) {
  body_t *slowdown = body_init_with_info(rect_init(POWER_WIDTH, POWER_WIDTH),
    POWER_MASS, color, make_type_info(POWER_SLOW), free, NULL);
  body_set_velocity(slowdown, BACKGROUND_SPEED);
  body_set_centroid(slowdown, centroid);
  scene_add_body(scene, slowdown);
}


/* Adds slow down power ups of random locations in the scene*/
void add_many_slow_downs(scene_t *scene){
  for (size_t i = 0; i < NUM_SLOW_DOWN; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = (MAX_POWER_HEIGHT - MIN_POWER_HEIGHT) * ran_y + MIN_POWER_HEIGHT; 
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_slow_down(scene, centroid, POWER_COLOR_1);
  }
}

/* Adds power up: double points for every tick for 5 secs*/
void add_one_double_points(scene_t *scene, vector_t centroid, rgb_color_t color) {
  body_t *double_points = body_init_with_info(rect_init(POWER_WIDTH, POWER_WIDTH),
    POWER_MASS, color, make_type_info(POWER_POINTS), free, NULL);
  body_set_velocity(double_points, BACKGROUND_SPEED);
  body_set_centroid(double_points, centroid);
  scene_add_body(scene, double_points);
}


/* Adds double points power ups of random locations in the scene*/
void add_many_double_points(scene_t *scene){
  for (size_t i = 0; i < NUM_DOUBLE_POINTS; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = (MAX_POWER_HEIGHT - MIN_POWER_HEIGHT) * ran_y + MIN_POWER_HEIGHT; 
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_double_points(scene, centroid, POWER_COLOR_2);
  }
}

/* Adds power up: add a live */
void add_one_shield(scene_t *scene, vector_t centroid, rgb_color_t color) {
  body_t *shield = body_init_with_info(rect_init(POWER_WIDTH, POWER_WIDTH),
    POWER_MASS, color, make_type_info(POWER_LIVE), free, NULL);
  body_set_velocity(shield, BACKGROUND_SPEED);
  body_set_centroid(shield, centroid);
  body_set_lives(shield, 1);
  scene_add_body(scene, shield);
}

/* Adds live power ups of random locations in the scene*/
void add_many_shields(scene_t *scene){
  for (size_t i = 0; i < NUM_ADD_LIFE; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = (MAX_POWER_HEIGHT - MIN_POWER_HEIGHT) * ran_y + MIN_POWER_HEIGHT; 
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_shield(scene, centroid, POWER_COLOR_3);
  }
}

/* Adds power up: magnet to get all the coins in sight for 5s */
void add_one_magnet(scene_t *scene, vector_t centroid, rgb_color_t color) {
  body_t *magnet = body_init_with_info(rect_init(POWER_WIDTH, POWER_WIDTH),
    POWER_MASS, color, make_type_info(POWER_MAGNET), free, NULL);
  body_set_velocity(magnet, BACKGROUND_SPEED);
  body_set_centroid(magnet, centroid);
  scene_add_body(scene, magnet);
}

/* Adds live power ups of random locations in the scene*/
void add_many_magnets(scene_t *scene){
  for (size_t i = 0; i < NUM_MAGNET; i++){
    double ran_x = (double)rand() / (double)RAND_MAX;
    double ran_y = (double)rand() / (double)RAND_MAX;
    double pos_x = ran_x * (SCENE_WIDTH - CENTER.x);
    double pos_y = (MAX_POWER_HEIGHT - MIN_POWER_HEIGHT) * ran_y + MIN_POWER_HEIGHT; 
    vector_t centroid = (vector_t){pos_x, pos_y};
    add_one_magnet(scene, centroid, POWER_COLOR_4);
  }
}

/* Adds forces to the state */
void add_forces(scene_t *scene, size_t index)
{
  body_t *ball = scene_get_body(scene, BEAVER_IDX);
  size_t body_count = scene_bodies(scene);

  // Add force creators with other bodies
  for (size_t i = 0; i < body_count; i++) {
    body_t *body = scene_get_body(scene, i);

    switch (get_type(body)) 
    {
      case BACKGROUND:
        if (index == GROUND_SCENE_INDEX)
        {
          create_earth_gravity(scene, G, ball);
          create_normal_force(scene, G, ball, body);
        }
        else if (index == WATER_SCENE_INDEX)
          create_buoyancy(scene, G, ball);
        break;
      case BRICK:
        create_physics_collision(scene, BRICK_ELASTICITY, ball, body);
        create_normal_force(scene, G, ball, body);
        break;
      case BRICK_TOP:
        create_normal_force(scene, G, ball, body);
        break;
      case DOOR:
        create_physics_collision(scene, BRICK_ELASTICITY, ball, body);
        create_normal_force(scene, G, ball, body);
        break;
      case BOARDER:
        create_normal_force(scene, G, ball, body);
        break;
      case DDL:
        create_remove_collision(scene, ball, body);
        break;
      case COIN:
        create_remove_collision(scene, ball, body);
        break;
      case BIRD:
        create_remove_collision(scene, ball, body);
        break;
      case TRASH:
        create_remove_collision(scene, ball, body);
        break;
      case FISH:
        create_remove_collision(scene, ball, body);
        break;
      case POWER_SLOW:
        create_slow_collision(scene, ball, body);
        break;
      case POWER_POINTS:
        create_double_points_collision(scene, ball, body);
        break;
      case POWER_LIVE:
        create_remove_collision(scene, ball, body);
        break;
      case POWER_MAGNET:
        create_magnet_collision(scene, ball, body);
        break;
    }
  }
}

/* create ground scene */
void create_ground_scene(scene_t *scene, list_t *loaded_surfaces, list_t *fonts)
{
  // Add water background and beaver to the scene
  add_background(scene, GROUND_WIDTH, GROUND_HEIGHT, GROUND_COLOR, GROUND_SURFACE_IDX);
  add_beaver(scene);
  
  // set surfaces for the images in scene
  if (loaded_surfaces != NULL)
  {
    scene_set_surfaces(scene, loaded_surfaces);
  }

  // set text in scene
  if (fonts != NULL)
  {
    scene_set_fonts(scene, fonts);

    list_t *font_index = list_init(LEVELS_NUM_FONTS, free);
    size_t *int_0 = malloc(sizeof(size_t));
    *int_0 = GROUND_FONT_SCORE_IDX;
    size_t *int_1 = malloc(sizeof(size_t));
    *int_1 = GROUND_FONT_LIVE_IDX;

    list_add(font_index, int_0);
    list_add(font_index, int_1);

    scene_set_font_indexs(scene, font_index);
  }

  // add coins, power-ups and enemies
  add_bricks_and_coins(scene);
  add_floor_coins(scene);
  add_ddls(scene);
  add_birds(scene);
  add_many_slow_downs(scene);
  add_many_double_points(scene);
  add_many_shields(scene);
  add_many_magnets(scene);
  
  //transition flag is the last to be add
  add_door(scene);
  add_forces(scene, GROUND_SCENE_INDEX);
}

/* create water scene */
void create_water_scene(scene_t *scene, list_t *loaded_surfaces, list_t *fonts)
{
  // Add water background and beave to the scene
  add_background(scene, SKY_WIDTH, SKY_HEIGHT, WATER_COLOR, WATER_SURFACE_IDX);
  add_beaver(scene);

  // set surfaces for the images in scene
  if (loaded_surfaces != NULL)
  {
    scene_set_surfaces(scene, loaded_surfaces);
  }

  // set text in scene
  if (fonts != NULL)
  {
    scene_set_fonts(scene, fonts);

    list_t *font_index = list_init(LEVELS_NUM_FONTS, free);
    size_t *int_2 = malloc(sizeof(size_t));
    *int_2 = WATER_FONT_SCORE_IDX;
    size_t *int_3 = malloc(sizeof(size_t));
    *int_3 = WATER_FONT_LIVE_IDX;

    list_add(font_index, int_2);
    list_add(font_index, int_3);

    scene_set_font_indexs(scene, font_index);
  }
  
  // add coins, power-ups and enemies
  add_many_fish(scene);
  add_random_coins(scene);
  add_ddls_non_ground(scene);
  add_many_slow_downs(scene);
  add_many_double_points(scene);
  add_many_shields(scene);
  add_many_magnets(scene);

  //transition flag is the last body to be added
  add_door(scene);
  add_forces(scene, WATER_SCENE_INDEX);
}


/* create sky scene*/
void create_sky_scene(scene_t *scene, list_t *loaded_surfaces, list_t *fonts)
{
  // Add sky background and beaver to the scene
  add_background(scene, SKY_WIDTH, SKY_HEIGHT, SKY_COLOR, SKY_SURFACE_IDX);
  add_beaver(scene);

  // set surfaces for the images in scene
  if (loaded_surfaces != NULL)
  {
    scene_set_surfaces(scene, loaded_surfaces);
  }

  // set text in scene
  if (fonts != NULL)
  {
    scene_set_fonts(scene, fonts);

    list_t *font_index = list_init(LEVELS_NUM_FONTS, free);
    size_t *int_4 = malloc(sizeof(size_t));
    *int_4 = SKY_FONT_SCORE_IDX;
    size_t *int_5 = malloc(sizeof(size_t));
    *int_5 = SKY_FONT_LIVE_IDX;

    list_add(font_index, int_4);
    list_add(font_index, int_5);

    scene_set_font_indexs(scene, font_index);
  }

  // add coins, power-ups and enemies
  add_many_trash(scene);
  add_random_coins(scene);
  add_ddls_non_ground(scene);
  add_many_slow_downs(scene);
  add_many_double_points(scene);
  add_many_shields(scene);

  //transition flag is the last body to be added
  add_door(scene);
  add_forces(scene, SKY_SCENE_INDEX);

}

/* create welcome page scene*/
void create_welcome_scene(scene_t *scene, list_t *loaded_surfaces, size_t surface_idx)
{
  // Add transition background to the scene
  add_background(scene, TRANSITION_WIDTH, TRANSITION_HEIGHT, GROUND_COLOR, surface_idx);

  // set surfaces for the images in scene
  if (loaded_surfaces != NULL)
  {
    scene_set_surfaces(scene, loaded_surfaces);
  }
}

/* create transition page scene*/ 
void create_transition_scene(scene_t *scene, list_t *loaded_surfaces, list_t *fonts, size_t scene_idx)
{

  // Add transition background to the scene
  add_background(scene, TRANSITION_WIDTH, TRANSITION_HEIGHT, GROUND_COLOR, scene_idx);
  // Add beaver to the scene to get the previous score
  add_beaver(scene);
  
  // set surfaces for the images in scene
  if (loaded_surfaces != NULL)
  {
    scene_set_surfaces(scene, loaded_surfaces);
  }

  // set text in scene
  if (fonts != NULL)
  {
    scene_set_fonts(scene, fonts);

    list_t *font_index = list_init(TRANSITION_NUM_FONTS, free);
    size_t *int_num = malloc(sizeof(size_t));
    if (scene_idx == TRANSITION1_SURFACE_INDEX || scene_idx == TRANSITION2_SURFACE_INDEX){
      *int_num = TRANSITION_FONT_SCORE_IDX;
    }
    else {
      *int_num = END_FONT_SCORE_IDX;
    }
    
    list_add(font_index, int_num);
    scene_set_font_indexs(scene, font_index);
  }
}


/* check if the current level is finished*/ 
bool finish_level(state_t *state)
{
  scene_t *scene = list_get(state->scenes, state->curr_scene);
  
  // check if current scene is not a level scene
  if (state->curr_scene == WELCOME_SCENE_INDEX || state->curr_scene % 2 == 1|| state->curr_scene == LOSE_SCENE_INDEX) {
    return true;
  }
  
  size_t body_count = scene_bodies(scene);
  
  if (body_get_centroid(scene_get_body(scene, body_count-1)).x < CENTER.x + BALL_RAD/2){
    state->curr_scene++;
    return true;
  }
  return false;
}

/* check if loses game*/ 
bool lose_game(state_t *state, scene_t *scene, size_t curr_scene) {
  return (!(curr_scene == WELCOME_SCENE_INDEX || curr_scene % 2 == 1)) && (body_get_lives(scene_get_body(scene, BEAVER_IDX)) == 0) && (curr_scene != 8);
}


void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  
  scene_t *scene = list_get(state->scenes, state->curr_scene);

  if (type == KEY_PRESSED) {
    switch (key) {
    case SPACE:
      if (finish_level(state))
      {
        state->curr_scene++;
        // check if all the levels are finished
        if (state->curr_scene >= NUM_SCENES-1)
        {
          // reset all scenes
          for (size_t i = 0; i<NUM_SCENES; i++)
          {
            scene_reset(list_get(state->scenes, i));
            scene_set_score(list_get(state->scenes, i), 0);
          }


          scene_t *ground = list_get(state->scenes, GROUND_SCENE_INDEX);
          create_ground_scene(ground, NULL, NULL);
          
          scene_t *scene_transition_1 = list_get(state->scenes, GROUND_SCENE_INDEX + 1);
          create_transition_scene(scene_transition_1, NULL, NULL, TRANSITION1_SURFACE_INDEX);

          scene_t *water = list_get(state->scenes, WATER_SCENE_INDEX);
          create_water_scene(water, NULL, NULL);

          scene_t *scene_transition_2 = list_get(state->scenes, TRANSITION2_SCENE_INDEX);
          create_transition_scene(scene_transition_2, NULL, NULL, TRANSITION2_SURFACE_INDEX);

          scene_t *sky = list_get(state->scenes, SKY_SCENE_INDEX);
          create_sky_scene(sky, NULL, NULL);

          scene_t *scene_end = list_get(state->scenes, END_SCENE_INDEX); 
          create_transition_scene(scene_end, NULL, NULL, END_SURFACE_INDEX);
          
          scene_t *lose = list_get(state->scenes, LOSE_SCENE_INDEX); 
          create_transition_scene(lose, NULL, NULL, LOSE_SURFACE_INDEX);

          state->curr_scene = 0;

          // clear the points
          state->total_points = 0;
          
        }
      }

      // hit space to jump
      else if (state->last_hit_space > TIME_BETWEEN_SPACE && state->curr_scene == GROUND_SCENE_INDEX)
      {
        state->last_hit_space = 0;
        body_t *beaver = scene_get_body(scene, BEAVER_IDX);
        body_add_velocity(beaver, (vector_t){0, BALL_SPEED});
      }
      
      break;
    
    case DOWN_ARROW:
      if (state->curr_scene == WATER_SCENE_INDEX || state->curr_scene == SKY_SCENE_INDEX)
      {
        body_t *beaver = scene_get_body(scene, BEAVER_IDX);
        body_set_velocity(beaver, (vector_t){0, -BALL_SPEED});
      }
      break;
    

    case UP_ARROW:
      if (state->curr_scene == SKY_SCENE_INDEX)
      {
        body_t *beaver = scene_get_body(scene, BEAVER_IDX);
        body_set_velocity(beaver, (vector_t){0, BALL_SPEED});
      }
      break;
    }
  } 

  // set the velocity of beaver to 0 if not hitting the keys
  else if (state->curr_scene == SKY_SCENE_INDEX)
  {
    body_t *beaver = scene_get_body(scene, BEAVER_IDX);
    body_set_velocity(beaver, VEC_ZERO);
  }
}

/* stop the beaver from going out of window*/
void wrap_around(state_t *state) 
{
  scene_t *scene = list_get(state->scenes, state->curr_scene);
  body_t *ball = scene_get_body(scene, BEAVER_IDX);
  vector_t centroid = body_get_centroid(ball);

  if (centroid.y < WRAP_DIS) {

    body_set_velocity(ball, VEC_ZERO);
    body_set_centroid(ball, (vector_t){centroid.x, WRAP_DIS});

  } else if (WINDOW.y - centroid.y < WRAP_DIS) {

    body_set_velocity(ball, VEC_ZERO);
    body_set_centroid(ball, (vector_t){centroid.x, WINDOW.y - WRAP_DIS});
  }
}

/* Slow down all the enemies*/ 
void scene_slow_enemy(scene_t *scene){
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (get_type(body) == DDL || get_type(body) == BIRD || get_type(body) == FISH || get_type(body) == TRASH){
      body_set_velocity(body, ENEMY_SLOW_VELOCITY);
    }
  }
}

/* Change the enemy speed to their original speed*/ 
void scene_fast_enemy(scene_t *scene){
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (get_type(body) == DDL || get_type(body) == BIRD || get_type(body) == FISH || get_type(body) == TRASH){
      body_set_velocity(body, DDL_SPEED);
    }
  }
}

/* create newtonian attraction between the beaver and coins within the distance*/
void scene_magnet(scene_t *scene){
  body_t *beaver = scene_get_body(scene, BEAVER_IDX);
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (get_type(body) == COIN){
      vector_t position = body_get_centroid(body);
      double dis = distance(position, body_get_centroid(beaver));
      if (dis < MAGNET_DISTANCE){
      create_newtonian_gravity(scene, g, beaver, body);
      }
    }
  }
}

state_t *emscripten_init(void) {
  srand(time(NULL));
  // Initialize scene
  vector_t min = VEC_ZERO;
  vector_t max = WINDOW;
  list_t *loaded_surfaces = list_init(NUM_SURFACES, (free_func_t) SDL_FreeSurface);

  list_t *fonts = list_init(NUM_FONTS, (free_func_t) free_text);
  
  sdl_init(min, max, loaded_surfaces, fonts);
  
  state_t *state = malloc(sizeof(state_t));
  state->last_hit_space = 1;

  state->scenes = list_init(NUM_SCENES, (free_func_t) scene_free);

  // create welcome scene
  scene_t *scene_welcome = scene_init();
  list_add(state->scenes, scene_welcome);
  create_welcome_scene(scene_welcome, loaded_surfaces, WELCOME_SURFACE_INDEX);

  // create gameplay scene
  scene_t *scene_gameplay = scene_init();
  list_add(state->scenes, scene_gameplay);
  create_welcome_scene(scene_gameplay, loaded_surfaces, GAMEPLAY_SURFACE_INDEX);

  // create ground scene
  scene_t *scene_ground = scene_init();
  list_add(state->scenes, scene_ground);
  create_ground_scene(scene_ground, loaded_surfaces, fonts);

  // create the first transition scene
  scene_t *scene_transition_1 = scene_init();
  list_add(state->scenes, scene_transition_1);
  create_transition_scene(scene_transition_1, loaded_surfaces, fonts, TRANSITION1_SURFACE_INDEX);

  // create water scene
  scene_t *scene_water = scene_init();
  list_add(state->scenes, scene_water);
  create_water_scene(scene_water, loaded_surfaces, fonts);

  // create the second transition scene
  scene_t *scene_transition_2= scene_init();
  list_add(state->scenes, scene_transition_2);
  create_transition_scene(scene_transition_2, loaded_surfaces, fonts, TRANSITION2_SURFACE_INDEX);

  // create the sky scene
  scene_t *scene_sky = scene_init();
  list_add(state->scenes, scene_sky);
  create_sky_scene(scene_sky, loaded_surfaces, fonts);

  // create endgame scene
  scene_t *scene_end = scene_init();
  list_add(state->scenes, scene_end);
  create_transition_scene(scene_end, loaded_surfaces, fonts, END_SURFACE_INDEX);
  
  // create lost game scene
  scene_t *scene_lose = scene_init();
  list_add(state->scenes, scene_lose);
  create_transition_scene(scene_lose, loaded_surfaces, fonts, LOSE_SURFACE_INDEX);

  // initialize state variables
  state->curr_scene = 0;
  state->background = GROUND;
  state->time_elapsed_slow = 0.0;
  state->time_elapsed_double = 0.0;
  state->state_slow = false;
  state->state_double = false;
  state->total_points = 0;

  return state;
}

void emscripten_main(state_t *state) {

  // get current scene
  scene_t *scene = list_get(state->scenes, state->curr_scene);
  sdl_on_key(on_key);

  // render transition scene 
  if (state->curr_scene == WELCOME_SCENE_INDEX || state->curr_scene % 2 == 1 || state->curr_scene == LOSE_SCENE_INDEX)
  {
    if (!sdl_is_done(state)) {
      sdl_render_scene(scene, 0.0);
    }
    finish_level(state);
  }

  // render level scenes
  else
  {
    body_t *beaver = scene_get_body(scene, BEAVER_IDX);
    double dt = time_since_last_tick();
    double curr_score = body_get_score(beaver);

    if (!sdl_is_done(state)) {
      state->last_hit_space += dt;
      // implement 10s of slow down power up
      if (scene_get_slow(scene)){
        if (state->state_slow == false){
          scene_slow_enemy(scene);
          state->state_slow = true;
        }
        state->time_elapsed_slow += dt;
      }
      if (state->time_elapsed_slow >= DOUBLE_POINTS){
        state->time_elapsed_slow = 0.0;
        scene_fast_enemy(scene);
        scene_set_slow(scene, false);
        body_set_slow(beaver, false);
        state->state_slow = false;
      }

      // implement 10s of double points power up
      if (body_get_double_points(beaver)){
        if (state->state_double == false){
          state->state_double = true;
        }
        body_add_score(beaver, DOUBLE_POINTS);
        state->time_elapsed_double += dt;
      }
      if (state->time_elapsed_double >= POWER_UP_TIME){
        state->time_elapsed_double = 0.0;
        state->state_double = false;
        body_set_double_points(beaver, false);
      }

      // implement 10s of double points power up
      if (body_get_magnet(beaver)){
        if (state->state_magnet == false){
          state->state_magnet = true;
        }
        state->time_elapsed_magnet += dt;
        scene_magnet(scene);
      }
      if (state->time_elapsed_magnet >= POWER_UP_TIME){
        state->time_elapsed_magnet = 0.0;
        state->state_magnet = false;
        body_set_magnet(beaver, false);
      }

      sdl_render_scene(scene, dt);
      wrap_around(state);
    }
    
    // print the current score of the beaver
    body_add_score(beaver, ONE_PT);
    double added_score = body_get_score(beaver) - curr_score;
    state->total_points += added_score;
    for (size_t i = state->curr_scene ; i < NUM_SCENES-1; i ++){
      scene_t *scene = list_get(state->scenes, i);
      scene_set_score(scene, state->total_points);
    }
    
    // check if the current level is finished and stop everything
    finish_level(state);

    // if lost game, skip to lost scene
    if (lose_game(state, list_get(state->scenes, state->curr_scene), state->curr_scene))
    {
      state->curr_scene = LOSE_SCENE_INDEX;
    }
  }
  
  sdl_show();
}

void emscripten_free(state_t *state) {
  list_free(state->scenes);
  free(state);
}
