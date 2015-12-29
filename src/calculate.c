////////////////////////////////////////////////////////////
// Implementation of calculation module 
// based on forumla of Charles D. H. Williams
//
// duration = 0,465 * weight ^ 2/3 * ln (0,76 * ( (t_water - t_start) / (t_water - t_end) ) )
//
// t_water = 100 °C - (1 °C * height / 285m)
// t_start =   4 °C (freezer) ... 20 °C (room temperature)
// t_end   =  62 °C (soft)    ... 82 °C (hard)
//

#include "pebble.h"
#include "data.h"

////////////////////////////////////////////////////////////
// In lieu of a math lib on pebble, advnced math
// is realised via pre-calculated look-ups

// LN of 1.20 => 4.40 in steps of 0.1 times 1000
uint32_t rgu32Lookup1 [] = {
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
uint32_t rgu32Lookup2 [] = {
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

////////////////////////////////////////////////////////////
// Calculation of timer (see above for formula)

static uint32_t calculate_in_min_times_1000(struct Configuration stData_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Height %lu", stData_p.u32Height);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "StartTemp %lu", stData_p.u32StartTemp);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "EndTemp %lu", stData_p.u32EndTemp);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Weight %lu", stData_p.u32Weight);

  // Part   I: u32p1 = ((t_water - t_start) * 100 ) / (t_water - t_end)
  uint32_t u32p1_1 = (100 * 100 - ((stData_p.u32Height * 100) / 285) ) - (stData_p.u32StartTemp * 100);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p1_1 %lu", u32p1_1);
  uint32_t u32p1 = u32p1_1 * 100 / ((100 * 100 - ((stData_p.u32Height * 100) / 285) ) - (stData_p.u32EndTemp * 100) ); 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p1 %lu", u32p1);

  // Part  II: u32p2 = (0,76 * 100 * u32u32p1) 
  uint32_t u32p2_1 = (760 * u32p1) / 10000; 
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p2_1 %lu", u32p2_1);
  uint32_t u32p2 = 0;
  if (u32p2_1 < 12) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub p2_1 %lu", u32p2_1);
    u32p2 = 12-12;
  } else if (u32p2_1 > 44) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub u32p2_1 %lu", u32p2_1);
    u32p2 = 44-12;
  } else {
    u32p2 = u32p2_1 - 12;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p2 %lu", u32p2);

  // Part III: u32p3 = lookup1(u32p2)  
  uint32_t u32p3 = rgu32Lookup1[u32p2];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p3 %lu", u32p3);

  // Part  IV: u32p4 = lookup2( (weight - 80) / 10)
  uint32_t u32p4_1 = (stData_p.u32Weight / 10);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p4_1 %lu", u32p4_1);

  uint32_t u32p4_2 = 0;
  if (u32p4_1 < 8) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub u32p4_1 %lu below lowest lookup", u32p4_1);
    u32p4_2 = 8-8;
  } else if (u32p4_1 > 44) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Sub u32p4_1 %lu above highest lookup", u32p4_1);
    u32p4_2 = 20-8;
  } else {
    u32p4_2 = u32p4_1 - 8;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p4_2 %lu", u32p4_2);

  uint32_t u32p4 = rgu32Lookup2[u32p4_2];
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p4 %lu", u32p4);

  // Part   V: u32p5 = 0,465 * 100 * u32p4 * u32p3 / 1000
  uint32_t u32p5 = (465 * u32p4 * u32p3) / 1000;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32p5 %lu", u32p5);

  return u32p5;
}

////////////////////////////////////////////////////////////
// Wrapper converting result to millis

uint32_t calculate_in_ms(struct Configuration stData_p) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter: calculate_in_ms");
  uint32_t u32MinutesTimes1000 = calculate_in_min_times_1000(stData_p);  
  uint32_t u32Seconds = u32MinutesTimes1000 / 1000;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32Seconds %lu", u32Seconds);  
  uint32_t u32Fraction = u32MinutesTimes1000 - (u32Seconds * 1000);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32Fraction %lu", u32Fraction);
  uint32_t u32Millis = (u32Seconds * 60 * 1000 ) + (u32Fraction * 1000 / 17);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sub u32Millis %lu", u32Millis);
  return u32Millis;
}
