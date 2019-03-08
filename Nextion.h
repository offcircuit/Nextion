#ifndef INextion_H
#define INextion_H

#ifndef NEXTION_H
#define NEXTION_H

#include "SoftwareSerial.h"
#include "Arduino.h"

#define NEXTION_SERIAL_INSTRUCTION(s) ({String(s) + char(0xFF) + char(0xFF) + char(0xFF);})
#define NEXTION_SERIAL_LENGTH 255
#define NEXTION_SERIAL_CYCLES 400

#define  NEXTION_EVENT_RELEASE 0
#define  NEXTION_EVENT_PRESS 1

#define NEXTION_CMD_STARTUP                                     0x00 // LISTEN
#define NEXTION_CMD_SERIAL_BUFFER_OVERFLOW                      0x24 // LISTEN
#define NEXTION_CMD_TOUCH_EVENT                                 0x65 // LISTEN
#define NEXTION_CMD_CURRENT_PAGE                                0x66 // CURRENT
#define NEXTION_CMD_TOUCH_COORDINATE_AWAKE                      0x67 // LISTEN
#define NEXTION_CMD_TOUCH_COORDINATE_SLEEP                      0x68 // LISTEN
#define NEXTION_CMD_STRING_DATA_ENCLOSED                        0x70 // GETATTRIBUTE
#define NEXTION_CMD_NUMERIC_DATA_ENCLOSED                       0x71 // GETATTRIBUTE
#define NEXTION_CMD_AUTO_ENTER_SLEEP                            0x86 // LISTEN
#define NEXTION_CMD_AUTO_ENTER_WAKEUP                           0x87 // LISTEN
#define NEXTION_CMD_READY                                       0x88 // LISTEN
#define NEXTION_CMD_START_MICROSD_UPDATE                        0x89 // LISTEN
#define NEXTION_CMD_TRANSPARENT_DATA_END                        0xFD // WAVE
#define NEXTION_CMD_TRANSPARENT_DATA_READY                      0xFE // WAVE
#define NEXTION_CMD_UNKNOW_MESSAGE                              0xFF // LISTEN

struct nextionComponent {
  int8_t page, id;
};

struct nextionTouch {
  int8_t page, id;
  bool event;
};

extern uint8_t NEXBUF[NEXTION_SERIAL_LENGTH];
extern uint8_t NEXBUFLEN;

class INextion {
  protected:
    typedef void (*nextionPointer) ();

    struct nextionCallback {
      nextionCallback *next;
      nextionTouch touch;
      nextionPointer pointer;
    };

    SoftwareSerial *_serial;
    nextionCallback *callbacks;

    nextionCallback * callback(nextionTouch touch, nextionPointer pointer);

  public:
    INextion(uint8_t rx, uint8_t tx, uint32_t baud = 9600);
    bool begin();
    bool baud(uint32_t baud);

    bool receipt();
    uint8_t transmit(String instruction);

    int16_t page();
    String read(String property);
    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length);

    void detach(nextionTouch touch);
    void detach(nextionComponent component, bool event);
    void event(nextionTouch touch, nextionPointer pointer);
    void event(nextionComponent component, bool event, nextionPointer pointer);
    uint8_t listen();
};

class Nextion: public INextion {
  public:
    Nextion(uint8_t rx, uint8_t tx): INextion(rx, tx) {};
};

#endif
#endif
