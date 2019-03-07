#ifndef INextion_H
#define INextion_H

#include "SoftwareSerial.h"
#include "NextionSyntax.h"

class INextion {
  private:
    SoftwareSerial *_serial;
    Callback *callback(nexTouch touch, Pointer pointer);

  protected:
    bool receipt(uint8_t size);

  public:
    uint8_t __buffer[NEXTION_UART_SIZE], __length;
    Callback *callbacks;

    INextion(uint8_t rx, uint8_t tx);
    bool begin();

    uint8_t transmit(String instruction, uint8_t size = 4);

    void detach(nexTouch touch);
    void detach(nexComponent component, nexEvent event);
    void event(nexTouch touch, Pointer pointer);
    void event(nexComponent component, nexEvent event, Pointer pointer);

    uint8_t listen();
};

#ifndef Nextion_H
#define Nextion_H

class Nextion: public INextion {
  private:
  public:
    Nextion(uint8_t rx, uint8_t tx): INextion(rx, tx) {};

    int16_t current();
};

#endif
#endif
