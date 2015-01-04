////////////////////////////////////////////////////////////
// Declaration of option layer module
//

#ifndef LAYER_OPTION_H_
#define LAYER_OPTION_H_

typedef struct OptionLayer {
	Layer * plyr;
	char rgszOptions [16][16];
	int16_t i16IsActive;
	int16_t i16IsSelected;
	int16_t i16SelectedOption;
	int16_t i16NumOptions;
} OptionLayer;

OptionLayer * option_layer_create(char * pszOptions_p [], uint16_t i16NumOptions_p, GRect rect_p); 
void option_layer_destroy(OptionLayer * plyrOption_p); 
void option_layer_set_selected(OptionLayer * plyrOption_p, int16_t i16IsSelected_p); 
void option_layer_set_active(OptionLayer * plyrOption_p, int16_t i16IsActive_p); 
void option_layer_next(OptionLayer * plyrOption_p); 
void option_layer_prev(OptionLayer * plyrOption_p); 
int16_t option_layer_selected_index(OptionLayer * plyrOption_p); 
Layer * option_layer_get_layer(OptionLayer * plyrOption_p);

#endif
