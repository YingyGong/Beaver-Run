#ifndef __SCENE_H__
#define __SCENE_H__

#include "body.h"
#include "list.h"
#include <SDL2/SDL.h>



/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store
 * arbitrarily many bodies and force creators.
 */
typedef struct scene scene_t;

/**
 * A collection of information for to create text in scene
 * Includes text font, container length and width, and color
 */
typedef struct text text_t;

/**
 * Allocates memory for an empty text.
 *
 * @return the new text
 */
text_t *text_init (size_t length, size_t width, vector_t center, void* font, SDL_Color color);

/**
 * Releases memory allocated for a given text
 *
 * @param text a pointer to a text returned from text_init()
 */
void free_text(text_t *text);

 /**
  * @brief Returns width of given text's container
  * 
  * @param text a pointer to a text returned from text_init()
  * @return * size_t width of text
  */
size_t text_get_width(text_t *text);


 /**
  * @brief Returns length of given text's container
  * 
  * @param text a pointer to a text returned from text_init()
  * @return * size_t length of text
  */
size_t text_get_length(text_t *text);


 /**
  * @brief Returns position of given text's container
  * 
  * @param text a pointer to a text returned from text_init()
  * @return * vector_t position of text
  */
vector_t text_get_center(text_t *text);

 /**
  * Returns font of given text
  * 
  * @param text a pointer to a text returned from text_init()
  * @return * void* font fo given text
  */
void* text_get_font(text_t *text);

 /**
  * Returns color of given text
  * 
  * @param text a pointer to a text returned from text_init()
  * @return * SDL_Color color of given text
  */
SDL_Color text_get_color(text_t *text);

 /**
  * Sets width of given text's container
  * 
  * @param text a pointer to a text returned from text_init()
  * @param width width of text
  * @return * void 
  */
void text_set_width(text_t *text, size_t width);

 /**
  * Sets length of given text's container
  * 
  * @param text a pointer to a text returned from text_init()
  * @param length length of text
  * @return * void 
  */
void text_set_length(text_t *text, size_t length);

 /**
  * Sets center of given text's container
  * 
  * @param text a pointer to a text returned from text_init()
  * @param center position of text
  * @return * void 
  */

void text_set_center(text_t *text, vector_t center);

 /**
  * Sets font of given text
  * 
  * @param text a pointer to a text returned from text_init()
  * @param font font of text
  * @return * void 
  */
void text_set_font(text_t *text, void* font);

 /**
  * Sets color of given text
  * 
  * @param text a color to a text returned from text_init()
  * @param color color of text
  * @return * void 
  */
void text_set_color(text_t *text, SDL_Color color);

/**
 * A function which adds some forces or impulses to bodies,
 * e.g. from collisions, gravity, or spring forces.
 * Takes in an auxiliary value that can store parameters or state.
 */
typedef void (*force_creator_t)(void *aux);

/**
 * Allocates memory for an empty scene.
 * Makes a reasonable guess of the number of bodies to allocate space for.
 * Asserts that the required memory is successfully allocated.
 *
 * @return the new scene
 */
scene_t *scene_init();

/**
 * Returns list of surfaces needed in all scenes
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @return * list_t* list of surfaces needed for scenes
 */
list_t *scene_get_surfaces(scene_t *scene);

/**
 * Sets list of surfaces needed in all scenes 
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @param loaded_surfaces list of surfaces needed for scenes
 * @return * void 
 */
void scene_set_surfaces(scene_t *scene, list_t *loaded_surfaces);

/**
 *returns list of texts needed in all scenes
**/

/**
 * Returns ist of fonts needed in the scenes
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @return * list_t* list of fonts needed in the scenes
 */

list_t *scene_get_fonts(scene_t *scene);

/**
 * Returns list of indicies for which texts in fonts is needed in the given scene 
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @return * list_t* list of indicies for which texts in fonts is needed in the given scene 
 */
list_t *scene_get_font_indexs(scene_t *scene);

/**
 * sets list of fonts in given scene
**/

/**
 * Sets list of fonts in given scene 
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @param font list of fonts needed for scenes
 * @return * void 
 */
void scene_set_fonts(scene_t *scene, list_t *font);

/**
 * Sets list of font indexs in given scene 
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @param font_indexs list of font indexs for the given scene 
 * @return * void 
 */
void scene_set_font_indexs(scene_t *scene, list_t *font_indexs);


