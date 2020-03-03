#ifndef NEXTION_H
#define NEXTION_H

#include <sys/types.h>
#include <SoftwareSerial.h>

#define NEXTION_BUFFER_SIZE                10
#define NEXTION_SERIAL_CYCLES              255

#define NEXTION_EVENT_RELEASE              0
#define NEXTION_EVENT_PRESS                1

#define NEXTION_MODE_SLEEP                 0
#define NEXTION_MODE_AWAKE                 1

#define NEXTION_CMD_STARTUP                0x00
#define NEXTION_CMD_TOUCH_EVENT            0x65
#define NEXTION_CMD_CURRENT_PAGE           0x66
#define NEXTION_CMD_TOUCH_COORDINATE_AWAKE 0x67
#define NEXTION_CMD_TOUCH_COORDINATE_SLEEP 0x68
#define NEXTION_CMD_STRING_DATA_ENCLOSED   0x70
#define NEXTION_CMD_NUMERIC_DATA_ENCLOSED  0x71
#define NEXTION_CMD_AUTO_ENTER_SLEEP       0x86
#define NEXTION_CMD_AUTO_ENTER_WAKEUP      0x87
#define NEXTION_CMD_READY                  0x88
#define NEXTION_CMD_START_MICROSD_UPDATE   0x89
#define NEXTION_CMD_TRANSPARENT_DATA_END   0xFD
#define NEXTION_CMD_TRANSPARENT_DATA_READY 0xFE

#define NEXTION_BKCMD_INVALID              0x00
#define NEXTION_BKCMD_SUCCESS              0x01
#define NEXTION_BKCMD_COMPONENT_INVALID    0x02
#define NEXTION_BKCMD_PAGE_INVALID         0x03
#define NEXTION_BKCMD_PICTURE_INVALID      0x04
#define NEXTION_BKCMD_FONT_INVALID         0x05
#define NEXTION_BKCMD_BAUD_INVALID         0x11
#define NEXTION_BKCMD_WAVE_INVALID         0x12
#define NEXTION_BKCMD_VARIABLE_INVALID     0x1A
#define NEXTION_BKCMD_OPERATION_INVALID    0x1B
#define NEXTION_BKCMD_ASSIGN_FAILED        0x1C
#define NEXTION_BKCMD_EEPROM_FAILED        0x1D
#define NEXTION_BKCMD_PARAMETER_INVALID    0x1E
#define NEXTION_BKCMD_IO_FAILED            0x1F
#define NEXTION_BKCMD_ESCAPE_UNDEFINED     0x20
#define NEXTION_BKCMD_NAME_TOO_LONG        0x23

#define NEXTION_BKCMD_RETURN_OFF           0
#define NEXTION_BKCMD_RETURN_SUCCESS       1
#define NEXTION_BKCMD_RETURN_FAILS         2
#define NEXTION_BKCMD_RETURN_ALL           3

#define NEXTION_COLOR_BLACK 0              //  BLACK 
#define NEXTION_COLOR_BLUE 31              //  BLUE
#define NEXTION_COLOR_GREEN 2016           //  GREEN
#define NEXTION_COLOR_GRAY 33840           //  GRAY
#define NEXTION_COLOR_BROWN 48192          //  BROWN
#define NEXTION_COLOR_RED 63488            //  RED
#define NEXTION_COLOR_YELLOW 65504         //  YELLOW
#define NEXTION_COLOR_WHITE 65535          //  WHITE

struct nextionComponent {
  int8_t page, id;
};

struct nextionEvent {
  int8_t page, id;
  bool state;
};

typedef void (*nextionOnChange) (bool);
typedef void (*nextionOnEvent) (uint8_t, uint8_t, bool);
typedef void (*nextionOnPointer) ();
typedef void (*nextionOnTouch) (uint16_t, uint16_t, bool);

class Nextion {
  protected:
    uint8_t *_buffer = (uint8_t *) malloc(NEXTION_BUFFER_SIZE);
    String _data = "";
    size_t _index = 0;
    size_t _length = 0;
    size_t _map;
    SoftwareSerial *_serial;
    uint8_t _signal = NEXTION_SERIAL_CYCLES;

  private:
    struct nextionCallback {
      nextionCallback *next;
      nextionEvent event;
      nextionOnEvent pointer;
    };

    nextionCallback *_callbacks;
    nextionOnChange _onChange;
    nextionOnPointer _onReady;
    nextionOnPointer _onStart;
    nextionOnTouch _onTouch;
    nextionOnPointer _onUpdate;

    uint32_t baud();
    nextionCallback *callback(nextionEvent event, nextionOnEvent pointer);
    bool connect();
    void flush();
    uint8_t read();
    uint8_t readln();
    void restore();
    void send(String data);

  public:
    Nextion(uint8_t rx, uint8_t tx);
    uint32_t begin(uint32_t speed = 0);
    void attach();
    void attach(nextionComponent component, bool state, nextionOnEvent pointer);
    void attach(nextionEvent event, nextionOnEvent pointer);
    uint8_t backlight(uint8_t value);
    uint8_t bkcmd(uint8_t mode);
    uint8_t brush(uint16_t c);
    uint8_t circle(uint16_t x, uint16_t y, uint16_t r, uint16_t c);
    uint8_t clear(uint16_t c = 0xFFFFFF);
    uint8_t click(uint8_t id, bool event);
    size_t content(uint8_t *&buffer);
    uint8_t crop(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t resource);
    uint8_t crop(uint16_t dx, uint16_t dy, uint16_t w, uint16_t h, uint16_t sx, uint16_t sy, uint8_t resource);
    uint8_t delay(uint16_t milliseconds);
    void detach();
    void detach(nextionComponent component, bool state);
    void detach(nextionEvent event);
    uint8_t disable(uint8_t id);
    uint8_t draw(bool state);
    uint8_t enable(uint8_t id);
    uint8_t erase(uint8_t id);
    uint8_t erase(uint8_t id, uint8_t channel);
    uint8_t fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t c);
    uint8_t fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c);
    String get(String data);
    uint8_t hide(uint8_t id);
    uint8_t line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
    int16_t listen();
    void onChange(nextionOnChange pointer);
    void onReady(nextionOnPointer pointer);
    void onStart(nextionOnPointer pointer);
    void onTouch(nextionOnTouch pointer);
    void onUpdate(nextionOnPointer pointer);
    bool open(size_t length);
    int16_t page();
    uint8_t page(uint8_t page);
    uint8_t picture(uint16_t x, uint16_t y, uint8_t resource);
    uint8_t print(String data);
    uint8_t rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c);
    uint8_t reply(bool state);
    uint8_t reset();
    uint8_t sendxy(bool state);
    uint32_t setBaud(uint32_t speed);
    uint8_t show(uint8_t id);
    uint8_t sleep();
    uint8_t text(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t font, uint16_t foreground, uint16_t background, uint8_t alignX, uint8_t alignY, uint8_t fill, String text);
    bool upload(uint8_t *buffer, size_t length);
    uint8_t waitSerial(uint16_t seconds = 0);
    uint8_t waitTouch(uint16_t seconds = 0);
    uint8_t wakeup();
    uint8_t wakeupPage(uint8_t page = 255);
    uint8_t wave(uint8_t id, uint8_t channel, uint8_t data);
    uint8_t wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length);
    bool write(uint8_t data);
};

#endif
