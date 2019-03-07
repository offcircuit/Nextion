#include "Nextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
  _serial->begin(9600);
  _serial->flush();
};

bool INextion::begin() {
  return transmit("bkcmd=3") && transmit("page 0") && transmit("baud=9600");
}

bool INextion::receipt() {
  int16_t i = NEXTION_SERIAL_CYCLES;
  __length__ = 1;
  do {
    if (_serial->available()) __buffer__[__length__++] = _serial->read();
    if (!i--) return false;
  } while ((__length__ < 4) || ((__buffer__[__length__ - 1] & __buffer__[__length__ - 2] & __buffer__[__length__ - 3]) != 0xFF));
  return true;
}

uint8_t INextion::transmit(String instruction) {
  _serial->print(NEXTION_SERIAL_INSTRUCTION(instruction));
  while (!_serial->available());
  if ((__buffer__[0] = _serial->read()) && (receipt())) return __buffer__[0];
  return 0;
}

int16_t INextion::page() {
  if (transmit("sendme") && (__buffer__[0] == NEXTION_CMD_CURRENT_PAGE)) return __buffer__[1];
  return -1;
}

String INextion::property(String name) {
  if (transmit("get " + name)) {
    switch (__buffer__[0]) {

      case  NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
        return String((uint32_t)__buffer__[4] << 24 | (uint32_t)__buffer__[3] << 16 | (uint32_t)__buffer__[2] << 8 | __buffer__[1]);
        break;

      case NEXTION_CMD_STRING_DATA_ENCLOSED:
        String s = "";
        for (uint8_t i = 1; i < __length__ - 3; s += String(char(__buffer__[i++])));
        return s;
        break;
    }
  }
  return __buffer__[0];
}

uint8_t INextion::wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
  if (transmit("addt " + String(id) + "," + String(channel) + "," + String(length)) && (__buffer__[0] == NEXTION_CMD_TRANSPARENT_DATA_READY)) {
    for (size_t i = 0; i < length; i++) _serial->write((size_t)data[i]);
    while (!_serial->available());
    if ((__buffer__[0] = _serial->read()) && (receipt())) return __buffer__[0];
  }
  return __buffer__[0];
}

INextion::nextionCallback *INextion::callback(nextionTouch touch, nextionPointer pointer) {
  nextionCallback *item = new nextionCallback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}

void INextion::detach(nextionTouch touch) {
  if (callbacks) {
    nextionCallback *item = callbacks, *preview;
    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        if (item == callbacks) callbacks = callbacks->next;
        else preview->next = ((item->next) ? item->next : NULL);
        return;
      }
    while (item->next && (preview = item) && (item = item->next));
  }
}

void INextion::detach(nextionComponent component, nextionEvent event) {
  detach({component.page, component.id, event});
}

void INextion::event(nextionTouch touch, nextionPointer pointer) {
  if (callbacks) {
    nextionCallback *item = callbacks;

    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        item->pointer = pointer;
        return;
      }
    while (item->next && (item = item->next));
    item->next = callback(touch, pointer);

  } else callbacks = callback(touch, pointer);
}

void INextion::event(nextionComponent component, nextionEvent event, nextionPointer pointer) {
  INextion::event({component.page, component.id, event}, pointer);
}

uint8_t INextion::listen() {
  if (_serial->available() > 3) {
    switch ((__buffer__[0] = _serial->read())) {
      case NEXTION_CMD_STARTUP:
        if (receipt() && (__buffer__[1] == 0x00) && (__buffer__[2] == 0x00)) break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (receipt()) {
          nextionCallback *item = callbacks;
          while (item) {
            if ((item->touch.page == __buffer__[1]) && (item->touch.id == __buffer__[2]) && (item->touch.event == __buffer__[3]) && (__buffer__[7] = (bool)item->pointer) && __length__++) {
              item->pointer();
              break;
            }
            item = item->next;
          }
          break;
        }

      case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
      case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
      case NEXTION_CMD_SERIAL_BUFFER_OVERFLOW:
      case NEXTION_CMD_AUTO_ENTER_SLEEP:
      case NEXTION_CMD_AUTO_ENTER_WAKEUP:
      case NEXTION_CMD_READY:
      case NEXTION_CMD_START_MICROSD_UPDATE:
        if (receipt()) break;

      default: return NEXTION_CMD_UNKNOW_MESSAGE;
    }
    return __buffer__[0];
  }
  return 0;
}
