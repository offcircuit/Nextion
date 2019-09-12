#ifndef NEXTION_H
#define NEXTION_H

#include <sys/types.h>
#include <SoftwareSerial.h>
#include <Arduino.h>

#define  NEXTION_EVENT_RELEASE 0
#define  NEXTION_EVENT_PRESS 1

#define NEXTION_SERIAL_CYCLES 255
#define NEXTION_BUFFER_SIZE 12

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

#define NEXTION_BKCMD_INVALID 0X00
#define NEXTION_BKCMD_SUCCESS 0X01
#define NEXTION_BKCMD_INVALID_COMPONENT 0X02
#define NEXTION_BKCMD_INVALID_PAGE 0X03
#define NEXTION_BKCMD_INVALID_PICTURE 0X04
#define NEXTION_BKCMD_INVALID_FONT 0X05
#define NEXTION_BKCMD_INVALID_BAUD 0X11
#define NEXTION_BKCMD_INVALID_WAVE 0X12
#define NEXTION_BKCMD_INVALID_VARIABLE 0X1A
#define NEXTION_BKCMD_INVALID_OPERATION 0X1B
#define NEXTION_BKCMD_ASSIGN_FAILED 0X1C
#define NEXTION_BKCMD_EEPROM_FAILED 0X1D
#define NEXTION_BKCMD_PARAMETER_INVALID 0X1E
#define NEXTION_BKCMD_IO_FAILED 0X1F
#define NEXTION_BKCMD_UNDEFINED_ESCAPR 0X20
#define NEXTION_BKCMD_NAME_TOO_LONG 0X23

#define NEXTION_BKCMD_RETURN_OFF 0
#define NEXTION_BKCMD_RETURN_SUCCESS 1
#define NEXTION_BKCMD_RETURN_FAILS 2
#define NEXTION_BKCMD_RETURN_ALL 3

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
    char *buffer = (char *) malloc(NEXTION_BUFFER_SIZE);
    uint8_t bkcmd = NEXTION_BKCMD_RETURN_FAILS;

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

    void t(uint8_t length) {
      String out;
      for (int i = 0; i < length; i++) {
        out += String(uint8_t(buffer[i]), HEX) + ",";
      }
      Serial.print("len = ");
      Serial.print(length);
      Serial.print(" val = ");
      Serial.println(out.substring(0, out.lastIndexOf(",")));
    }
    void attach();
    void attach(nextionComponent component, bool event, nextionPointer pointer);
    void attach(nextionTouch touch, nextionPointer pointer);
    uint8_t circle(uint16_t x, uint16_t y, uint16_t r, uint16_t c);
    uint8_t clear(uint16_t c = 0xFFFFFF);
    uint8_t click(uint8_t id, bool event);
    uint8_t crop(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t resource);
    uint8_t crop(uint16_t dx, uint16_t dy, uint16_t w, uint16_t h, uint16_t sx, uint16_t sy, uint8_t resource);
    void detach();
    void detach(nextionComponent component, bool event);
    void detach(nextionTouch touch);
    uint8_t disable(uint8_t id);
    uint8_t enable(uint8_t id);
    uint8_t erase(uint8_t id);
    uint8_t erase(uint8_t id, uint8_t channel);
    uint8_t fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t c);
    uint8_t fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c);
    void flush();
    String get(String attribute);
    uint8_t hide(uint8_t id);
    uint8_t line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
    int16_t listen();
    int16_t page();
    uint8_t page(uint8_t page);
    uint8_t picture(uint16_t x, uint16_t y, uint8_t resource);
    uint8_t print(String data);
    void println(String data);
    uint8_t reboot();
    uint8_t rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
    uint8_t reply(bool state);
    uint8_t show(uint8_t id);
    uint8_t sleep();
    void target(nextionTarget pointer);
    uint8_t text(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t font, uint16_t foreground, uint16_t background, uint8_t alignX, uint8_t alignY, uint8_t fill, String text);
    uint8_t wakeup();
    uint8_t wave(uint8_t id, uint8_t channel, uint8_t data);


    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
      if (print("addt " + String(id) + "," + String(channel) + "," + String(length)) == NEXTION_CMD_TRANSPARENT_DATA_READY) {
        for (size_t i = 0; i < length;) _serial->write(data[i++]);
        if (read()) return buffer[0];
      }
      return 0;
    }

    uint8_t read() {
      uint8_t length = NEXTION_BUFFER_SIZE;
      uint8_t signal = NEXTION_SERIAL_CYCLES;
      while (length) buffer[--length] = char(0x00);
      do while (_serial->available()) buffer[length++] += char(_serial->read());
      while (signal-- && ((length < 4) || (((char)buffer[length - 1] & (char)buffer[length - 2] & (char)buffer[length - 3]) != 0xFF)));
      t(length);
      return length;
    }

};

#endif
