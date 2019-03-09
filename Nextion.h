#ifndef INextion_H
#define INextion_H

#ifndef NEXTION_H
#define NEXTION_H

#include "SoftwareSerial.h"
#include "Arduino.h"

#define NEXTION_SERIAL_SIZE 9
#define NEXTION_SERIAL_CYCLES 255
#define NEXTION_SERIAL_DELAY 50
// higher as baud raise

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
    bool compose(String instruction);
    bool wait();
    bool response();

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

    uint32_t begin(uint32_t speed = 0);
    bool reset();

    uint8_t transmit(String instruction);

    String read(String attribute);
    int16_t page();
    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length);


    void detach(nextionTouch touch);
    void event(nextionTouch touch, nextionPointer pointer);
    uint8_t listen();
};

class Nextion: public INextion {
  public:
    Nextion(uint8_t rx, uint8_t tx): INextion(rx, tx) {};

    uint8_t circle(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
      return transmit("cir " + String(x) + "," + String(y) + "," + String(r) + "," + String(c));
    }

    uint8_t click(uint8_t id, bool event) {
      return transmit("click " + String(id) + "," + String(event));
    }

    uint8_t crop(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t resource) {
      return transmit("picq " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," + String(resource));
    }

    uint8_t crop(uint16_t dx, uint16_t dy, uint16_t w, uint16_t h, uint16_t sx, uint16_t sy, uint8_t resource) {
      return transmit("xpic " + String(dx) + "," + String(dy) + "," + String(w) + "," + String(h) + "," + String(sx) + "," + String(sy) + "," + String(resource));
    }

    void detach(nextionTouch touch) {
      INextion::detach(touch);
    }

    void detach(nextionComponent component, bool event) {
      INextion::detach({component.page, component.id, event});
    }

    uint8_t disable(uint8_t id) {
      return transmit("tsw " + String(id) + ",0");
    }

    uint8_t enable(uint8_t id) {
      return transmit("tsw " + String(id) + ",1");
    }

    uint8_t erase(uint16_t c = 0xFFFFFF) {
      return transmit("cls " + String(c));
    }

    void event(nextionTouch touch, nextionPointer pointer) {
      INextion::event(touch, pointer);
    }

    void event(nextionComponent component, bool event, nextionPointer pointer) {
      INextion::event({component.page, component.id, event}, pointer);
    }

    uint8_t fillCicle(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
      return transmit("cirs " + String(x) + "," + String(y) + "," + String(r) + "," + String(c));
    }

    uint8_t fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c) {
      return transmit("fill " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," + String(c));
    }

    uint8_t hide(uint8_t id) {
      return transmit("vis " + String(id) + ",0");
    }

    uint8_t line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
      return transmit("line " + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "," + String(c));
    }

    int16_t page()  {
      return INextion::page();
    }

    uint8_t page(uint8_t page) {
      return transmit("page " + String(page));
    }

    uint8_t picture(uint16_t x, uint16_t y, uint8_t resource) {
      return transmit("pic " + String(x) + "," + String(y) + "," + String(resource));
    }

    uint8_t rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
      return transmit("draw " + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "," + String(c));
    }

    uint8_t show(uint8_t id) {
      return transmit("vis " + String(id) + ",1");
    }

    uint8_t text(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t font, uint16_t foreground, uint16_t background, uint8_t alignX, uint8_t alignY, uint8_t fill, String text) {
      return transmit("xstr " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," +
                      String(font) + "," + String(foreground) + "," + String(background) + "," + String(alignX) + "," + String(alignY) + "," + String(fill) + "," + text );
    }

    uint8_t wave(uint8_t id, uint8_t channel) {
      return transmit("cle " + String(id) + "," + String(channel));
    }

    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
      return INextion::wave(id, channel, *data, length);
    }

    uint8_t wave(uint8_t id, uint8_t channel, uint8_t data) {
      return transmit("add " + String(id) + "," + String(channel) + "," + String(data));
    }

};


#endif
#endif
