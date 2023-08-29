#include "sdl_wrapper.h"
#include "state.h"
#include "body.h"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // for debugging
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;
const double REMOVE_X_POSITION = -100.0;
const double RENDER_INTERVAL = 50;
int WIDTH = 800;
int HEIGHT = 600;


/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
  int *width = malloc(sizeof(*width)), *height = malloc(sizeof(*height));
  assert(width != NULL);
  assert(height != NULL);
  SDL_GetWindowSize(window, width, height);
  vector_t dimensions = {.x = *width, .y = *height};
  free(width);
  free(height);
  return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
  // Scale scene so it fits entirely in the window
  double x_scale = window_center.x / max_diff.x,
         y_scale = window_center.y / max_diff.y;
  return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
  // Scale scene coordinates by the scaling factor
  // and map the center of the scene to the center of the window
  vector_t scene_center_offset = vec_subtract(scene_pos, center);
  double scale = get_scene_scale(window_center);
  vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
  vector_t pixel = {.x = round(window_center.x + pixel_center_offset.x),
                    // Flip y axis since positive y is down on the screen
                    .y = round(window_center.y - pixel_center_offset.y)};
  return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
  switch (key) {
  case SDLK_LEFT:
    return LEFT_ARROW;
  case SDLK_UP:
    return UP_ARROW;
  case SDLK_RIGHT:
    return RIGHT_ARROW;
  case SDLK_DOWN:
    return DOWN_ARROW;
  case SDLK_SPACE:
    return SPACE;
  default:
    // Only process 7-bit ASCII characters
    return key == (SDL_Keycode)(char)key ? key : '\0';
  }
}

/*Initializes all surfaces, music, fonts, and window*/
void sdl_init(vector_t min, vector_t max, list_t *loaded_surfaces, list_t *fonts) {
  int w = 10;
  int h = 20; 

  // Check parameters
  assert(min.x < max.x);
  assert(min.y < max.y);

  center = vec_multiply(0.5, vec_add(min, max));
  max_diff = vec_subtract(max, center);
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);


  //// Music initialization
  if(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) 
    fprintf(stderr, "\n failed to open audio \n"); 

  //// Image initialization
  SDL_Rect texr; texr.x = WIDTH/2; texr.y = HEIGHT/2; texr.w = w*2; texr.h = h*2; 

  IMG_Init(IMG_INIT_PNG);

  // create beaver surface
  SDL_Surface *surface =  IMG_Load( "assets/beaver.png");
  list_add(loaded_surfaces, surface);

  // create ground surface
  surface =  IMG_Load( "assets/ground.png");
  list_add(loaded_surfaces, surface);

  // create water surface
  surface =  IMG_Load( "assets/water.png");
  list_add(loaded_surfaces, surface);

  // create sky surface
  surface =  IMG_Load( "assets/space.png");
  list_add(loaded_surfaces, surface); 

  // create welcome page
  surface =  IMG_Load( "assets/welcome.png");
  list_add(loaded_surfaces, surface); 

  // create gameplay page
  surface =  IMG_Load( "assets/gameplay.png");
  list_add(loaded_surfaces, surface); 

  // create transition page for level 1
  surface =  IMG_Load( "assets/transition1.png");
  list_add(loaded_surfaces, surface); 

  // create transition page for level 2
  surface =  IMG_Load( "assets/transition2.png");
  list_add(loaded_surfaces, surface); 

  // create ending page
  surface =  IMG_Load( "assets/endgame.png");
  list_add(loaded_surfaces, surface); 

  // create lose game page
  surface =  IMG_Load( "assets/losegame.png");
  list_add(loaded_surfaces, surface); 


  // coin
  surface =  IMG_Load( "assets/coin.png");
  list_add(loaded_surfaces, surface); 

  // power-ups
  surface =  IMG_Load( "assets/boba.png");
  list_add(loaded_surfaces, surface); 
  surface =  IMG_Load( "assets/coffee.png");
  list_add(loaded_surfaces, surface); 
  surface =  IMG_Load( "assets/ice_cube.png");
  list_add(loaded_surfaces, surface); 
  surface =  IMG_Load( "assets/job_offer.png");
  list_add(loaded_surfaces, surface); 

  // enemies
  surface =  IMG_Load( "assets/crow.png");
  list_add(loaded_surfaces, surface);
  surface =  IMG_Load( "assets/deadline.png");
  list_add(loaded_surfaces, surface); 
  surface =  IMG_Load( "assets/shark.png");
  list_add(loaded_surfaces, surface); 
  surface =  IMG_Load( "assets/trash.png");
  list_add(loaded_surfaces, surface); 

  // Text initialization
  TTF_Init();

  // Text colors
  // black
  SDL_Color black = {0, 0, 0};
  // white
  SDL_Color white = {255, 255, 255};

  //Fonts for score and lives for ground
  TTF_Font* Sans = TTF_OpenFont("assets/OpenSans.ttf", 10);
  assert(Sans != NULL);
  
  text_t *text = text_init(150,50, (vector_t) {.x = 10, .y = 10},(void *)Sans, black);
  list_add(fonts, text);
  
  text = text_init(80,50,(vector_t) {.x = 10, .y = 50}, (void *)Sans, black);
  list_add(fonts, text);

  //Fonts for score and lives for water
  text = text_init(150,50,(vector_t) {.x = 10, .y = 10}, (void *)Sans, black);
  list_add(fonts, text);
  
  text = text_init(80,50,(vector_t) {.x = 10, .y = 50}, (void *)Sans, black);
  list_add(fonts, text);

  //Fonts for score and lives for sky
  text = text_init(150,50,(vector_t) {.x = 10, .y = 10}, (void *)Sans, white);
  list_add(fonts, text);
  
  text = text_init(80,50,(vector_t) {.x = 10, .y = 50}, (void *)Sans, white);
  list_add(fonts, text);
  
  //Fonts for score and lives for transition
  text = text_init(200,100,(vector_t) {.x = 210, .y = 180}, (void *)Sans, black);
  list_add(fonts, text);

  //Fonts for score and lives for endgame and lost game
  text = text_init(200,100,(vector_t) {.x = 430, .y = 250}, (void *)Sans, black);
  list_add(fonts, text);
}

