////////////////////////////////////////////////////////////
// Egg timer based on the following formula of 
// Charles D. H. Williams
//
// duration = 0,465 * weight ^ 2/3 * ln (0,76 * ( (t_water - t_start) / (t_water - t_end) ) )
//
// t_water = 100 °C - (1 °C * height / 285m)
// t_start =   4 °C (freezer) ... 20 °C (room temperature)
// t_end   =  62 °C (soft)    ... 82 °C (hard)
//

// TODO: Rename to main.c

// TODO: Functions
// - Pop window
// - Data CRUD
// - Persistence
//

// export PEBBLE_PHONE=192.168.42.152

#include "pebble.h"
#include "data.h"

#include "window_menu.h"

////////////////////////////////////////////////////////////
// Data structure

#define NUM_ENTRIES 9

static struct Configuration rgstConfig_g [NUM_ENTRIES] = {
  { "Soft - Small",    "512m / 120g / 9C", 512, 120, 9, 62},
  { "Soft - Medium",   "512m / 140g / 9C", 512, 140, 9, 62},
  { "Soft - Large",    "512m / 160g / 9C", 512, 160, 9, 62},
  { "Medium - Small",  "512m / 120g / 9C", 512, 120, 9, 72},
  { "Medium - Medium", "512m / 140g / 9C", 512, 140, 9, 72},
  { "Medium - Large",  "512m / 160g / 9C", 512, 160, 9, 72},
  { "Hard - Small",    "512m / 120g / 9C", 512, 120, 9, 82},
  { "Hard - Medium",   "512m / 140g / 9C", 512, 140, 9, 82},
  { "Hard - Large",    "512m / 160g / 9C", 512, 160, 9, 82},
};

int main(void) {
  struct Data * pstData = data_create();
  data_append_items(pstData, rgstConfig_g, NUM_ENTRIES);  
  window_menu_create(pstData);
  app_event_loop();
  data_destroy(pstData);
  window_menu_destroy();
}
