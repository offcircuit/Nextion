#include "Nextion.h"

Nextion::Nextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t Nextion::begin(uint32_t speed) {
  const uint8_t rate[8] = {1, 2, 4, 8, 16, 24, 48};
  uint8_t baud = 0, signal;

  do {
    _serial->begin(rate[baud] * 2400UL);
    _serial->print("connect" + char(0xFF) + char(0xFF) + char(0xFF));
    for (signal = 255; signal-- && !_serial->available(););
  } while (!signal && (7 > ++baud));
  flush();
  
  if (speed) {
    write("baud=" + String(speed));
    _serial->begin(speed);
    flush();
    return speed;
  }

  return (rate[baud] * 2400UL);
}

void Nextion::attach() {
  _callbacks = NULL;
}

void Nextion::attach(nextionComponent component, bool event, nextionPointer pointer) {
  attach({component.page, component.id, event}, pointer);
}

void Nextion::attach(nextionTouch touch, nextionPointer pointer) {
  if (_callbacks) {
    nextionCallback *item = _callbacks;
    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        item->pointer = pointer;
        return;
      } while (item->next && (item = item->next));
    item->next = callback(touch, pointer);

  } else _callbacks = callback(touch, pointer);
}

void Nextion::detach() {
  _callbacks = NULL;
}

void Nextion::detach(nextionComponent component, bool event) {
  detach({component.page, component.id, event});
}

void Nextion::detach(nextionTouch touch) {
  if (_callbacks) {
    nextionCallback *item = _callbacks, *preview;
    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        if (item == _callbacks) _callbacks = _callbacks->next;
        else preview->next = ((item->next) ? item->next : NULL);
        return;
      } while (item->next && (preview = item) && (item = item->next));
  }
}

uint8_t Nextion::listen() {
  if (_serial->available() > 4) {

    switch (buffer[0] = uint8_t(_serial->read())) {

      case NEXTION_CMD_STARTUP:
        //if ((data[1] == 0x00) && (data[2] == 0x00))
        break;

      case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
      case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
        if (readBytes(1)) {
          if (_target) _target((uint16_t(buffer[1]) << 8) | uint8_t(buffer[2]), (uint16_t(buffer[3]) << 8) | uint8_t(buffer[4]), buffer[5]);
        }
        break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (readBytes(1)) {
          nextionCallback *item = _callbacks;
          while (item) {
            if ((item->touch.page == buffer[1]) && (item->touch.id == buffer[2]) && (item->touch.event == buffer[3]) && (item->pointer)) {
              item->pointer(buffer[1], buffer[2], buffer[2]);
              break;
            }
            item = item->next;
          }
        }
        break;

      default: flush();
    }
  }
  return 0;
}
