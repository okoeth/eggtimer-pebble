////////////////////////////////////////////////////////////
// Implementation of pop window module
// Main services include
//  - Construction
//  - Deconstruction
//  - Confirmation of message
//

// TODO: Implement other options than POP and OK

#include "pebble.h"
#include "window_pop.h"

#define POP_DURATION 3000

////////////////////////////////////////////////////////////
// Pop window

static Window *pwndPop_g;
static TextLayer *plyrMessage_g;
static char * pszMessage_g = NULL;
static int16_t i16Mode_g = 0;
static AppTimer *ptmrPop_g = NULL;

////////////////////////////////////////////////////////////
// Call back for the timer destroying the window

static void app_timer_pop_callback(void *pData_p) {
  window_stack_pop(true);
}

////////////////////////////////////////////////////////////
// This constructs the timer layer when window is loaded

// 16.2. - 20.2.

static void window_load(Window *pwnd_p) {
  Layer *plyrRoot = window_get_root_layer(pwnd_p);
  GRect rectBounds = layer_get_frame(plyrRoot);
  plyrMessage_g = text_layer_create(rectBounds);
  text_layer_set_font(plyrMessage_g, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_overflow_mode(plyrMessage_g, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(plyrMessage_g, GTextAlignmentCenter);
  text_layer_set_text(plyrMessage_g, pszMessage_g);
  layer_add_child(plyrRoot, text_layer_get_layer(plyrMessage_g));
  vibes_short_pulse();
}

////////////////////////////////////////////////////////////
// This destrcuts the timer layer when window is unloaded

static void window_unload(Window *pwnd_p) {
  if (plyrMessage_g != NULL) {
    text_layer_destroy(plyrMessage_g);
    plyrMessage_g=NULL;
  }
}

////////////////////////////////////////////////////////////
// Create pop window

void window_pop_create(int16_t i16Mode_p, char * pszMessage_p) {
  i16Mode_g = i16Mode_p;
  pszMessage_g = pszMessage_p;
  pwndPop_g = window_create();
  window_set_window_handlers(pwndPop_g, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(pwndPop_g, true /* Animated */);  
  if (i16Mode_g==MODE_POP) {
    ptmrPop_g = app_timer_register(POP_DURATION, app_timer_pop_callback, NULL);  
  }
}

////////////////////////////////////////////////////////////
// Destroy pop window

void window_pop_destroy() {
  if (pwndPop_g!=NULL) {
  	 window_destroy(pwndPop_g);
  	 pwndPop_g=NULL;
  }
}

