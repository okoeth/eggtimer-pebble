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

// export PEBBLE_PHONE=192.168.42.152

#include "pebble.h"

// LN of 1.20 => 4.40 in steps of 0.1 times 1000
uint32_t lookup1 [] = {
  182,
  262,
  336,
  405,
  470,
  531,
  588,
  642,
  693,
  742,
  788,
  833,
  875,
  916,
  956,
  993,
  1030,
  1065,
  1099,
  1131,
  1163,
  1194,
  1224,
  1253,
  1281,
  1308,
  1335,
  1361,
  1386,
  1411,
  1435,
  1459,
  1482
};

// x^3/2 of 80 => 200
uint32_t lookup2 [] = {
  19,
  20,
  22,
  23,
  24,
  26,
  27,
  28,
  29,
  31,
  32,
  33,
  34
};

#define NUM_MENU_SECTIONS 2
#define NUM_MENU_ICONS 3
#define NUM_FIRST_MENU_ITEMS 1
#define NUM_SECOND_MENU_ITEMS 2

////////////////////////////////////////////////////////////
// Data structure

struct configuration {
  char     title[32]; 
  char     subtitle[32];
  uint32_t  height;   //  m, Maisach: 512m
  uint32_t  weight;   //  g, S/M/L: 120g/140g/160g 
  uint32_t  t_start;  // °C, 4°C 
  uint32_t  t_end;    // °C, S/M/H: 62°C/72°C/82°C
};

struct configuration data [9] = {
  { "Soft",   "512m / 120g / 7°C", 512, 120, 7, 62},
  { "Soft",   "512m / 140g / 7°C", 512, 140, 7, 62},
  { "Soft",   "512m / 160g / 7°C", 512, 160, 7, 62},
  { "Medium", "512m / 120g / 7°C", 512, 120, 7, 72},
  { "Medium", "512m / 140g / 7°C", 512, 140, 7, 72},
  { "Medium", "512m / 160g / 7°C", 512, 160, 7, 72},
  { "Hard",   "512m / 120g / 7°C", 512, 120, 7, 82},
  { "Hard",   "512m / 140g / 7°C", 512, 140, 7, 82},
  { "Hard",   "512m / 160g / 7°C", 512, 160, 7, 82},
};

////////////////////////////////////////////////////////////
// Calculation

uint32_t calculate_in_min_times_1000(struct configuration data_p) {
  // Part   I: p1 = ((t_water - t_start) * 100 ) / (t_water - t_end)
  uint32_t p1_1 = (100 * 100 - ((data_p.height * 100) / 285) ) - (data_p.t_start * 100);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p1_1 %lu", p1_1);
  uint32_t p1 = p1_1 * 100 / ((100 * 100 - ((data_p.height * 100) / 285) ) - (data_p.t_end * 100) ); 
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p1 %lu", p1);

  // Part  II: p2 = (0,76 * 100 * p1) 
  uint32_t p2_1 = (760 * p1) / 10000; 
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p2_1 %lu", p2_1);
  uint32_t p2 = 0;
  if (p2_1 < 12) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub p2_1 %lu", p2_1);
    p2 = 12-12;
  } else if (p2_1 > 44) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub p2_1 %lu", p2_1);
    p2 = 44-12;
  } else {
    p2 = p2_1 - 12;
  }
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p2 %lu", p2);

  // Part III: p3 = lookup1(p2)  
  uint32_t p3 = lookup1[p2];
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p3 %lu", p3);

  // Part  IV: p4 = lookup2( (weight - 80) / 10)
  uint32_t p4_1 = (data_p.weight / 10);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p4_1 %lu", p4_1);

  uint32_t p4_2 = 0;
  if (p4_1 < 8) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub p4_1 %lu below lowest lookup", p4_1);
    p4_2 = 8-8;
  } else if (p2_1 > 44) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub p4_1 %lu above highest lookup", p4_1);
    p4_2 = 20-8;
  } else {
    p4_2 = p4_1 - 8;
  }
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p4_2 %lu", p4_2);

  uint32_t p4 = lookup2[p4_2];
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p4 %lu", p4);

  // Part   V: p5 = 0,465 * 100 * p4 * p3 / 1000
  uint32_t p5 = (465 * p4 * p3) / 1000;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub p5 %lu", p5);

  return p5;
}

////////////////////////////////////////////////////////////
// Menu window

static Window *window;

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
      return NUM_FIRST_MENU_ITEMS;

    case 1:
      return NUM_SECOND_MENU_ITEMS;

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
      menu_cell_basic_header_draw(ctx, cell_layer, "New Config");
      break;

    case 1:
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
      switch (cell_index->row) {
        case 0:
          // This is a basic menu item with a title and subtitle
          menu_cell_basic_draw(ctx, cell_layer, "New Config", "Create new config", NULL);
          break;
      }
      break;

    case 1:
      switch (cell_index->row) {
        case 0:
          // There is title draw for something more simple than a basic menu item
          menu_cell_basic_draw(ctx, cell_layer, "Soft egg", "120g, 300m", NULL);
          break;
        case 1:
          // There is title draw for something more simple than a basic menu item
          menu_cell_basic_draw(ctx, cell_layer, "Medium egg", "120g, 300m", NULL);
          break;
      }
      break;
  }
}

// Here we capture when a user selects a menu item
void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Select section
  switch (cell_index->section) {
    case 0: // Section: New Config
      // Select the row
      switch (cell_index->row) {
        // This is the menu item with the cycling icon
        case 0: // New Config
          // TODO: Implement creation of new config
          break;
      }
      break; // Section: New Config
        
    case 1: // Section: Saved Configs
      // Select the row
      switch (cell_index->row) {
        // This is the menu item with the cycling icon
        case 0: // First saved
          // TODO: Implement running timer
          break;
        case 1: // Second saved
          // TODO: Implement running timer
          break;
      }
      break; // Section: Saved Config
  }
}

// This initializes the menu upon window load
void window_load(Window *window) {
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

void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
}

int main(void) {
  for (int i = 0; i < 9; i++) {
    calculate_in_min_times_1000(data[i]);
  }

  window = window_create();

  // Setup the window handlers
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true /* Animated */);

  app_event_loop();

  window_destroy(window);
}
