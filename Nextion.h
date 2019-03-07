#ifndef INEXTION_H
#define INEXTION_H

#ifndef NEXTION_H
#define NEXTION_H

#include "SoftwareSerial.h"
#include "NextionSyntax.h"
#include "Arduino.h"

class INextion {
  protected:
    typedef void (*nextionPointer) ();

  private:
    struct nextionCallback {
      nextionCallback *next;
      nextionTouch touch;
      nextionPointer pointer;
    };

    SoftwareSerial *_serial;
    nextionCallback *callback(nextionTouch touch, nextionPointer pointer);

  public:
    uint8_t __buffer__[NEXTION_UART_SIZE], __length__;
    nextionCallback *callbacks;

    INextion(uint8_t rx, uint8_t tx);
    bool begin();

    uint8_t transmit(String instruction);
    bool receipt();

    String getAttribute(String name);

    void detach(nextionTouch touch);
    void event(nextionTouch touch, nextionPointer pointer);
    void detach(nextionComponent component, nextionEvent event);
    void event(nextionComponent component, nextionEvent event, nextionPointer pointer);

    int16_t current();

    uint8_t listen();
};

class Nextion: public INextion {
  private:
  public:
    Nextion(uint8_t rx, uint8_t tx): INextion(rx, tx) {};

};

#endif
#endif