/**
 * Releases memory allocated for a given scene
 * and all the bodies and force creators it contains.
 *
 * @param scene a pointer to a scene returned from scene_init()
 */
void scene_free(scene_t *scene);

/**
 * Releases memory of scene bodies and forces, which changes when game resets.  
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @return * void 
 */
void scene_reset(scene_t *scene);

/**
 * Gets the number of bodies in a given scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @return the number of bodies added with scene_add_body()
 */
size_t scene_bodies(scene_t *scene);

/**
 * Gets the body at a given index in a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 * @return a pointer to the body at the given index
 */
body_t *scene_get_body(scene_t *scene, size_t index);

/**
 * Adds a body to a scene.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param body a pointer to the body to add to the scene
 */
void scene_add_body(scene_t *scene, body_t *body);

/**
 * Returns accumulative score of scenes
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @return score of given scene
 */
double scene_get_score(scene_t *scene);

/**
 * Add/Minus scores of a scene.
*/
void scene_change_score(scene_t *scene, double score);

/**
 * Sets score in a scene
 * 
 * @param scene a pointer to a scene returned from scene_init()
 * @param score score to store in given scene
 * @return * void 
 */
void scene_set_score(scene_t *scene, double score);

/**
 * @deprecated Use body_remove() instead
 *
 * Removes and frees the body at a given index from a scene.
 * Asserts that the index is valid.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param index the index of the body in the scene (starting at 0)
 */
void scene_remove_body(scene_t *scene, size_t index);

/**
 * @deprecated Use scene_add_bodies_force_creator() instead
 * so the scene knows which bodies the force creator depends on
 */
void scene_add_force_creator(scene_t *scene, force_creator_t forcer, void *aux,
                             free_func_t freer);

/**
 * Adds a force creator to a scene,
 * to be invoked every time scene_tick() is called.
 * The auxiliary value is passed to the force creator each time it is called.
 * The force creator is registered with a list of bodies it applies to,
 * so it can be removed when any one of the bodies is removed.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param forcer a force creator function
 * @param aux an auxiliary value to pass to forcer when it is called
 * @param bodies the list of bodies affected by the force creator.
 *   The force creator will be removed if any of these bodies are removed.
 *   This list does not own the bodies, so its freer should be NULL.
 * @param freer if non-NULL, a function to call in order to free aux
 */
void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies,
                                    free_func_t freer);

/**
 * Executes a tick of a given scene over a small time interval.
 * This requires executing all the force creators
 * and then ticking each body (see body_tick()).
 * If any bodies are marked for removal, they should be removed from the scene
 * and freed, along with any force creators acting on them.
 *
 * @param scene a pointer to a scene returned from scene_init()
 * @param dt the time elapsed since the last tick, in seconds
 */
void scene_tick(scene_t *scene, double dt);

/**
 * Count the number of forcers in the scene
 *
 * @param scene the scene to free forcers
 * @return number of forcers
 */
size_t scene_forcer_count(scene_t *scene);

/**
 * Free all forcers in the scene
 *
 * @param scene the scene to free forcers
 */
void free_all_forcer(scene_t *scene);

/**
 * Get whether the beaver should slow down other objects in this scene
 *
 * @param scene the scene to check
 * @return indicator of slow down for scene
 */
bool scene_get_slow(scene_t *scene);

/**
 * Set whether the beaver should slow down other objects in this scene
 *
 * @param scene the scene to check
 * @param slow_or_not indicator of slow down for scene
 */
void scene_set_slow(scene_t *scene, bool slow_or_not);

/**
 * Half the speed of all objects in the scene
 *
 * @param scene the scene to change speed
 */
void scene_slow_all_bodies(scene_t *scene);

/**
 * Double the speed of all objects in the scene
 *
 * @param scene the scene to change speed
 */
void scene_fast_all_bodies(scene_t *scene);

/**
 * Change the speed of all objects to the original speed
 *
 * @param scene the scene to change speed
 * @param speed the speed to reach
 */
void scene_original_speed(scene_t *scene, vector_t speed);

/**
 * Get whether the beaver should earn double points in this scene
 *
 * @param scene the scene to check
 * @return indicator of double points for scene
 */
bool scene_get_double_points(scene_t *scene);

/**
 * Set whether the beaver should earn double points in this scene
 *
 * @param scene the scene to check
 * @param result indicator of double points for scene
 */
void scene_set_double_points(scene_t *scene, bool result);

#endif // #ifndef __SCENE_H__
