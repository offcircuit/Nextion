#ifndef INEXTION_H
#define INEXTION_H

#ifndef NEXTION_H
#define NEXTION_H

#include "SoftwareSerial.h"
#include "NextionSyntax.h"
#include "Arduino.h"

class INextion {
  private:
    typedef void (*nextionPointer) ();

    struct nextionCallback {
      nextionCallback *next;
      nextionTouch touch;
      nextionPointer pointer;
    };

    SoftwareSerial *_serial;
    nextionCallback *callback(nextionTouch touch, nextionPointer pointer);

  protected:
    INextion(uint8_t rx, uint8_t tx);
    int8_t receipt();

  public:
    uint8_t __buffer__[NEXTION_UART_SIZE], __length__;
    nextionCallback *callbacks;

    bool begin();

    uint8_t transmit(String instruction);


    void detach(nextionTouch touch);
    void detach(nextionComponent component, nextionEvent event);
    void event(nextionTouch touch, nextionPointer pointer);
    void event(nextionComponent component, nextionEvent event, nextionPointer pointer);

    uint8_t listen();
};

class Nextion: public INextion {
  private:
  public:
    Nextion(uint8_t rx, uint8_t tx): INextion(rx, tx) {};

    int16_t current();
    String getAttribute(String name);
};

#endif
#endif
