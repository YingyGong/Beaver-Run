#include "scene.h"
#include "body.h"
#include "list.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_ttf.h>


const size_t initial_num_bodies = 50;
const size_t initial_num_forces = 10;

// stores information for creating forces between bodies
typedef struct store_force_creator {
  force_creator_t forcer;
  void *aux;
  free_func_t freer;
  list_t *bodies;
} store_force_creator_t;


typedef struct scene {
  list_t *bodies;
  list_t *force_creators;
  double score;
  // stores all surfaces needed in game
  list_t *loaded_surfaces;
  // stores all fonts needed in game
  list_t *fonts;
  // stores index in fonts of text needed for specific scene
  list_t *font_indexs;
  bool slow_speed;
  bool have_double_points;
  double total_points;
} scene_t;

// stores information for kind of text needed for each scene
typedef struct text {
  size_t width;
  size_t length;
  vector_t center;
  void* font;
  SDL_Color color;
} text_t;

// initializes text struct with the font, size, and position
text_t *text_init (size_t length, size_t width, vector_t center, void* font, SDL_Color color)
{
  text_t* text = malloc(sizeof(text_t));
  text->length = length;
  text->width = width;
  text->center = center;
  text->font = font;
  text->color = color;
  return text;
}

// frees stext
void free_text(text_t *text)
{
  free(text->font);
  free(text);
}

// returns width of container for text
size_t text_get_width(text_t *text)
{
  return text->width;
}

// returns length of container for text
size_t text_get_length(text_t *text)
{
  return text->length;
}

// returns positino for text
vector_t text_get_center(text_t *text)
{
  return text->center;
}

// returns font type for text
void* text_get_font(text_t *text)
{
  return text->font;
}

// returns color of text
SDL_Color text_get_color(text_t *text)
{
  return text->color;
}

// sets container width for text
void text_set_width(text_t *text, size_t width)
{
  text->width = width;
}

// sets container length for text
void text_set_length(text_t *text, size_t length)
{
  text->length = length;
}

// sets text position on scene
void text_set_center(text_t *text, vector_t center)
{
  text->center = center;
}

// sets text font 
void text_set_font(text_t *text, void* font)
{
  text->font = font;
}

// sets text color
void text_set_color(text_t *text, SDL_Color color)
{
  text->color = color;
}

void list_freer(void *ptr) { list_free((list_t *)ptr); }

// frees forces struct
void force_creator_freer(void *ptr) {
  store_force_creator_t *fc = (store_force_creator_t *)ptr;
  // bodies are not owned by force, so should not be freed
  if (fc->freer != NULL) {
    fc->freer(fc->aux);
  } else {
    // free the "empty shell"
    list_free(fc->bodies);
  }
  free(fc);
}

// initializes scene
scene_t *scene_init(void) {
  scene_t *scene = malloc(sizeof(scene_t));
  assert(scene != NULL);
  scene->bodies = list_init(initial_num_bodies, (free_func_t)body_free);
  scene->force_creators = list_init(initial_num_forces, (free_func_t)force_creator_freer);
  scene->score = 0.0;
  return scene;
}

// frees variables in scene
void scene_free(scene_t *scene) {
  list_free(scene->force_creators);
  list_free(scene->bodies);
  list_free(scene->loaded_surfaces);
  list_free(scene->fonts);
  list_free(scene->font_indexs);
  free(scene);
}

// frees variables changed during game play to be initialized later
void scene_reset(scene_t *scene)
{
  // frees forces
  if (list_size(scene->force_creators) != 0)
  {
    list_free(scene->force_creators);
    scene->force_creators = list_init(initial_num_forces, (free_func_t)force_creator_freer);
  }

  // frees all body in scenes apart from welcome page and gameplay page
  if (scene_bodies(scene) > 1)
  {
    list_free(scene->bodies);
    scene->bodies = list_init(initial_num_bodies, (free_func_t)body_free);
  }

  // reset scene score
  scene->score = 0.0;
}

// returns number of bodies in scene
size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }


body_t *scene_get_body(scene_t *scene, size_t index) {
  // assert is done in list_get
  return list_get(scene->bodies, index);
}

// sets scene surface
void scene_set_surfaces(scene_t *scene, list_t *loaded_surfaces)
{
  scene->loaded_surfaces = loaded_surfaces;
}

// returns scene surface
list_t *scene_get_surfaces(scene_t *scene)
{
  return scene->loaded_surfaces;
}

// returns scene fonts
list_t *scene_get_fonts(scene_t *scene)
{
  return scene->fonts;
}