bool sdl_is_done(state_t *state) {
  SDL_Event *event = malloc(sizeof(*event));
  assert(event != NULL);
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_QUIT:
      free(event);
      Mix_CloseAudio();
      return true;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      // Skip the keypress if no handler is configured
      // or an unrecognized key was pressed

      if (key_handler == NULL)
        break;
      char key = get_keycode(event->key.keysym.sym);
      if (key == '\0')
        break;

      uint32_t timestamp = event->key.timestamp;
      if (!event->key.repeat) {
        key_start_timestamp = timestamp;
      }
      key_event_type_t type =
          event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
      double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
      key_handler(key, type, held_time, state);
      break;
    }
  }
  free(event);
  return false;
}

void sdl_clear(void) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color) {
  // Check parameters
  size_t n = list_size(points);
  assert(n >= 3);
  assert(0 <= color.r && color.r <= 1);
  assert(0 <= color.g && color.g <= 1);
  assert(0 <= color.b && color.b <= 1);

  vector_t window_center = get_window_center();

  // Convert each vertex to a point on screen
  int16_t *x_points = malloc(sizeof(*x_points) * n),
          *y_points = malloc(sizeof(*y_points) * n);
  assert(x_points != NULL);
  assert(y_points != NULL);
  for (size_t i = 0; i < n; i++) {
    vector_t *vertex = list_get(points, i);
    vector_t pixel = get_window_position(*vertex, window_center);
    x_points[i] = pixel.x;
    y_points[i] = pixel.y;
  }

  // Draw polygon with the given color
  filledPolygonRGBA(renderer, x_points, y_points, n, color.r * 255,
                    color.g * 255, color.b * 255, 255);
  free(x_points);
  free(y_points);
}

