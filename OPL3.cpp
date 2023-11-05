#include <Arduino.h>
#include "OPL3.h"

#define YM_D7   15
#define YM_D6   14
#define YM_D5   13
#define YM_D4   41
#define YM_D3   40
#define YM_D2   39
#define YM_D1   16
#define YM_D0   17

#define YM_CS   18
#define YM_RD   19
#define YM_WR   20

#define YM_IC   23
#define YM_A0   22
#define YM_A1   21

OPL3::OPL3() {

    for(int i = 13; i <= 23; i++) {
      pinMode(i,OUTPUT);
    }
    for(int i = 33; i <= 41; i++) {
      pinMode(i,OUTPUT);
    }
    digitalWrite(YM_RD, 1);
}
void OPL3::reset(void) {
    digitalWrite(YM_IC, LOW);
    delay(10);
    digitalWrite(YM_IC, HIGH);
    delay(10);
}

void OPL3::writereg(const uint8_t addr, const uint8_t val, const uint8_t port) {
    digitalWriteFast(YM_A1, port);
    //digitalWriteFast(13, port);
    
    digitalWriteFast(YM_A0, 0);
    delayNanoseconds(10); // tAS
    
    digitalWriteFast(YM_CS, 0);
    delayNanoseconds(500);
    write8(addr);
    delayNanoseconds(500);
    digitalWriteFast(YM_WR, 0);
    delayNanoseconds(500);
    digitalWriteFast(YM_WR, 1);
    digitalWriteFast(YM_CS, 1);
    delayNanoseconds(10); // tAH;

    digitalWriteFast(YM_A0, 1);
    delayNanoseconds(10); // tAS
    digitalWriteFast(YM_CS, 0);
    delayNanoseconds(500);
    write8(val);
    delayNanoseconds(500);
    digitalWriteFast(YM_WR, 0);
    delayNanoseconds(500);
    digitalWriteFast(YM_WR, 1);
    digitalWriteFast(YM_CS, 1);
}

void OPL3::write8(const uint8_t data) {
  digitalWriteFast( YM_D0, data&0x01);
  digitalWriteFast( YM_D1, data&0x02);
  digitalWriteFast( YM_D2, data&0x04);
  digitalWriteFast( YM_D3, data&0x08);
  digitalWriteFast( YM_D4, data&0x10);
  digitalWriteFast( YM_D5, data&0x20);
  digitalWriteFast( YM_D6, data&0x40);
  digitalWriteFast( YM_D7, data&0x80);   
}
