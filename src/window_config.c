////////////////////////////////////////////////////////////
// Implementation of config window module
// Main services include
//  - Construction
//  - Deconstruction
//  - Edit configuration
//

#include "pebble.h"
#include "data.h"
#include "layer_option.h"

#define NUM_MENU_SECTIONS 2

////////////////////////////////////////////////////////////
// Config window

typedef enum {
  MODE_SELECT = 0,
  MODE_EDIT,
  NUM_MODE
} ModeId;

static Window *pwndConfig_g = NULL;
static Layer *plyrConfig_g = NULL;
static MenuLayer *plyrParentMenu_g = NULL;
static OptionLayer *plyrOptionHeight_g = NULL;
static OptionLayer *plyrOptionWeight_g = NULL;
static OptionLayer *plyrOptionStartTemp_g = NULL;
static OptionLayer *plyrOptionEndTemp_g = NULL;
static OptionLayer *plyrActiveOption_g = NULL;
static ActionBarLayer *plyrActionBar_g = NULL;
static GBitmap * picnPlus_g = NULL;
static GBitmap * picnMinus_g = NULL;
static GBitmap * picnSelect_g = NULL;
static struct Configuration * pstConfig_g = NULL;
static ModeId mode_g=MODE_SELECT;

static char * rgszOptionHeight [] = {"0", "100", "200", "300", "400", "500", "600", "700"};
static uint32_t rgu32Height [] = {0, 100, 200, 300, 400, 500, 600, 700};
static char * rgszOptionWeight [] = {"90", "100", "110", "120", "130", "140"};
static uint32_t rgu32Weight [] = {90, 100, 110, 120, 130, 140};
static char * rgszOptionStartTemp [] = {"4", "5", "6", "7", "8", "9", "10", "20"};
static uint32_t rgu32StartTemp [] = {4, 5, 6, 7, 8, 9, 10, 20};
static char * rgszOptionEndTemp [] = {"62", "67", "72", "77", "82"};
static uint32_t rgu32EndTemp [] = {62, 67, 72, 77, 82};

////////////////////////////////////////////////////////////
// Click config provider for action bar

void up_click_handler(ClickRecognizerRef stRecognizer_p, void *pContext_p) {
  //Window *pwnd = (Window *)pContext_p;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Mode %d", mode_g);

  if (mode_g==MODE_SELECT) {
    if (plyrOptionHeight_g->i16IsSelected > 0) {
      // Do nothing
    }
    else if (plyrOptionWeight_g->i16IsSelected > 0) {
      option_layer_set_selected (plyrOptionWeight_g, 0);
      option_layer_set_selected (plyrOptionHeight_g, 1);
    }
    else if (plyrOptionStartTemp_g->i16IsSelected > 0) {
      option_layer_set_selected (plyrOptionStartTemp_g, 0);
      option_layer_set_selected (plyrOptionWeight_g, 1);
    }
    else if (plyrOptionEndTemp_g->i16IsSelected > 0) {
      option_layer_set_selected (plyrOptionEndTemp_g, 0);
      option_layer_set_selected (plyrOptionStartTemp_g, 1);
    }
  }
  else if (mode_g==MODE_EDIT) {
    option_layer_next(plyrActiveOption_g);
  }
}

void select_click_handler(ClickRecognizerRef stRecognizer_p, void *pContext_p) {
  //Window *pwnd = (Window *)pContext_p;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Mode %d", mode_g);

  if (mode_g==MODE_SELECT) {
    if (plyrOptionHeight_g->i16IsSelected > 0) {
      plyrActiveOption_g=plyrOptionHeight_g;
    }
    else if (plyrOptionWeight_g->i16IsSelected > 0) {
      plyrActiveOption_g=plyrOptionWeight_g;
    }
    else if (plyrOptionStartTemp_g->i16IsSelected > 0) {
      plyrActiveOption_g=plyrOptionStartTemp_g;
    }
    else if (plyrOptionEndTemp_g->i16IsSelected > 0) {
      plyrActiveOption_g=plyrOptionEndTemp_g;
    }
    option_layer_set_active(plyrActiveOption_g, 1);
    mode_g=MODE_EDIT;
  }
  else if (mode_g==MODE_EDIT) {
    option_layer_set_active(plyrActiveOption_g, 0);
    plyrActiveOption_g=NULL;
    mode_g=MODE_SELECT;
  }
}

