#ifndef __BODY_H__
#define __BODY_H__

#include "color.h"
#include "list.h"
#include "vector.h"
#include <math.h>
#include <stdbool.h>

/**
 * A rigid body constrained to the plane.
 * Implemented as a polygon with uniform density.
 * Bodies can accumulate forces and impulses during each tick.
 * Angular physics (i.e. torques) are not currently implemented.
 */
typedef struct body body_t;

/**
 * Info type that contains information about different types of bodies
 */
typedef struct info info_t;

/**
 * Picture type that contains the size and path of the pictures
 */
typedef struct picture picture_t;

/**
 * Allocates memory for an empty info.
 *
 * @return the new info
 */
info_t *info_init();

/**
 * Allocates memory for an empty picture type.
 *
 * @return the new picture
 */
picture_t *picture_init(size_t index, size_t length, size_t width);

/**
 * Returns info type 
 * 
 * @param info a pointer to a info returned from info_init()
 * @return type of given info
 */
size_t info_type(info_t *info);

/**
 * @brief Set the info type object
 * 
 * @param info a pointer to a info returned from info_init()
 * @param type type to be set for given info
 * @return * void 
 */
void set_info_type(info_t *info, size_t type);

/**
 * @brief get index of picture surface stored in scene laoded surfaces
 * 
 * @param picture 
 * @return * size_t index of picture in scene loaded surfaces
 */
size_t pic_index(picture_t *picture);

/**
 * Returns length of picture
 * 
 * @param picture a pointer to a picture returned from picture_init()
 * @return length of given picture
 */
size_t pic_length(picture_t *picture);

/**
 * Returns width of picture
 * 
 * @param picture a pointer to a picture returned from picture_init()
 * @return width of given picture
 */size_t pic_width(picture_t *picture);

/**
 * Initializes a body without any info.
 * Acts like body_init_with_info() where info and info_freer are NULL.
 * @param shape shape of body
 * @param mass mass of body
 * @param color color of body
 * @return * body_t*
 */
body_t *body_init(list_t *shape, double mass, rgb_color_t color);

 /**
  * Initializes a body without any info but with a given velocity.
  * Acts like body_init_with_info() where info and info_freer are NULL. 
  * 
  * @param shape shape of body
  * @param mass mass of body
  * @param color color of body
  * @param info info of body
  * @param info_freer function to info in body
  * @param initial_velocity sets body velocity
  * @return * body_t* 
  */
body_t *body_init_with_velocity(list_t *shape, double mass, rgb_color_t color, void *info, free_func_t info_freer, vector_t initial_velocity);

/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 * Asserts that the mass is positive and that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @param info additional information to associate with the body,
 *   e.g. its type if the scene has multiple types of bodies
 * @param info_freer if non-NULL, a function call on the info to free it
 * @return a pointer to the newly allocated body
 */
body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer, void *picture);

/**
 * Releases the memory allocated for a body.
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_free(body_t *body);

/**
 * Gets the current shape of a body.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
list_t *body_get_shape(body_t *body);

/**
 * Gets the current center of mass of a body.
 * While this could be calculated with polygon_centroid(), that becomes too slow
 * when this function is called thousands of times every tick.
 * Instead, the body should store its current centroid.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's center of mass
 */
vector_t body_get_centroid(body_t *body);

/**
 * Gets the current velocity of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the body's velocity vector
 */
vector_t body_get_velocity(body_t *body);

/**
 * Gets the mass of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the mass passed to body_init(), which must be greater than 0
 */
double body_get_mass(body_t *body);

/**
 * Gets the friction coefficient of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the friction coefficient passed to body_init()
 */
double body_get_mu(body_t *body);

/**
 * Gets the display color of a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the color passed to body_init(), as an (R, G, B) tuple
 */
rgb_color_t body_get_color(body_t *body);

/**
 * Gets the information associated with a body.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the info passed to body_init()
 */
void *body_get_info(body_t *body);

/**
 * Translates a body to a new position.
 * The position is specified by the position of the body's center of mass.
 *
 * @param body a pointer to a body returned from body_init()
 * @param x the body's new centroid
 */
void body_set_centroid(body_t *body, vector_t x);

picture_t *body_get_picture(body_t *body);
/**
 * Changes a body's velocity (the time-derivative of its position).
 *
 * @param body a pointer to a body returned from body_init()
 * @param v the body's new velocity
 */
void body_set_velocity(body_t *body, vector_t v);

void body_set_friction_c(body_t *body, double friction_c);