void sdl_show(void) {
  // Draw boundary lines
  vector_t window_center = get_window_center();
  vector_t max = vec_add(center, max_diff),
           min = vec_subtract(center, max_diff);
  vector_t max_pixel = get_window_position(max, window_center),
           min_pixel = get_window_position(min, window_center);
  SDL_Rect *boundary = malloc(sizeof(*boundary));
  boundary->x = min_pixel.x;
  boundary->y = max_pixel.y;
  boundary->w = max_pixel.x - min_pixel.x;
  boundary->h = min_pixel.y - max_pixel.y;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  free(boundary);

  SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene, double dt) 
{

  sdl_clear();
  scene_tick(scene, dt);

  Mix_Music *music = NULL;
  music = Mix_LoadMUS("assets/life.wav");

  // If music is not playing, play music
  if ( !Mix_PlayingMusic())
  {  
    if (music == NULL)
      fprintf(stderr, "\n music path is empty\n");
    Mix_PlayMusic( music, -1);
  }

  // Free music chunks
  else if (music != NULL)
  {
    // clean up our resources
    Mix_FreeMusic(music);
    
  }

  int bodies = scene_bodies(scene);
  
  list_t *surfaces = scene_get_surfaces(scene);

// draw all the bodies
  for (int i = 0; i <bodies; i++) {
    // do not draw the beaver for transition level
    if (bodies == 2 && i == 1){
      break;
    }

    body_t *body = scene_get_body(scene, i);
    list_t *shape = body_get_shape(body);

    picture_t *picture = body_get_picture(body);
    vector_t pos = body_get_centroid(body);


    // If no picture data saved, render as polygon
    if (picture == NULL && (pos.x < WINDOW_WIDTH + RENDER_INTERVAL && pos.x > -RENDER_INTERVAL))
    {
      sdl_draw_polygon(shape, body_get_color(body));
    }

    //Rendering photos
    else if ((pos.x < WINDOW_WIDTH + RENDER_INTERVAL && pos.x > -RENDER_INTERVAL))
    {
      size_t pic_l = pic_length(picture);
      size_t pic_w = pic_width(picture);

      // the position of the picture is the position of the lower corner
      SDL_Rect img_container = {pos.x - pic_l/2, WINDOW_HEIGHT - (pos.y + pic_w/2), pic_l, pic_w};

      SDL_RenderCopy(renderer, SDL_CreateTextureFromSurface(renderer, list_get(surfaces, pic_index(picture))), NULL, &img_container);
	    
    }

    // delete body if out of scene
    vector_t add = body_get_centroid(body);
    if (add.x < REMOVE_X_POSITION)
    {
      body_remove(body);
    }

    list_free(shape);
  }


  // showing text for all scenes 
  if (scene_get_fonts(scene) != NULL)
  {

    for(size_t i = 0; i<list_size(scene_get_font_indexs(scene)); i++)
    {

      const char *context_string = malloc(sizeof(char)*20);

      if (i == 0)
      {
        // converting score from int to string
        sprintf(context_string, "Score : %zu", (int)scene_get_score(scene));

      }
      else
      {
        // converting lives from int to string
        sprintf(context_string, "Live: %zu", body_get_lives(scene_get_body(scene, 1)));

      }

      // rendering text
      text_t *text = (text_t*)list_get(scene_get_fonts(scene), *((size_t*)(list_get(scene_get_font_indexs(scene),i))));
      
      SDL_Surface* surfaceMessage =
          TTF_RenderText_Solid((TTF_Font*)text_get_font(text), context_string, (SDL_Color)text_get_color(text)); 

      if (surfaceMessage >=0 && renderer!=NULL)
      {
        // now you can convert it into a texture
        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

        assert(Message != NULL);
        
        SDL_Rect Message_rect; //create a rect
        vector_t text_center = text_get_center(text);
        Message_rect.x = text_center.x;  //controls the rect's x coordinate 
        Message_rect.y = text_center.y; // controls the rect's y coordinte

        Message_rect.w = text_get_length(text); // controls the width of the rect

        Message_rect.h = text_get_width(text); // controls the height of the rect

        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

        SDL_FreeSurface(surfaceMessage);
        SDL_DestroyTexture(Message);
      }

      free(context_string);
    }
  }
  sdl_show();
}


void sdl_on_key(key_handler_t handler) { key_handler = handler; }

double time_since_last_tick(void) {
  clock_t now = clock();
  double difference = last_clock
                          ? (double)(now - last_clock) / CLOCKS_PER_SEC
                          : 0.0; // return 0 the first time this is called
  last_clock = now;
  return difference;
}
