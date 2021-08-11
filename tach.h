/*********************************************************************************
 * data.h - holds data structures for WebTachometer.ino
 * 
 * -- Yuri - Aug 2021
*********************************************************************************/

#ifndef DATA_H
#define DATA_H

struct tach {
  uint8_t  pin;
  double   rpm = 0;
  uint32_t last_triggered = 0;
  uint8_t  num_poles = 1;
};

tach tach1;
tach tach2;

void IRAM_ATTR trigger1() {
  uint32_t time_now = micros();
  tach1.rpm = 60000000.0 / (double)(time_now - tach1.last_triggered) / (double)tach1.num_poles;
  tach1.last_triggered = time_now;
}

void IRAM_ATTR trigger2() {
  uint32_t time_now = micros();
  tach2.rpm = 60000000.0 / (double)(time_now - tach2.last_triggered) / (double)tach2.num_poles;
  tach2.last_triggered = time_now;
}

#endif // DATA_H