/**
 * Changes a body's orientation in the plane.
 * The body is rotated about its center of mass.
 * Note that the angle is *absolute*, not relative to the current orientation.
 *
 * @param body a pointer to a body returned from body_init()
 * @param angle the body's new angle in radians. Positive is counterclockwise.
 */
void body_set_rotation(body_t *body, double angle);

/**
 * Changes a body's score
 *
 * @param body a pointer to a body returned from body_init()
 * @param score the score that beaver will get when the body is removed
 */
void body_set_score(body_t *body, double score);

/**
 * Gets the score when a body is removed
 *
 * @param body a pointer to a body returned from body_init()
 * @return the score
 */
double body_get_score(body_t *body);

/**
 * Adds score to the body's current score
 * @param body a pointer to a body returned from body_init()
 */
void body_add_score(body_t *body, double score);

/**
 * Changes a body's remaining lives
 *
 * @param body a pointer to a body returned from body_init()
 * @param remaining_lives the remaining number of lives of the body
 */
void body_set_lives(body_t *body, size_t remaining_lives);

/**
 * Gets the remaining lives of the beaver
 *
 * @param body a pointer to a body returned from body_init()
 * @return the remaining lives
 */
size_t body_get_lives(body_t *body);

void body_add_lives(body_t *body, size_t added_lives);

/**
 * Reduce the beaver live by 1
 * @param body a pointer to a body returned from body_init()
 */
void body_reduce_live(body_t *body);


/**
 * Changes a body's score to be the negative of its original
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_negate_score(body_t *body);

/**
 * Applies a force to a body over the current tick.
 * If multiple forces are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param force the force vector to apply
 */
void body_add_force(body_t *body, vector_t force);

/**
 * Applies an impulse to a body.
 * An impulse causes an instantaneous change in velocity,
 * which is useful for modeling collisions.
 * If multiple impulses are applied in the same tick, they should be added.
 * Should not change the body's position or velocity; see body_tick().
 *
 * @param body a pointer to a body returned from body_init()
 * @param impulse the impulse vector to apply
 */
void body_add_impulse(body_t *body, vector_t impulse);

/**
 * Updates the body after a given time interval has elapsed.
 * Sets acceleration and velocity according to the forces and impulses
 * applied to the body during the tick.
 * The body should be translated at the *average* of the velocities before
 * and after the tick.
 * Resets the forces and impulses accumulated on the body.
 *
 * @param body the body to tick
 * @param dt the number of seconds elapsed since the last tick
 */
void body_tick(body_t *body, double dt);

/**
 * Marks a body for removal--future calls to body_is_removed() will return true.
 * Does not free the body.
 * If the body is already marked for removal, does nothing.
 *
 * @param body the body to mark for removal
 */
void body_remove(body_t *body);

/**
 * Returns whether a body has been marked for removal.
 * This function returns false until body_remove() is called on the body,
 * and returns true afterwards.
 *
 * @param body the body to check
 * @return whether body_remove() has been called on the body
 */
bool body_is_removed(body_t *body);

/**
 * Gets the force of a body
 *
 * @param body the body to check
 * @return the force
 */
vector_t body_get_force(body_t *body);

/**
 * Gets the impulse of a body
 *
 * @param body the body to check
 * @return the impulse
 */
vector_t body_get_impulse(body_t *body);

/**
 * Add a certain velocity to the current velocity of the body
 *
 * @param body the body to check
 */
void body_add_velocity(body_t *body, vector_t v);

/**
 * Set whether the body should slow down nearby bodies
 *
 * @param body the body to set the indicator of slow
 */
void body_set_slow(body_t *body, bool true_or_false);

/**
 * Get whether the body should slow down nearby bodies
 *
 * @param body the body to set the indicator for slow
 * @return the indicator for slow
 */
bool body_get_slow(body_t *body);

/**
 * Set whether the body should earn double point
 * 
 * @param body the body to set the indicator for double points
 */
void body_set_double_points(body_t *body, bool true_or_false);

/**
 * Get whether the body should earn double point
 *
 * @param body the body to set the indicator for double points
 * @return the indicator for double points
 */
bool body_get_double_points(body_t *body);

/**
 * Set whether the body should have magnetism to nearby objects
 * 
 * @param body the body to set the indicator for magnetism
 */
void body_set_magnet(body_t *body, bool true_or_false);

/**
 * Get whether the body should have magnetism to nearby objects
 * 
 * @param body the body to set the indicator for magnetism
 * @return the indicator for magnetism
 */
bool body_get_magnet(body_t *body);

#endif // #ifndef __BODY_H__
