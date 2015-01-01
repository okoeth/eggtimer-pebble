////////////////////////////////////////////////////////////
// Declaration of pop window module
//

#ifndef WINDOW_POP_H_
#define WINDOW_POP_H_

#define POP_OK       0
#define POP_CANCEL  -1
#define POP_YES      1
#define POP_NO      -2

#define MODE_POP       0
#define MODE_OK        1
#define MODE_OK_CANCEL 2
#define MODE_YES_NO    3

void window_pop_create(int16_t i16Mode_p, char * pszMessage_p); 
void window_pop_destroy(); 

#endif
