#include "color.h"

rgb_color_t set_color(size_t cur_idx) {
  size_t idx;
  if (cur_idx < 8) {
    idx = cur_idx;
    return (rgb_color_t){.r = 0.0, .g = 1.0 - idx / 8.0, .b = 1.0};
  } else if (cur_idx >= 8 && cur_idx < 16) {
    idx = cur_idx - 8;
    return (rgb_color_t){.r = idx / 8.0, .g = 0.0, .b = 1.0};
  } else if (cur_idx >= 16 && cur_idx < 24) {
    idx = cur_idx - 16;
    return (rgb_color_t){.r = 1.0, .g = 0.0, .b = 1.0 - idx / 8.0};
  } else if (cur_idx >= 24 && cur_idx < 32) {
    idx = cur_idx - 24;
    return (rgb_color_t){.r = 1.0, .g = idx / 8.0, .b = 0.0};
  } else if (cur_idx >= 32 && cur_idx < 40) {
    idx = cur_idx - 32;
    return (rgb_color_t){.r = 1.0 - idx / 8.0, .g = 1.0, .b = 0.0};
  } else {
    idx = cur_idx - 40;
    return (rgb_color_t){.r = 0.0, .g = 1.0, .b = idx / 8.0};
  }
}

rgb_color_t set_color2(size_t idx) {

  double r;
  double g;
  double b;
  if (idx == 0 || idx == 1 || idx == 8 || idx == 9) {
    r = 1.0;
  } else if (idx == 2 || idx == 7) {
    r = 0.5;
  } else {
    r = 0.0;
  }
  if (idx == 1 || idx == 5) {
    g = 0.5;
  } else if (idx == 2 || idx == 3 || idx == 4) {
    g = 1.0;
  } else {
    g = 0.0;
  }
  if (idx == 9) {
    b = 0.5;
  } else if (idx <= 3) {
    b = 0.0;
  } else {
    b = 1.0;
  }
  return (rgb_color_t){r, g, b};
}
