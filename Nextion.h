#ifndef NEXTION_H
#ifndef INEXTION_H

#define INEXTION_H
#define NEXTION_H

#include <sys/types.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

#define  NEXTION_EVENT_RELEASE 0
#define  NEXTION_EVENT_PRESS 1

#define NEXTION_CMD_STARTUP 0x00                    // LISTEN
#define NEXTION_CMD_SERIAL_BUFFER_OVERFLOW 0x24     // LISTEN
#define NEXTION_CMD_TOUCH_EVENT 0x65                // LISTEN
#define NEXTION_CMD_CURRENT_PAGE 0x66               // PAGE
#define NEXTION_CMD_TOUCH_COORDINATE_AWAKE 0x67     // LISTEN
#define NEXTION_CMD_TOUCH_COORDINATE_SLEEP 0x68     // LISTEN
#define NEXTION_CMD_STRING_DATA_ENCLOSED 0x70       // READ
#define NEXTION_CMD_NUMERIC_DATA_ENCLOSED 0x71      // READ
#define NEXTION_CMD_AUTO_ENTER_SLEEP 0x86           // LISTEN
#define NEXTION_CMD_AUTO_ENTER_WAKEUP 0x87          // LISTEN
#define NEXTION_CMD_READY 0x88                      // LISTEN
#define NEXTION_CMD_START_MICROSD_UPDATE 0x89       // LISTEN
#define NEXTION_CMD_TRANSPARENT_DATA_END 0xFD       // WAVE
#define NEXTION_CMD_TRANSPARENT_DATA_READY 0xFE     // WAVE

struct nextionComponent {
  int8_t page, id;
};

struct nextionTouch {
  int8_t page, id;
  bool event;
};

class INextion {
  protected:
    SoftwareSerial *_serial;
  public:
    INextion(uint8_t rx, uint8_t tx);

    uint32_t begin(uint32_t speed = 0);
    uint8_t write(String data);
    void reading(char *data, uint16_t length) {
      String out;
      for (int i = 0; i < length; i++) {
        out += String(char(data[i]), HEX) + ",";
      }
      Serial.print("len = ");
      Serial.println(out);
    }
    void reading(String data) {
      String out;
      for (int i = 0; i < data.length(); i++) {
        out += String(char(data[i]), HEX) + ",";
      }
      Serial.print("len = ");
      Serial.println(out);
    }

};

class Nextion: public INextion {
  private:
    typedef void (*nextionPointer) (uint8_t, uint8_t, bool);
    typedef void (*nextionTarget) (uint16_t, uint16_t, bool);

    struct nextionCallback {
      nextionCallback *next;
      nextionTouch touch;
      nextionPointer pointer;
    };

    nextionCallback *_callbacks;
    nextionTarget _target;

    nextionCallback *callback(nextionTouch touch, nextionPointer pointer) {
      nextionCallback *item = new nextionCallback;
      item->next = NULL;
      item->pointer = pointer;
      item->touch = touch;
      return item;
    }

  public:
    Nextion(uint8_t rx, uint8_t tx): INextion(rx, tx) {};

    void attach() {
      _callbacks = NULL;
    }

    void attach(nextionComponent component, bool event, nextionPointer pointer) {
      attach({component.page, component.id, event}, pointer);
    }

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

    void detach() {
      _callbacks = NULL;
    }

    void detach(nextionComponent component, bool event) {
      detach({component.page, component.id, event});
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

    uint8_t listen() {
      if (_serial->available() > 4) {
        char buffer[8];
        switch (uint8_t(_serial->read())) {

          case NEXTION_CMD_STARTUP:
            //if ((data[1] == 0x00) && (data[2] == 0x00))
            break;

          case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
          case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
            _serial->readBytes(buffer, 8);
            if (_target) _target((uint16_t(buffer[0]) << 8) | uint8_t(buffer[1]), (uint16_t(buffer[2]) << 8) | uint8_t(buffer[3]), buffer[4]);
            break;

          case NEXTION_CMD_TOUCH_EVENT:
            _serial->readBytes(buffer, 6);
            nextionCallback *item = _callbacks;
            while (item) {
              if ((item->touch.page == buffer[0]) && (item->touch.id == buffer[1]) && (item->touch.event == buffer[2]) && (item->pointer)) {
                item->pointer(buffer[0], buffer[1], buffer[2]);
                break;
              }
              item = item->next;
            }
            break;
        }
      }
      return 0;
    }
};

#endif
#endif
