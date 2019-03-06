#ifndef NEXTIONI_H
#define NEXTIONI_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SoftwareSerial.h"
#include "nextionSyntax.h"

class NextionI {
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
    NextionI(uint8_t rx, uint8_t tx);
    bool begin(uint16_t baud = 9600);

    uint8_t transmit(String instruction);
    bool receipt(uint8_t buffer[], uint8_t size);

    uint8_t current();

    Callback *callback(nexTouch touch, Pointer pointer);
    void event(nexTouch touch, Pointer pointer);
    void event(nexComponent component, nexEvent event, Pointer pointer);
    uint8_t* listen();
};

#endif
