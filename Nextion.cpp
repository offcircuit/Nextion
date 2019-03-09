#include "Nextion.h"

uint8_t NEXBUF[NEXTION_SERIAL_SIZE];
uint8_t NEXBUFLEN;

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t INextion::begin(uint32_t speed = 0) {
  const uint8_t baud[8] = {1, 2, 4, 8, 16, 24, 48, 0};
  uint8_t current = 0;

  do _serial->begin(baud[current] * 2400UL);
  while (!transmit("bkcmd=3") && (7 > ++current));

  if (speed && (speed != (baud[current] * 2400UL))) for (uint8_t i = 0; i < 7; i++)
      if (speed == (baud[i] * 2400UL)) {
        if (transmit("baud=" + String(speed))) {
          _serial->begin(speed);
          return speed;
        }
        _serial->begin(baud[current] * 2400UL);
        break;
      }
  return (baud[current] * 2400UL);
}

bool INextion::reset() {
  return (transmit("rest") && (NEXBUF[0] = NEXBUF[1] = 0));
}

bool INextion::compose(String instruction) {
  while (_serial->available()) _serial->read();
  return _serial->print((instruction + char(0xFF) + char(0xFF) + char(0xFF))) && wait();
}

bool INextion::wait() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  while (--cycle && !_serial->available());
  return cycle;
}

bool INextion::response() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  NEXBUFLEN = 1;
  do if (_serial->available()) NEXBUF[NEXBUFLEN++] = _serial->read();
  while (cycle-- && (NEXBUFLEN < NEXTION_SERIAL_SIZE) && ((NEXBUFLEN < 4) || ((NEXBUF[NEXBUFLEN - 1] & NEXBUF[NEXBUFLEN - 2] & NEXBUF[NEXBUFLEN - 3]) != 0xFF)));
  while (_serial->available()) _serial->read();
  return cycle;
}

uint8_t INextion::transmit(String instruction) {
  if (compose(instruction) && (NEXBUF[0] = _serial->read()) && response()) {

    delay(NEXTION_SERIAL_DELAY);

    return NEXBUF[0];
  }
  delay(NEXTION_SERIAL_DELAY);
  return 0;
}

String INextion::read(String attribute) {
  if (compose("get " + attribute) && (NEXBUF[0] = _serial->read()))
    switch (NEXBUF[0]) {

      case  NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
        if (response()) return String((uint32_t)NEXBUF[4] << 24 | (uint32_t)NEXBUF[3] << 16 | (uint32_t)NEXBUF[2] << 8 | NEXBUF[1]);

      case NEXTION_CMD_STRING_DATA_ENCLOSED:
        uint8_t cycle = NEXTION_SERIAL_CYCLES;
        String string = "";
        NEXBUFLEN = 0;
        do if (_serial->available()) {
            string += (char)_serial->read();
            NEXBUFLEN++;
            delayMicroseconds(500);
          }
        while (cycle--  && ((NEXBUFLEN < 4) || ((string[NEXBUFLEN - 1] & string[NEXBUFLEN - 2] & string[NEXBUFLEN - 3]) != 0xFF)));
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
    if (wait() && (NEXBUF[0] = _serial->read()) && response()) return NEXBUF[0];
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

uint8_t INextion::listen() {
  if (_serial->available() > 3) {
    switch ((NEXBUF[0] = _serial->read())) {

      case NEXTION_CMD_STARTUP:
        if (response() && (NEXBUF[1] == 0x00) && (NEXBUF[2] == 0x00)) break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (response()) {
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
        if (response()) break;

      default: return NEXTION_CMD_UNKNOW_MESSAGE;
    }
    return NEXBUF[0];
  }
  return 0;
}
