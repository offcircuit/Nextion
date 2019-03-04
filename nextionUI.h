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
    bool send(String command);
    bool receipt();
    void add(nexTouch touch, Pointer pointer);
    void add(nexComponent component, nexEvent event, Pointer pointer);

    Callback *callback(nexTouch touch, Pointer pointer);
    nexCommand command(uint32_t code, nexEvent event);
    nexCommand loop();
};

#endif
