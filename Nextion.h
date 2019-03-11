#ifndef NEXTION_H
#ifndef INEXTION_H

#define INEXTION_H
#define NEXTION_H

#include "SoftwareSerial.h"

#define NEXTION_SERIAL_SIZE 9
#define NEXTION_SERIAL_CYCLES 255

#define  NEXTION_EVENT_RELEASE 0
#define  NEXTION_EVENT_PRESS 1

#define NEXTION_CMD_STARTUP                                     0x00 // LISTEN
#define NEXTION_CMD_SERIAL_BUFFER_OVERFLOW                      0x24 // LISTEN
#define NEXTION_CMD_TOUCH_EVENT                                 0x65 // LISTEN
#define NEXTION_CMD_CURRENT_PAGE                                0x66 // PAGE
#define NEXTION_CMD_TOUCH_COORDINATE_AWAKE                      0x67 // LISTEN
#define NEXTION_CMD_TOUCH_COORDINATE_SLEEP                      0x68 // LISTEN
#define NEXTION_CMD_STRING_DATA_ENCLOSED                        0x70 // READ
#define NEXTION_CMD_NUMERIC_DATA_ENCLOSED                       0x71 // READ
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

class INextion {
  protected:
    uint8_t _buffer[NEXTION_SERIAL_SIZE];

    bool _event, _pointer;
    String _string;
    uint8_t _command, _page, _id;
    uint16_t _x, _y;
    uint32_t _value;

    SoftwareSerial *_serial;

    bool response();
    bool wait();

  public:
    const bool &event = _event, &pointer = _pointer;
    const String &string = _string;
    const uint8_t &command = _command, &page = _page, &id = _id;
    const uint16_t &x = _x, &y = _y;
    const uint32_t &value = _value;

    INextion(uint8_t rx, uint8_t tx);

    uint32_t begin(uint32_t speed = 0);
    uint8_t transmit(String instruction);
};

class Nextion: public INextion {
  private:
    typedef void (*nextionPointer) ();

    struct nextionCallback {
      nextionCallback *next;
      nextionTouch touch;
      nextionPointer pointer;
    };

    nextionCallback *_callbacks;
    nextionCallback *callback(nextionTouch touch, nextionPointer pointer) {
      nextionCallback *item = new nextionCallback;
      item->next = NULL;
      item->pointer = pointer;
      item->touch = touch;
      return item;
    }

  public:
    Nextion(uint8_t rx, uint8_t tx): INextion(rx, tx) {};

    void attach(nextionTouch touch, nextionPointer pointer) {
      if (_callbacks) {
        nextionCallback *item = _callbacks;

        do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
            item->pointer = pointer;
            return;
          } while (item->next && (item = item->next));
        item->next = callback(touch, pointer);

      } else _callbacks = callback(touch, pointer);
    }

    void attach(nextionComponent component, bool event, nextionPointer pointer) {
      attach({component.page, component.id, event}, pointer);
    }

    uint8_t circle(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
      return transmit("cir " + String(x) + "," + String(y) + "," + String(r) + "," + String(c));
    }

    uint8_t clear(uint16_t c = 0xFFFFFF) {
      return transmit("cls " + String(c));
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
      if (_callbacks) {
        nextionCallback *item = _callbacks, *preview;
        do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
            if (item == _callbacks) _callbacks = _callbacks->next;
            else preview->next = ((item->next) ? item->next : NULL);
            return;
          } while (item->next && (preview = item) && (item = item->next));
      }
    }

    void detach(nextionComponent component, bool event) {
      detach({component.page, component.id, event});
    }

    uint8_t disable(uint8_t id) {
      return transmit("tsw " + String(id) + ",0");
    }

    uint8_t enable(uint8_t id) {
      return transmit("tsw " + String(id) + ",1");
    }

    uint8_t erase(uint8_t id) {
      return transmit("cle " + String(id) + ",255");
    }

    uint8_t erase(uint8_t id, uint8_t channel) {
      return transmit("cle " + String(id) + "," + String(channel));
    }

    uint8_t fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
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

    uint8_t listen() {
      if (_serial->available() > 3) {
        switch (_command = _serial->read()) {

          case NEXTION_CMD_STARTUP:
            if (response() && (_buffer[0] == 0x00) && (_buffer[1] == 0x00)) break;

          case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
          case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
            if (response()) {
              _x = (((uint16_t)_buffer[0] << 8) | _buffer[1]);
              _y = (((uint16_t)_buffer[2] << 8) | _buffer[3]);
              _event = _buffer[4];
              break;
            }

          case NEXTION_CMD_TOUCH_EVENT:
            if (response()) {
              _page = _buffer[0];
              _id = _buffer[1];
              _event = _buffer[2];
              _pointer = false;

              nextionCallback *item = _callbacks;
              while (item) {
                if ((item->touch.page == _page) && (item->touch.id == _id) && (item->touch.event == _event) && (_pointer = (bool)item->pointer)) {
                  item->pointer();
                  break;
                }
                item = item->next;
              }
              break;
            }

          case NEXTION_CMD_SERIAL_BUFFER_OVERFLOW:
          case NEXTION_CMD_AUTO_ENTER_SLEEP:
          case NEXTION_CMD_AUTO_ENTER_WAKEUP:
          case NEXTION_CMD_READY:
          case NEXTION_CMD_START_MICROSD_UPDATE:
            if (response()) break;

          default: return NEXTION_CMD_UNKNOW_MESSAGE;
        }
        return _command;
      }
      return 0;
    }

    int16_t page() {
      if (transmit("sendme") && (command == NEXTION_CMD_CURRENT_PAGE)) return _page;
      return -1;
    }

    uint8_t page(uint8_t page) {
      return transmit("page " + String(page));
    }

    uint8_t picture(uint16_t x, uint16_t y, uint8_t resource) {
      return transmit("pic " + String(x) + "," + String(y) + "," + String(resource));
    }

    String read(String attribute) {
      if (transmit("get " + attribute)) return ((_command == NEXTION_CMD_STRING_DATA_ENCLOSED) ? _string : String(_value));
    }
    
    bool reboot() {
      return transmit("rest");
    }
    
    uint8_t rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
      return transmit("draw " + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "," + String(c));
    }

    uint8_t reply(bool state) {
      return transmit("thup=" + String(state));
    }

    uint8_t show(uint8_t id) {
      return transmit("vis " + String(id) + ",1");
    }

    uint8_t sleep() {
      return transmit("sleep=1");
    }

    uint8_t text(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t font, uint16_t foreground, uint16_t background, uint8_t alignX, uint8_t alignY, uint8_t fill, String text) {
      return transmit("xstr " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," +
                      String(font) + "," + String(foreground) + "," + String(background) + "," + String(alignX) + "," + String(alignY) + "," + String(fill) + "," + text );
    }

    uint8_t wakeup() {
      return transmit("sleep=0");
    }

    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
      if (transmit("addt " + String(id) + "," + String(channel) + "," + String(length)) && (_command == NEXTION_CMD_TRANSPARENT_DATA_READY)) {
        for (size_t i = 0; i < length;) _serial->write(data[i++]);
        if (wait() && (_command = _serial->read()) && response()) return _command;
      }
      return 0;
    }
};

#endif
#endif
