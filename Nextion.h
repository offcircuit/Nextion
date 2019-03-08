#ifndef INextion_H
#define INextion_H

#ifndef NEXTION_H
#define NEXTION_H

#include "SoftwareSerial.h"

#define NEXTION_SERIAL_SIZE 1024
#define NEXTION_SERIAL_CYCLES 255

#define  NEXTION_EVENT_RELEASE 0
#define  NEXTION_EVENT_PRESS 1

#define NEXTION_CMD_STARTUP                                     0x00 // LISTEN
#define NEXTION_CMD_SERIAL_BUFFER_OVERFLOW                      0x24 // LISTEN
#define NEXTION_CMD_TOUCH_EVENT                                 0x65 // LISTEN
#define NEXTION_CMD_CURRENT_PAGE                                0x66 // PAGE
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

extern uint8_t NEXBUF[NEXTION_SERIAL_SIZE];
extern uint8_t NEXBUFLEN;

class INextion {
  private:
    const uint8_t _baud[7] = {1, 2, 4, 8, 16, 24, 48};

    bool wait();
    
  protected:
    typedef void (*nextionPointer) ();

    struct nextionCallback {
      nextionCallback *next;
      nextionTouch touch;
      nextionPointer pointer;
    };

    SoftwareSerial *_serial;
    nextionCallback *_callbacks;

    nextionCallback *callback(nextionTouch touch, nextionPointer pointer);

  public:
    INextion(uint8_t rx, uint8_t tx);
    
    bool begin();
    bool baud(uint32_t baud);
    bool reset();

    uint8_t transmit(String instruction);
    bool receipt();

    int16_t page();
    String read(String attribute);
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

    int16_t page()  {
      return INextion::page();
    }

    uint8_t page(uint8_t page) {
      return transmit("page " + String(page));
    }

    uint8_t click(uint8_t id, bool event) {
      return transmit("click " + String(id) + "," + String(event));
    }

    uint8_t hide(uint8_t id) {
      return transmit("vis " + String(id) + ",0");
    }

    uint8_t show(uint8_t id) {
      return transmit("vis " + String(id) + ",1");
    }

    uint8_t disable(uint8_t id) {
      return transmit("tsw " + String(id) + ",0");
    }

    uint8_t enable(uint8_t id) {
      return transmit("tsw " + String(id) + ",1");
    }

    uint8_t wave(uint8_t id, uint8_t channel) {
      return transmit("cle " + String(id) + "," + String(channel));
    }

    uint8_t wave(uint8_t id, uint8_t channel, uint8_t data) {
      return transmit("add " + String(id) + "," + String(channel) + "," + String(data));
    }

    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
      return INextion::wave(id, channel, *data, length);
    }

    uint8_t clear(uint16_t c = 0xFFFFFF) {
      return transmit("cls " + String(c));
    }

    uint8_t picture(uint16_t x, uint16_t y, uint8_t resource) {
      return transmit("pic " + String(x) + "," + String(y) + "," + String(resource));
    }

    uint8_t crop(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t resource) {
      return transmit("picq " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," + String(resource));
    }

    uint8_t crop(uint16_t dx, uint16_t dy, uint16_t w, uint16_t h, uint16_t sx, uint16_t sy, uint8_t resource) {
      return transmit("xpic " + String(dx) + "," + String(dy) + "," + String(w) + "," + String(h) + "," + String(sx) + "," + String(sy) + "," + String(resource));
    }

    uint8_t text(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t font, uint16_t foreground, uint16_t background, uint8_t alignX, uint8_t alignY, uint8_t fill, String text) {
      return transmit("xstr " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," +
                      String(font) + "," + String(foreground) + "," + String(background) + "," + String(alignX) + "," + String(alignY) + "," + String(fill) + "," + text );
    }

    uint8_t fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c) {
      return transmit("fill " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," + String(c));
    }

    uint8_t line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
      return transmit("line " + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "," + String(c));
    }

    uint8_t rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
      return transmit("draw " + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "," + String(c));
    }

    uint8_t circle(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
      return transmit("cir " + String(x) + "," + String(y) + "," + String(r) + "," + String(c));
    }

    uint8_t disk(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
      return transmit("cirs " + String(x) + "," + String(y) + "," + String(r) + "," + String(c));
    }
};

#endif
#endif
