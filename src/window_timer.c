////////////////////////////////////////////////////////////
// Implementation of timer window module
// Main services include
//  - Construction
//  - Deconstruction
//  - Timer for final vibe
//  - Timer for update of timer screen
//

#include "pebble.h"

////////////////////////////////////////////////////////////
// Timer window

#define UPDATE_CYCLE 1000

static Window *pwndTimer_g = NULL;
static Layer  *plyrTimer_g = NULL;
static uint32_t u32DurationMillies_g = 0;
static int32_t  i32Start = 0;
static AppTimer *ptmrFinal_g = NULL;
static AppTimer *ptmrUpdate_g = NULL;

////////////////////////////////////////////////////////////
// Call back for the timer stoppig the count down

static void app_timer_final_callback(void *pData_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: app_timer_final_callback");
  if (ptmrUpdate_g != NULL) {
    app_timer_cancel(ptmrUpdate_g);
    ptmrUpdate_g=NULL;
  }
  vibes_long_pulse();
  layer_mark_dirty(plyrTimer_g);
}

////////////////////////////////////////////////////////////
// Call back for the timer updating the text

static void app_timer_update_callback(void *pData_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: app_timer_update_callback");
  layer_mark_dirty(plyrTimer_g);
  ptmrUpdate_g = app_timer_register(UPDATE_CYCLE, app_timer_update_callback, NULL);  
}

////////////////////////////////////////////////////////////
// Create current time string

static void create_time_string(char * psz_p) {
  int32_t i32Now = (int32_t) time(NULL);
  int32_t i32Remaining = ((int32_t) (u32DurationMillies_g / 1000)) - (i32Now - i32Start);
  int32_t i32Minutes = 0;
  int32_t i32Seconds = 0;
  if (i32Remaining > 0) {
    i32Minutes = i32Remaining / 60;
    i32Seconds = i32Remaining - (i32Minutes * 60);
  }
  snprintf(psz_p, 32, "%02ld:%02ld", i32Minutes, i32Seconds);  
}

////////////////////////////////////////////////////////////
// Update the screen of the timer

static void update_layer_callback(Layer *plyr_p, GContext* pctx_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: update_timer_layer_callback");
  graphics_context_set_text_color(pctx_p, GColorBlack);
  GRect rectBounds = layer_get_frame(plyr_p);
  char szTimeString[32] = "";
  create_time_string(szTimeString);
  graphics_draw_text(
      pctx_p,
      szTimeString,
      fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS),
      GRect(15, (144 / 2)-26, rectBounds.size.w, 42),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
}

////////////////////////////////////////////////////////////
// This constructs the timer layer when window is loaded

static void window_load(Window *pwnd_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: window_load");
  Layer *plyrRoot = window_get_root_layer(pwnd_p);
  GRect rectBounds = layer_get_frame(plyrRoot);
  plyrTimer_g = layer_create(rectBounds);
  layer_set_update_proc(plyrTimer_g, update_layer_callback);
  layer_add_child(plyrRoot, plyrTimer_g);
  ptmrUpdate_g = app_timer_register(UPDATE_CYCLE, app_timer_update_callback, NULL);  
  ptmrFinal_g = app_timer_register(u32DurationMillies_g, app_timer_final_callback, NULL);  
}

////////////////////////////////////////////////////////////
// This destrcuts the timer layer when window is unloaded

static void window_unload(Window *pwnd_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: window_unload");
  if (ptmrUpdate_g != NULL) {
    app_timer_cancel(ptmrUpdate_g);
    ptmrUpdate_g=NULL;
  }
  if (ptmrFinal_g != NULL) {
    app_timer_cancel(ptmrFinal_g);
    ptmrFinal_g=NULL;
  }
  if (plyrTimer_g != NULL) {
    layer_destroy(plyrTimer_g);
    plyrTimer_g=NULL;
  }

}


////////////////////////////////////////////////////////////
// Create timer window

void window_timer_create(uint32_t u32DurationMillies_p) {
  // Set-up timer
  i32Start = (int32_t) time(NULL);
  u32DurationMillies_g = u32DurationMillies_p;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer started at second %lu for %lu seconds", i32Start, u32DurationMillies_g / 1000);

  // Set-up window
  pwndTimer_g = window_create();
  window_set_window_handlers(pwndTimer_g, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(pwndTimer_g, true /* Animated */);
}

