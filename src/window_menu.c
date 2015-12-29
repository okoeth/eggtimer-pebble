////////////////////////////////////////////////////////////
// Implementation of menu window module
// Main services include
//  - Construction
//  - Deconstruction
//  - Selection of entry
//  - TODO: Creation of entry
//  - TODO: Load and store data
//

#include "pebble.h"
#include "data.h"
#include "calculate.h"
#include "window_timer.h"
#include "window_pop.h"
#include "window_config.h"

#define NUM_MENU_SECTIONS 2

////////////////////////////////////////////////////////////
// Menu window

static Window *pwndMenu_g;
static MenuLayer *plyrMenu_g;
static struct Data * pstData_g = NULL;
static uint16_t u16DataRecords_g = 0;
static struct Configuration stDefaultConfig_g = {"Soft - 120g", "500m / 9C / 62C", 500, 120, 9, 62};

////////////////////////////////////////////////////////////
// Call back to tell menu number of sections

static uint16_t get_num_sections_callback(MenuLayer *plyr_p, void *pData_p) {
  return NUM_MENU_SECTIONS;
}

////////////////////////////////////////////////////////////
// Call back to tell menu number of rows

static uint16_t get_num_rows_callback(MenuLayer *plyr_p, uint16_t u16SectionIndex_p, void *pData_p) {
  switch (u16SectionIndex_p) {
    case 0: // First section
      return 1;
    case 1: // Second section
      return pstData_g->u16NumConfigs;
    default:
      return 0;
  }
}

////////////////////////////////////////////////////////////
// Call back to tell menu header height

static int16_t get_header_height_callback(MenuLayer *plyr_p, uint16_t u16SectionIndex_p, void *pData_p) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

////////////////////////////////////////////////////////////
// Call back to draw menu headers

static void draw_header_callback(GContext* pctx_p, const Layer *plyrCell_p, uint16_t u16SectionIndex_p, void *pData_p) {
  switch (u16SectionIndex_p) {
    case 0: // First section
      menu_cell_basic_header_draw(pctx_p, plyrCell_p, "Create");
      break;
    case 1: // Second section
      menu_cell_basic_header_draw(pctx_p, plyrCell_p, "Saved Configs");
      break;
  }
}

////////////////////////////////////////////////////////////
// Call back to draw menu rows

static void draw_row_callback(GContext* pctx_p, const Layer *plyrCell_p, MenuIndex *pidxCell_p, void *pData_p) {
  switch (pidxCell_p->section) {
    case 0:
      menu_cell_basic_draw(pctx_p, plyrCell_p, "New...", "Creates new config", NULL);
      break;
    case 1:
      menu_cell_basic_draw(
        pctx_p, 
        plyrCell_p, 
        pstData_g->prgstConfig[pidxCell_p->row].szTitle, 
        pstData_g->prgstConfig[pidxCell_p->row].szSubtitle, 
        NULL);
      break;
  }
}

////////////////////////////////////////////////////////////
// Call back to handle menu item selection

static void select_callback(MenuLayer *plyr_p, MenuIndex *pidxCell_p, void *pData_p) {
  uint32_t u32Duration = 0;
  switch (pidxCell_p->section) {
    case 0: // First section
      data_append_item(pstData_g, & stDefaultConfig_g); 
      window_config_create(plyr_p, & (pstData_g->prgstConfig[pstData_g->u16NumConfigs-1]));
      break;
    case 1: // Second section
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Height %lu", pstData_g->prgstConfig[pidxCell_p->row].u32Height);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "StartTemp %lu", pstData_g->prgstConfig[pidxCell_p->row].u32StartTemp);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "EndTemp %lu", pstData_g->prgstConfig[pidxCell_p->row].u32EndTemp);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Weight %lu", pstData_g->prgstConfig[pidxCell_p->row].u32Weight);    
      u32Duration = calculate_in_ms(pstData_g->prgstConfig[pidxCell_p->row]);
      window_timer_create(u32Duration);
      break;
  }
}

////////////////////////////////////////////////////////////
// Call back to handle menu item selection

static void select_long_callback(MenuLayer *plyr_p, MenuIndex *pidxCell_p, void *pData_p) {
  switch (pidxCell_p->section) {
    case 0: // First section
      // Do nothing
      break;
    case 1: // Second section
      data_remove_item(pstData_g, pidxCell_p->row);
      menu_layer_reload_data(plyrMenu_g);
      break;
  }
}

////////////////////////////////////////////////////////////
// This constructs the menu layer when window is loaded

static void window_load(Window *pwnd_p) {
  Layer *plyrRoot = window_get_root_layer(pwnd_p);
  GRect rectBounds = layer_get_frame(plyrRoot);
  plyrMenu_g = menu_layer_create(rectBounds);
  menu_layer_set_callbacks(
  	plyrMenu_g, 
  	NULL, 
  	(MenuLayerCallbacks){
    	.get_num_sections = get_num_sections_callback,
    	.get_num_rows = get_num_rows_callback,
    	.get_header_height = get_header_height_callback,
    	.draw_header = draw_header_callback,
    	.draw_row = draw_row_callback,
    	.select_click = select_callback,
    	.select_long_click = select_long_callback,
  	});
  menu_layer_set_click_config_onto_window(plyrMenu_g, pwnd_p);
  layer_add_child(plyrRoot, menu_layer_get_layer(plyrMenu_g));
}

////////////////////////////////////////////////////////////
// This destrcuts the timer layer when window is unloaded

static void window_unload(Window *pwnd_p) {
  menu_layer_destroy(plyrMenu_g);
}

////////////////////////////////////////////////////////////
// Create menu window

void window_menu_create (struct Data * pstData_p) {
  pstData_g = pstData_p;
  pwndMenu_g = window_create();
  window_set_window_handlers(pwndMenu_g, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(pwndMenu_g, true /* Animated */);
} 

////////////////////////////////////////////////////////////
// Destroy menu window

void window_menu_destroy () {
  if (pwndMenu_g!=NULL) {
  	  window_destroy(pwndMenu_g);
  	  pwndMenu_g=NULL;
  }
}

