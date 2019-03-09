#include "Nextion.h"

uint8_t NEXBUF[NEXTION_SERIAL_SIZE];
uint8_t NEXBUFLEN;

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

bool INextion::begin(uint32_t speed = 0) {
  const uint8_t baud[7] = {1, 2, 4, 8, 16, 24, 48};
  
  if (speed) for (uint8_t i = 7; i > 0;)
      if ((speed == (baud[--i] * 2400UL)) && transmit("baud=" + String(speed))) {
        _serial->begin(speed);
        return transmit("bkcmd=3");
      }
      
  for (uint8_t i = 7; i > 0;) {
    _serial->begin(baud[--i] * 2400UL);
    if (transmit("bkcmd=3")) return true;
  }
  return false;
}

bool INextion::reset() {
  return (transmit("rest") && (NEXBUF[0] = 0) && (NEXBUF[1] = 0));
}

bool INextion::compose(String instruction) {
  while (_serial->available()) _serial->read();
  return (_serial->print(instruction + char(0xFF) + char(0xFF) + char(0xFF)) && wait());
}

bool INextion::wait() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  while (--cycle && !_serial->available());
  return cycle;
}

bool INextion::receipt() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  NEXBUFLEN = 1;
  do if (_serial->available()) NEXBUF[NEXBUFLEN++] = _serial->read();
  while (cycle-- && (NEXBUFLEN < 4) || ((NEXBUF[NEXBUFLEN - 1] & NEXBUF[NEXBUFLEN - 2] & NEXBUF[NEXBUFLEN - 3]) != 0xFF));
  return cycle;
}

uint8_t INextion::transmit(String instruction) {
  if (compose(instruction) && (NEXBUF[0] = _serial->read()) && receipt()) return NEXBUF[0];
  return 0;
}

String INextion::read(String attribute) {
  if (compose("get " + attribute) && (NEXBUF[0] = _serial->read()))
    switch (NEXBUF[0]) {

      case  NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
        if (receipt()) return String((uint32_t)NEXBUF[4] << 24 | (uint32_t)NEXBUF[3] << 16 | (uint32_t)NEXBUF[2] << 8 | NEXBUF[1]);

      case NEXTION_CMD_STRING_DATA_ENCLOSED:
        uint8_t cycle = NEXTION_SERIAL_CYCLES;
        String string = "";
        NEXBUFLEN = 0;
        do if (_serial->available() && (string += char(_serial->read()))) NEXBUFLEN++;
        while (cycle-- && ((NEXBUFLEN < 4) || ((string[NEXBUFLEN - 1] & string[NEXBUFLEN - 2] & string[NEXBUFLEN - 3]) != 0xFF)));
        if (cycle) return string.substring(0, NEXBUFLEN - 3);
    }
  return NEXBUF[0];
}

int16_t INextion::page() {
  if (transmit("sendme") && (NEXBUF[0] == NEXTION_CMD_CURRENT_PAGE)) return NEXBUF[1];
  return -1;
}

uint8_t INextion::wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
  if (transmit("addt " + String(id) + "," + String(channel) + "," + String(length)) && (NEXBUF[0] == NEXTION_CMD_TRANSPARENT_DATA_READY)) {
    for (size_t i = 0; i < length;) _serial->write(data[i++]);
    if (wait() && (NEXBUF[0] = _serial->read()) && receipt()) return NEXBUF[0];
  }
  return 0;
}

INextion::nextionCallback *INextion::callback(nextionTouch touch, nextionPointer pointer) {
  nextionCallback *item = new nextionCallback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}

void INextion::detach(nextionTouch touch) {
  if (_callbacks) {
    nextionCallback *item = _callbacks, *preview;
    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        if (item == _callbacks) _callbacks = _callbacks->next;
        else preview->next = ((item->next) ? item->next : NULL);
        return;
      } while (item->next && (preview = item) && (item = item->next));
  }
}

void INextion::detach(nextionComponent component, bool event) {
  detach({component.page, component.id, event});
}

void INextion::event(nextionTouch touch, nextionPointer pointer) {
  if (_callbacks) {
    nextionCallback *item = _callbacks;

    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        item->pointer = pointer;
        return;
      }
    while (item->next && (item = item->next));
    item->next = callback(touch, pointer);

  } else _callbacks = callback(touch, pointer);
}

void INextion::event(nextionComponent component, bool event, nextionPointer pointer) {
  INextion::event({component.page, component.id, event}, pointer);
}

uint8_t INextion::listen() {
  if (_serial->available() > 3) {
    switch ((NEXBUF[0] = _serial->read())) {

      case NEXTION_CMD_STARTUP:
        if (receipt() && (NEXBUF[1] == 0x00) && (NEXBUF[2] == 0x00)) break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (receipt()) {
          nextionCallback *item = _callbacks;
          while (item) {
            if ((item->touch.page == NEXBUF[1]) && (item->touch.id == NEXBUF[2]) && (item->touch.event == NEXBUF[3]) && (NEXBUF[7] = (bool)item->pointer)) {
              NEXBUFLEN++;
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
    return NEXBUF[0];
  }
  return 0;
}
