////////////////////////////////////////////////////////////
// Declaration of ctimer window module
//

#ifndef WINDOW_TIMER_H_
#define WINDOW_TIMER_H_

#define START_DATA_ID 801
#define DURATION_DATA_ID 802

bool check_timer_state(); 
void window_timer_precreate(uint32_t u32Duration_p); 
void window_timer_create(); 

#endif
