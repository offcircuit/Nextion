#ifndef INEXTION_H
#define INEXTION_H

#ifndef NEXTION_H
#define NEXTION_H

#include "SoftwareSerial.h"
#include "NextionSyntax.h"
#include "Arduino.h"

class INextion {
  private:

  protected:
    typedef void (*nextionPointer) ();

    struct nextionCallback {
      nextionCallback *next;
      nextionTouch touch;
      nextionPointer pointer;
    };

    SoftwareSerial *_serial;
    nextionCallback *callback(nextionTouch touch, nextionPointer pointer);

  public:
    uint8_t __buffer__[NEXTION_SERIAL_LENGTH], __length__;
    nextionCallback *callbacks;

    INextion(uint8_t rx, uint8_t tx);
    bool begin();

    bool receipt();
    uint8_t transmit(String instruction);

    int16_t page();
    String property(String name);
    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length);

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
};

#endif
#endif