// return indexes of fonts needed in scene
list_t *scene_get_font_indexs(scene_t *scene)
{
  return scene->font_indexs;
}

// set scene fonts
void scene_set_fonts(scene_t *scene, list_t *fonts)
{
  scene->fonts = fonts;
}

// set scene font indexes
void scene_set_font_indexs(scene_t *scene, list_t *font_indexs)
{
  scene->font_indexs = font_indexs;
}

// adds a body to scene body list 
void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
}

double scene_get_score(scene_t *scene){
  return scene->score;
}

void scene_change_score(scene_t *scene, double score){
  scene->score += score;
}

void scene_set_score(scene_t *scene, double score){
  scene->score = score;
}

void scene_remove_body(scene_t *scene, size_t index) {
  // assertion is encapsultaed in list_remove
  body_t *toremove = scene_get_body(scene, index);
  body_remove(toremove);
}

void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer) {
  scene_add_bodies_force_creator(scene, forcer, aux, NULL, freer);
}

store_force_creator_t *force_creator_init(force_creator_t forcer, void *aux,
                                          list_t *bodies, free_func_t freer) {
  store_force_creator_t *fc = malloc(sizeof(store_force_creator_t));
  assert(fc != NULL);
  fc->forcer = forcer;
  fc->aux = aux;
  fc->freer = freer;
  fc->bodies = bodies;
  return fc;
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer) {
  // Create a struct to hold the force creator function and its auxiliary
  // value
  store_force_creator_t *fc = force_creator_init(forcer, aux, bodies, freer);
  // Add the force creator to the scene
  list_add(scene->force_creators, fc);
}

// mark a store_force_creator for removal
bool force_to_removed(store_force_creator_t *fc, body_t *body_removed) {
  list_t *bodies = fc->bodies;
  for (size_t k = 0; k < list_size(bodies); k++) {
    body_t *bodyf = list_get(bodies, k);
    if (bodyf == body_removed) {
      return 1;
    }
  }
  return 0;
}

bool scene_get_slow(scene_t *scene){
  return scene->slow_speed;
}

void scene_set_slow(scene_t *scene, bool slow_or_not){
  scene->slow_speed = slow_or_not;
}

void scene_slow_all_bodies(scene_t *scene){
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    vector_t velocity = body_get_velocity(body);
    body_set_velocity(body, vec_multiply(0.5, velocity));
  }
}

void scene_fast_all_bodies(scene_t *scene){
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    vector_t velocity = body_get_velocity(body);
    body_set_velocity(body, vec_multiply(2.0, velocity));
  }
}

void scene_original_speed(scene_t *scene, vector_t speed){
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    vector_t velocity = body_get_velocity(body);
    if (velocity.x != 0){
      body_set_velocity(body, speed);
    }
  }
}

bool scene_get_double_points(scene_t *scene){
  return scene->have_double_points;
}

void scene_set_double_points(scene_t *scene, bool result){
  scene->have_double_points = result;
}


void scene_tick(scene_t *scene, double dt) {
  // apply all forces
  for (size_t i = 0; i < list_size(scene->force_creators); i++) {
    store_force_creator_t *fc = list_get(scene->force_creators, i);
    force_creator_t forcer = fc->forcer;
    forcer(fc->aux);
  }

  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (body_get_slow(body) == true){
      scene->slow_speed = true;
      break;
    }
  }

  // body tick for the rest of bodies
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    body_tick(body, dt);
  }

  // remove force creators if associated bodies are removed
  for (size_t i = 0; i < scene_bodies(scene); i++) {
    body_t *body = scene_get_body(scene, i);
    if (body_is_removed(body)) {
      for (size_t j = 0; j < list_size(scene->force_creators); j++) {
        store_force_creator_t *fc = list_get(scene->force_creators, j);
        if (force_to_removed(fc, body)) {
          force_creator_t *removed = list_remove(scene->force_creators, j);
          force_creator_freer(removed);
          j--;
        }
      }
      // free body
      body_t *removed = list_remove(scene->bodies, i);
      double score = body_get_score(removed);
      scene_change_score(scene, score);
      body_free(removed);
      i--;
    }
  }
}

size_t scene_forcer_count(scene_t *scene) {
  return list_size(scene->force_creators);
}

void free_all_forcer(scene_t *scene) {
  for (size_t i = 0; i < list_size(scene->force_creators); i++) {
    store_force_creator_t *fc = list_get(scene->force_creators, i);
    free(fc);
  }
}
