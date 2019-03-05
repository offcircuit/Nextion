#ifndef NEXTION_H
#define NEXTION_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SoftwareSerial.h"
#include "nextionUISyntax.h"

class Nextion {
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
    Nextion(uint8_t rx, uint8_t tx);
    bool begin(uint16_t baud = 9600);

    bool receipt(char buffer[], size_t size);
    char* listen();

    Callback *callback(nexTouch touch, Pointer pointer);
    void add(nexTouch touch, Pointer pointer);
    void add(nexComponent component, nexEvent event, Pointer pointer);

    void list() {
      Callback *item = callbacks;
      while (item) {
        Serial.print("Event ["); Serial.print(item->touch.page); Serial.print(", "); Serial.print(item->touch.id); Serial.print(", "); Serial.print(item->touch.event); Serial.println("]");
        item = item->next;
      }
    }
};

#endif
