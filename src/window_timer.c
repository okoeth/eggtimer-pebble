////////////////////////////////////////////////////////////
// Implementation of timer window module
// Main services include
//  - Construction
//  - Deconstruction
//  - Timer for final vibe
//  - Timer for update of timer screen
//

#include "pebble.h"

#include "window_timer.h"

////////////////////////////////////////////////////////////
// Timer window

#define UPDATE_CYCLE 1000

static Window *pwndTimer_g = NULL;
static Layer  *plyrTimer_g = NULL;
static uint32_t u32DurationMillies_g = 0;
static int32_t  i32Start_g = 0;
static AppTimer *ptmrFinal_g = NULL;
static AppTimer *ptmrUpdate_g = NULL;

////////////////////////////////////////////////////////////
// Check timer state

bool check_timer_state() {
  if (persist_exists(START_DATA_ID) && persist_exists(DURATION_DATA_ID)) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer state does exist");
    return true;
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer state does not exist");
    return false;
  }
} 

////////////////////////////////////////////////////////////
// Persist timer state

void write_timer_state (int32_t i32Start_p, uint32_t u32DurationMillies_p) {
  persist_write_data(START_DATA_ID, &i32Start_p, sizeof(int32_t));
  persist_write_data(DURATION_DATA_ID, &u32DurationMillies_p, sizeof(uint32_t));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Wrote timer state %lu (start) and %lu (duration)", i32Start_p, u32DurationMillies_p / 1000);
}

////////////////////////////////////////////////////////////
// Read timer state

void read_timer_state (int32_t* pi32Start_p, uint32_t* pu32DurationMillies_p) {
  persist_read_data(START_DATA_ID, pi32Start_p, sizeof(int32_t));
  persist_read_data(DURATION_DATA_ID, pu32DurationMillies_p, sizeof(uint32_t));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Read timer state %lu (start) and %lu (duration)", (* pi32Start_p), (* pu32DurationMillies_p) / 1000);
}

////////////////////////////////////////////////////////////
// Delete timer state

void delete_timer_state () {
  persist_delete(START_DATA_ID);
  persist_delete(DURATION_DATA_ID);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Deleted timer state");
}

////////////////////////////////////////////////////////////
// Call back for the timer stoppig the count down

static void app_timer_final_callback(void *pData_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: app_timer_final_callback");
  if (plyrTimer_g == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Bringing back timer window");
    window_timer_create();    
  }
  if (ptmrUpdate_g != NULL) {
    app_timer_cancel(ptmrUpdate_g);
    ptmrUpdate_g=NULL;
  }
  vibes_long_pulse();
  psleep(1000);
  vibes_long_pulse();
  delete_timer_state();
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
  int32_t i32Remaining = ((int32_t) (u32DurationMillies_g / 1000)) - (i32Now - i32Start_g);
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
  // Set final timer or terminate immediately
  int32_t i32Now = (int32_t) time(NULL);
  int32_t i32Remaining = ((int32_t) (u32DurationMillies_g / 1000)) - (i32Now - i32Start_g);
  if (i32Remaining > 1) {
    ptmrFinal_g = app_timer_register(i32Remaining * 1000, app_timer_final_callback, NULL);  
  } else {
    app_timer_final_callback(NULL);
  }
}

////////////////////////////////////////////////////////////
// This destrcuts the timer layer when window is unloaded

static void window_unload(Window *pwnd_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: window_unload");
  // Remove update timer, but keep final timer
  if (ptmrUpdate_g != NULL) {
    app_timer_cancel(ptmrUpdate_g);
    ptmrUpdate_g=NULL;
  }
  //if (ptmrFinal_g != NULL) {
  //  app_timer_cancel(ptmrFinal_g);
  //  ptmrFinal_g=NULL;
  //}
  if (plyrTimer_g != NULL) {
    layer_destroy(plyrTimer_g);
    plyrTimer_g=NULL;
  }

}

////////////////////////////////////////////////////////////
// Pre-create timer window

void window_timer_precreate(uint32_t u32DurationMillies_p) {
  // Set-up timer
  i32Start_g = (int32_t) time(NULL);
  u32DurationMillies_g = u32DurationMillies_p;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer started at second %lu for %lu seconds", i32Start_g, u32DurationMillies_g / 1000);
  write_timer_state(i32Start_g, u32DurationMillies_g);
  wakeup_schedule(i32Start_g + (u32DurationMillies_g / 1000), 0, false);
}

////////////////////////////////////////////////////////////
// Create timer window

void window_timer_create() {
  // Set-up timer
  i32Start_g = 0;
  u32DurationMillies_g = 0;
  read_timer_state(& i32Start_g, & u32DurationMillies_g);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer re-activated at second %lu for %lu seconds", i32Start_g, u32DurationMillies_g / 1000);

  // Set-up window
  if (u32DurationMillies_g != 0) {
    pwndTimer_g = window_create();
    window_set_window_handlers(pwndTimer_g, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
    window_stack_push(pwndTimer_g, true /* Animated */);
  }
}

