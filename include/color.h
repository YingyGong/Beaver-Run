#ifndef __COLOR_H__
#define __COLOR_H__

#include <stddef.h>
/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
  float r;
  float g;
  float b;
} rgb_color_t;

/**
 * Set the color to display on the screen
 * @param cur_idx index to set
 * @return the color
 */
rgb_color_t set_color(size_t cur_idx);

/**
 * Set the color to display on the screen for breakout
 * @param cur_idx index to set
 * @return the color
 */
rgb_color_t set_color2(size_t idx);

#endif // #ifndef __COLOR_H__