void down_click_handler(ClickRecognizerRef stRecognizer_p, void *pContext_p) {
  //Window *pwnd = (Window *)pContext_p;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Mode %d", mode_g);

  if (mode_g==MODE_SELECT) {
    if (plyrOptionHeight_g->i16IsSelected > 0) {
      option_layer_set_selected (plyrOptionHeight_g, 0);
      option_layer_set_selected (plyrOptionWeight_g, 1);
    }
    else if (plyrOptionWeight_g->i16IsSelected > 0) {
      option_layer_set_selected (plyrOptionWeight_g, 0);
      option_layer_set_selected (plyrOptionStartTemp_g, 1);
    }
    else if (plyrOptionStartTemp_g->i16IsSelected > 0) {
      option_layer_set_selected (plyrOptionStartTemp_g, 0);
      option_layer_set_selected (plyrOptionEndTemp_g, 1);
    }
    else if (plyrOptionEndTemp_g->i16IsSelected > 0) {
      // Do nothing
    }
  }
  else if (mode_g==MODE_EDIT) {
    option_layer_prev(plyrActiveOption_g);
  }
}

void save_and_close () {
  configuration_set_values (
    pstConfig_g, 
    rgu32Height[option_layer_selected_index(plyrOptionHeight_g)],
    rgu32Weight[option_layer_selected_index(plyrOptionWeight_g)],
    rgu32StartTemp[option_layer_selected_index(plyrOptionStartTemp_g)],
    rgu32EndTemp[option_layer_selected_index(plyrOptionEndTemp_g)]);

  // TODO: Add config to data base
  window_stack_pop(true);
  menu_layer_reload_data(plyrParentMenu_g);
}

void back_click_handler(ClickRecognizerRef stRecognizer_p, void *pContext_p) {
  save_and_close();
}

void select_long_click_handler(ClickRecognizerRef stRecognizer_p, void *pContext_p) {
  save_and_close();
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler) back_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_long_click_handler, NULL);
}

////////////////////////////////////////////////////////////
// Update the screen of the timer

