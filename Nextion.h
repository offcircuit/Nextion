#ifndef NEXTION_H
#define NEXTION_H

#include <sys/types.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

#define  NEXTION_EVENT_RELEASE 0
#define  NEXTION_EVENT_PRESS 1

#define NEXTION_SERIAL_CYCLES 255

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

class Nextion {
  protected:
    SoftwareSerial *_serial;
    char *buffer = (char *) malloc(12);
    uint8_t bkcmd = 0;

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
    Nextion(uint8_t rx, uint8_t tx);
    uint32_t begin(uint32_t speed = 0);

    void reading(uint8_t length) {
      String out;
      for (int i = 0; i < length; i++) {
        out += String(uint8_t(buffer[i]), HEX) + ",";
      }
      Serial.print("len = ");
      Serial.println(length);
      Serial.println(out);
    }
    void r(String data) {
      String out;
      for (int i = 0; i < data.length(); i++) {
        out += String(char(data[i]), HEX) + ",";
      }
      Serial.print("len = ");
      Serial.println(data.length());
      Serial.println(out);
    }

    void attach();
    void attach(nextionComponent component, bool event, nextionPointer pointer);
    void attach(nextionTouch touch, nextionPointer pointer);
    void detach();
    void detach(nextionComponent component, bool event);
    void detach(nextionTouch touch);
    uint8_t listen();

    String get(String attribute) {
      uint8_t signal;
      flush();
      _serial->print("get " + attribute + char(0xFF) + char(0xFF) + char(0xFF));
      for (signal = NEXTION_SERIAL_CYCLES; signal-- && !_serial->available(););

      if (signal) switch (buffer[0] = _serial->read()) {
          case NEXTION_CMD_STRING_DATA_ENCLOSED:
            String data = "";
            do while (_serial->available()) data += char(_serial->read());
            while (signal-- && ((data.length() < 4) || (((char)data[data.length() - 1] & (char)data[data.length() - 2] & (char)data[data.length() - 3]) != 0xFF)));
            return data.substring(0, data.length() - 3);

          case NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
            readBytes(1);
            return String((uint32_t(buffer[4]) << 24) + (uint32_t(buffer[3]) << 16) + (uint32_t(buffer[2]) << 8) + uint8_t(buffer[1]));
        }
      return "";
    }

    int16_t page() {
      if (print("sendme") == NEXTION_CMD_CURRENT_PAGE) return uint8_t(buffer[1]);
      else return -1;
    }

    bool page(uint8_t page) {
      return !print("page " + String(page));
    }
    
    void flush() {
      uint8_t signal = NEXTION_SERIAL_CYCLES;
      do while (_serial->available()) _serial->read(); while (signal--);
    }

    uint8_t print(const String data) {
      flush();
      _serial->print(data + char(0xFF) + char(0xFF) + char(0xFF));
      readBytes();
      return uint8_t(buffer[0]);
    }

    uint8_t readBytes(uint16_t length = 0) {
      uint8_t signal = NEXTION_SERIAL_CYCLES;
      do while (_serial->available()) buffer[length++] = char(_serial->read()); while (signal--);
      reading(length);
      return length;
    }

    void write(String data) {
      print(data);
    }
};

#endif
