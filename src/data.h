////////////////////////////////////////////////////////////
// Declaration of data structure 
//

#ifndef DATA_H_
#define DATA_H_

struct Configuration {
  char     szTitle[32]; 
  char     szSubtitle[32];
  uint32_t u32Height;      //  m, Maisach: 512m
  uint32_t u32Weight;      //  g, S/M/L: 120g/140g/160g 
  uint32_t u32StartTemp;   // °C, 4°C 
  uint32_t u32EndTemp;     // °C, S/M/H: 62°C/72°C/82°C
};

#endif