static void update_layer_callback(Layer *plyr_p, GContext* pctx_p) {
  GFont * pfntLabel = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  graphics_context_set_text_color(pctx_p, GColorBlack);  
  // Labels
  graphics_draw_text(
    pctx_p, "Height", pfntLabel, GRect(4, 4, 70, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(
    pctx_p, "Weight", pfntLabel, GRect(4, 44, 70, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(
    pctx_p, "Start Tmp", pfntLabel, GRect(4, 84, 70, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(
    pctx_p, "End Tmp", pfntLabel, GRect(4, 124, 70, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  // Units
  graphics_draw_text(
    pctx_p, "m", pfntLabel, GRect(105, 4, 80, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(
    pctx_p, "g", pfntLabel, GRect(105, 44, 80, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(
    pctx_p, "°C", pfntLabel, GRect(105, 84, 80, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(
    pctx_p, "°C", pfntLabel, GRect(105, 124, 80, 20), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);  
}

////////////////////////////////////////////////////////////
// This constructs the config layer when window is loaded

static void window_load(Window *pwnd_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: window_load");
  // Action Bar Layer
  plyrActionBar_g = action_bar_layer_create();
  action_bar_layer_add_to_window(plyrActionBar_g, pwndConfig_g);
  action_bar_layer_set_click_config_provider(plyrActionBar_g, click_config_provider);
  action_bar_layer_set_icon(plyrActionBar_g, BUTTON_ID_UP, picnPlus_g);
  action_bar_layer_set_icon(plyrActionBar_g, BUTTON_ID_SELECT, picnSelect_g);  
  action_bar_layer_set_icon(plyrActionBar_g, BUTTON_ID_DOWN, picnMinus_g);  
  // Generic Layer
  Layer *plyrRoot = window_get_root_layer(pwnd_p);
  GRect rectBounds = layer_get_frame(plyrRoot);
  plyrConfig_g = layer_create(rectBounds);
  layer_set_update_proc(plyrConfig_g, update_layer_callback);
  layer_add_child(plyrRoot, plyrConfig_g);
  // Option Layers
  plyrOptionHeight_g = option_layer_create(rgszOptionHeight, 8, GRect(65, 4, 40, 25));
  plyrOptionHeight_g->i16IsSelected=1;
  layer_add_child(plyrRoot, option_layer_get_layer(plyrOptionHeight_g));
  plyrOptionWeight_g = option_layer_create(rgszOptionWeight, 6, GRect(65, 44, 40, 25));
  layer_add_child(plyrRoot, option_layer_get_layer(plyrOptionWeight_g));
  plyrOptionStartTemp_g = option_layer_create(rgszOptionStartTemp, 8, GRect(65, 84, 40, 25));
  layer_add_child(plyrRoot, option_layer_get_layer(plyrOptionStartTemp_g));
  plyrOptionEndTemp_g = option_layer_create(rgszOptionEndTemp, 5, GRect(65, 124, 40, 25));
  layer_add_child(plyrRoot, option_layer_get_layer(plyrOptionEndTemp_g));
}

////////////////////////////////////////////////////////////
// This destrcuts the config layer when window is unloaded

static void window_unload(Window *pwnd_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: window_unload");
  if (plyrConfig_g != NULL) {
    layer_destroy(plyrConfig_g);
    plyrConfig_g=NULL;
  }
  if (plyrOptionHeight_g != NULL) {
    option_layer_destroy(plyrOptionHeight_g);
    plyrOptionHeight_g=NULL;
  }
  if (plyrOptionWeight_g != NULL) {
    option_layer_destroy(plyrOptionWeight_g);
    plyrOptionWeight_g=NULL;
  }
  if (plyrOptionStartTemp_g != NULL) {
    option_layer_destroy(plyrOptionStartTemp_g);
    plyrOptionStartTemp_g=NULL;
  }
  if (plyrOptionEndTemp_g != NULL) {
    option_layer_destroy(plyrOptionEndTemp_g);
    plyrOptionEndTemp_g=NULL;
  }
  if (plyrActionBar_g != NULL) {
    action_bar_layer_destroy(plyrActionBar_g);
    plyrActionBar_g=NULL;
  }
}

////////////////////////////////////////////////////////////
// Create config window

void window_config_create(MenuLayer *plyrParentMenu_p, struct Configuration * pstConfig_p) {
  // Set-up config
  pstConfig_g=pstConfig_p;
  plyrParentMenu_g=plyrParentMenu_p;
  // Set-up window
  if (picnPlus_g==NULL) {
    picnPlus_g = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  }
  if (picnSelect_g==NULL) {
    picnSelect_g = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_SELECT);
  }
  if (picnMinus_g==NULL) {
    picnMinus_g = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);
  }
  pwndConfig_g = window_create();
  window_set_window_handlers(pwndConfig_g, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(pwndConfig_g, true /* Animated */);
}

////////////////////////////////////////////////////////////
// Destroy config window

void window_config_destroy() {
  if (picnPlus_g!=NULL) {
    gbitmap_destroy(picnPlus_g);
    picnPlus_g=NULL;
  }
  if (picnSelect_g!=NULL) {
    gbitmap_destroy(picnSelect_g);
    picnSelect_g=NULL;
  }
  if (picnMinus_g!=NULL) {
    gbitmap_destroy(picnMinus_g);
    picnMinus_g=NULL;
  }
  if (pwndConfig_g!=NULL) {
    window_destroy(pwndConfig_g);
    pwndConfig_g=NULL;
  }
  plyrParentMenu_g=NULL;
}

