#ifndef INextion_H
#define INextion_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SoftwareSerial.h"
#include "nextionSyntax.h"

class INextion {
  private:
    typedef void (*Pointer) ();

    struct Callback {
      Callback *next;
      nexTouch touch;
      Pointer pointer;
    };


    SoftwareSerial *_serial;
    Callback *callbacks;

  public:
    int8_t _bkcmd;
    uint8_t __buffer[NEXTION_UART_SIZE];

    INextion(uint8_t rx, uint8_t tx);
    bool begin(uint16_t baud = 9600);

    uint8_t transmit(String instruction, uint8_t size = 4);
    bool receipt(uint8_t size);
    
    int8_t getbkcmd();
    uint8_t setbkcmd(uint8_t level);

    int16_t current();

    Callback *callback(nexTouch touch, Pointer pointer);
    void event(nexTouch touch, Pointer pointer);
    void event(nexComponent component, nexEvent event, Pointer pointer);
    void detach(nexTouch touch);
    void detach(nexComponent component, nexEvent event);

    void list() {
      Callback *item = callbacks;
      while (item) {
        Serial.print("Event ["); Serial.print(item->touch.page); Serial.print(", "); Serial.print(item->touch.id); Serial.print(", "); Serial.print(item->touch.event); Serial.println("]");
        item = item->next;
      }
    }


    uint8_t listen();
};

#endif
