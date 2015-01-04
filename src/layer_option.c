////////////////////////////////////////////////////////////
// Implementation of option layer module
// Main services include
//  - Construction
//  - Deconstruction
//  - Show option
//  - Toggle option
// TODO: More configuration options, e.g. fonts
//

#include "pebble.h"
#include "layer_option.h"


////////////////////////////////////////////////////////////
// Update the screen of the timer

static void update_layer_callback(Layer *plyr_p, GContext* pctx_p) {
  OptionLayer * plyrOption = (OptionLayer *) (* (void **)layer_get_data(plyr_p));
  if (plyrOption==NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Layer Data is NULL");
  } else {
    GFont * pfntLabel = fonts_get_system_font(FONT_KEY_GOTHIC_18);
    GRect rectBounds = layer_get_frame(plyr_p);
    graphics_context_set_text_color(pctx_p, GColorBlack);
    graphics_context_set_stroke_color(pctx_p, GColorBlack);
    graphics_context_set_fill_color(pctx_p, GColorWhite);

    if (plyrOption->i16IsSelected > 0) {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Is selected");
      graphics_draw_rect(pctx_p, GRect(0, 0, rectBounds.size.w, rectBounds.size.h));
    } else {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Is not selected");
    }

    if (plyrOption->i16IsActive > 0) {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Is active");
      graphics_context_set_text_color(pctx_p, GColorWhite);
      graphics_context_set_stroke_color(pctx_p, GColorBlack);
      graphics_context_set_fill_color(pctx_p, GColorBlack);      
      graphics_fill_rect(pctx_p, GRect(2, 2, rectBounds.size.w-4, rectBounds.size.h-4), 0, GCornerNone);
    }
    else {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Is not active");
    }

    graphics_draw_text(
      pctx_p, 
      plyrOption->rgszOptions[plyrOption->i16SelectedOption], 
      pfntLabel, 
      GRect(3, 0, rectBounds.size.w, rectBounds.size.h), 
      GTextOverflowModeWordWrap, 
      GTextAlignmentLeft, 
      NULL);
  }
}

////////////////////////////////////////////////////////////
// Set / unset selected

void option_layer_set_selected(OptionLayer * plyrOption_p, int16_t i16IsSelected_p) {
  plyrOption_p->i16IsSelected=i16IsSelected_p;
  layer_mark_dirty(option_layer_get_layer(plyrOption_p));
}

////////////////////////////////////////////////////////////
// Set / unset active

void option_layer_set_active(OptionLayer * plyrOption_p, int16_t i16IsActive_p) {
  plyrOption_p->i16IsActive=i16IsActive_p;
  layer_mark_dirty(option_layer_get_layer(plyrOption_p));
}

////////////////////////////////////////////////////////////
// Show next option

void option_layer_next(OptionLayer * plyrOption_p) {
  if (plyrOption_p!=NULL) {
    if (plyrOption_p->i16SelectedOption < plyrOption_p->i16NumOptions-1) {
      plyrOption_p->i16SelectedOption+=1;
        layer_mark_dirty(option_layer_get_layer(plyrOption_p));
    }
  }
}

////////////////////////////////////////////////////////////
// Show prev option

void option_layer_prev(OptionLayer * plyrOption_p) {
  if (plyrOption_p!=NULL) {
    if (plyrOption_p->i16SelectedOption > 0) {
      plyrOption_p->i16SelectedOption-=1;
        layer_mark_dirty(option_layer_get_layer(plyrOption_p));
    }
  }
}

////////////////////////////////////////////////////////////
// Return selected index

int16_t option_layer_selected_index(OptionLayer * plyrOption_p) {
  if (plyrOption_p!=NULL) {
    return plyrOption_p->i16SelectedOption;
  }
  else {
    return -1;
  }
} 


////////////////////////////////////////////////////////////
// Create option layer

OptionLayer * option_layer_create(char * prgszOptions_p [], uint16_t i16NumOptions_p, GRect rect_p) {
  OptionLayer * plyrOption = calloc(1, sizeof(OptionLayer));
  for (int i = 0; i < 16 && i < i16NumOptions_p; i++) {
    strcpy(plyrOption->rgszOptions [i], prgszOptions_p [i]);
  }
  plyrOption->i16IsActive = 0;
  plyrOption->i16IsSelected = 0;
  plyrOption->i16SelectedOption = 0;
  plyrOption->i16NumOptions = i16NumOptions_p;
  plyrOption->plyr = layer_create_with_data(rect_p, sizeof(OptionLayer *));
  layer_set_update_proc(plyrOption->plyr, update_layer_callback);
  memcpy (layer_get_data(plyrOption->plyr), & plyrOption, sizeof(OptionLayer *));
  return plyrOption;
}

////////////////////////////////////////////////////////////
// Destroy option layer

void option_layer_destroy(OptionLayer * plyrOption_p) {
  if (plyrOption_p!=NULL) {
    if (plyrOption_p->plyr!=NULL) {
      layer_destroy(plyrOption_p->plyr);
    }
    free(plyrOption_p);
  }
}

////////////////////////////////////////////////////////////
// Get underlying layer

Layer * option_layer_get_layer(OptionLayer * plyrOption_p) {
	return plyrOption_p->plyr;
}
