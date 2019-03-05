#include "nextionUI.h"

Nextion::Nextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
};

bool Nextion::begin(uint16_t baud = 9600) {
  _serial->begin(baud);
  _serial->print(String("page 0" + char(0xFF) + char(0xFF) + char(0xFF)));
}

Nextion::Callback *Nextion::callback(nexTouch touch, Pointer pointer) {
  Callback *item = new Callback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}

void Nextion::add(nexTouch touch, Pointer pointer) {
  if (callbacks) {
    Callback *item = callbacks;

    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        item->pointer = pointer;
        return;
      }
    while (item->next && (item = item->next));
    item->next = callback(touch, pointer);

  } else callbacks = callback(touch, pointer);
}

void Nextion::add(nexComponent component, nexEvent event, Pointer pointer) {
  add({component.page, component.id, event}, pointer);
}

bool Nextion::receipt(char buffer[], size_t size) {
  for (uint8_t i = 1; i < size - 3; i++) buffer[i] = (uint8_t)_serial->read();
  for (uint8_t i = 0; i < 3; i++) size *= ((uint8_t)_serial->read() == 0xFF);
  return size;
}

char* Nextion::listen() {

  if (_serial->available() > 3) {

    static uint8_t buffer[6];
    buffer[0] = _serial->read();
    
    switch (buffer[0]) {
      case NEXTION_CMD_STARTUP:
        if (receipt(buffer, 6) && (buffer[1] == 0x00) && (buffer[2] == 0x00)) return buffer;
        break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (receipt(buffer, 7)) {
          Callback *item = callbacks;
          buffer[4] = 0;
          while (item) {
            if ((item->touch.page == buffer[1]) && (item->touch.id == buffer[2]) && (item->touch.event == buffer[3]) && (buffer[4] = (bool)item->pointer)) {
              item->pointer();
              return buffer;
            }
            item = item->next;
          }
          return buffer;
        }
        break;

      case NEXTION_CMD_SERIAL_BUFFER_OVERFLOW:
      case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
      case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
        if (receipt(buffer, 9)) return buffer;
        break;

      case NEXTION_CMD_AUTO_ENTER_SLEEP:
      case NEXTION_CMD_AUTO_ENTER_WAKEUP:
      case NEXTION_CMD_READY:
      case NEXTION_CMD_START_MICROSD_UPDATE:
        if (receipt(buffer, 4)) return buffer;
        break;
    }
  }
  return NULL;
}
