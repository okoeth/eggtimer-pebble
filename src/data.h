////////////////////////////////////////////////////////////
// Declaration of data structure 
//

#ifndef DATA_H_
#define DATA_H_

#define DATA_RET_OK 0
#define DATA_RET_OUT_OF_BOUNDS -1

struct Configuration {
  char     szTitle[32]; 
  char     szSubtitle[32];
  uint32_t u32Height;      //  m, Maisach: 512m
  uint32_t u32Weight;      //  g, S/M/L: 120g/140g/160g 
  uint32_t u32StartTemp;   // °C, 4°C 
  uint32_t u32EndTemp;     // °C, S/M/H: 62°C/72°C/82°C
};

struct Data {
  uint16_t u16NumConfigs;
  struct Configuration * prgstConfig;
};

struct Data *  data_create ();
int16_t data_destroy (struct Data * pstData_p);
int16_t data_append_items (struct Data * pstData_p, struct Configuration * pstConfig_p, uint16_t u16NumConfigs_p);
int16_t data_append_item (struct Data * pstData_p, struct Configuration * pstConfig_p);
int16_t data_remove_item (struct Data * pstData_p, uint16_t u16Pos_p);
int16_t data_remove_all_items (struct Data * pstData_p);
int16_t data_log(struct Data * pstData_p);
int16_t data_config_log (struct Configuration * pstConfig_p);

#endif
