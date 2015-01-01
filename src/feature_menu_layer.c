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

// export PEBBLE_PHONE=192.168.42.152

#include "pebble.h"
#include "data.h"
#include "calculate.h"

#include "window_timer.h"

#define NUM_MENU_SECTIONS 1

////////////////////////////////////////////////////////////
// Data structure

#define NUM_ENTRIES 9

struct Configuration stData_g [NUM_ENTRIES] = {
  { "Soft - Samll",    "512m / 120g / 9°C", 512, 120, 9, 62},
  { "Soft - Medium",   "512m / 140g / 9°C", 512, 140, 9, 62},
  { "Soft - Large",    "512m / 160g / 9°C", 512, 160, 9, 62},
  { "Medium - Small",  "512m / 120g / 9°C", 512, 120, 9, 72},
  { "Medium - Medium", "512m / 140g / 9°C", 512, 140, 9, 72},
  { "Medium - Large",  "512m / 160g / 9°C", 512, 160, 9, 72},
  { "Hard - Small",    "512m / 120g / 9°C", 512, 120, 9, 82},
  { "Hard - Medium",   "512m / 140g / 9°C", 512, 140, 9, 82},
  { "Hard - Large",    "512m / 160g / 9°C", 512, 160, 9, 82},
};


// 500 = 30sec


////////////////////////////////////////////////////////////
// Menu window

static Window *menu_window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *menu_layer;

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_ENTRIES;

    default:
      return 0;
  }
}

// A callback is used to specify the height of the section header
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Saved Configs");
      break;

  }
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      // Use the row to specify which item we'll draw
      menu_cell_basic_draw(ctx, cell_layer, stData_g[cell_index->row].szTitle, stData_g[cell_index->row].szSubtitle, NULL);
      break;

  }
}

// Here we capture when a user selects a menu item
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  uint32_t u32Duration = 0;
  // Select section
  switch (cell_index->section) {
    case 0: // Section: Saved config
      u32Duration = calculate_in_ms(stData_g[cell_index->row]);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Run timer for %lu ms", u32Duration);

      window_timer_create(u32Duration);

      break; // Section: Saved config        
  }
}

// This initializes the menu upon window load
static void menu_window_load(Window *window) {
  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

static void menu_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
}

int main(void) {
  menu_window = window_create();

  window_set_window_handlers(menu_window, (WindowHandlers) {
    .load = menu_window_load,
    .unload = menu_window_unload,
  });

  window_stack_push(menu_window, true /* Animated */);

  app_event_loop();

  window_destroy(menu_window);
}
