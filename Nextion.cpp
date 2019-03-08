#include "Nextion.h"

uint8_t NEXBUF[NEXTION_SERIAL_SIZE];
uint8_t NEXBUFLEN;

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
  _serial->begin(9600);
};

bool INextion::begin() {
  for (uint8_t i = 0; i < 7 ; i++) {
    _serial->begin(_baud[6 - i]);
    if (transmit("bkcmd=3"))return true;
  }
  return false;
}

bool INextion::baud(uint32_t baud) {
  if (transmit("bauds=" + String(baud))) _serial->begin(baud);
  return NEXBUF[0];
}

bool INextion::reset() {
  if (transmit("rest")) {
    for (uint8_t i = 0; i < NEXTION_SERIAL_SIZE;) NEXBUF[i++] = 0;
    return true;
  }
}

bool INextion::wait() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  while (--cycle && !_serial->available());
  return cycle;
}

uint8_t INextion::transmit(String instruction) {
  if (_serial->print(instruction + char(0xFF) + char(0xFF) + char(0xFF)) &&  wait() && (NEXBUF[0] = _serial->read()) && receipt()) return NEXBUF[0];
  return 0;
}

bool INextion::receipt() {
  int16_t cycle = NEXTION_SERIAL_CYCLES;
  NEXBUFLEN = 1;
  do if (_serial->available()) NEXBUF[NEXBUFLEN++] = _serial->read();
  while (cycle-- && ((NEXBUFLEN < 4) || ((NEXBUF[NEXBUFLEN - 1] & NEXBUF[NEXBUFLEN - 2] & NEXBUF[NEXBUFLEN - 3]) != 0xFF)));
  return cycle;
}

String INextion::read(String attribute) {
  if (transmit("get " + attribute)) {
    switch (NEXBUF[0]) {

      case  NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
        return String((uint32_t)NEXBUF[4] << 24 | (uint32_t)NEXBUF[3] << 16 | (uint32_t)NEXBUF[2] << 8 | NEXBUF[1]);
        break;

      case NEXTION_CMD_STRING_DATA_ENCLOSED:
        String s = "";
        for (uint8_t i = 1; i < NEXBUFLEN - 3; s += String(char(NEXBUF[i++])));
        return s;
        break;
    }
  }
  return NEXBUF[0];
}

uint8_t INextion::wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
  if (transmit("addt " + String(id) + "," + String(channel) + "," + String(length)) && (NEXBUF[0] == NEXTION_CMD_TRANSPARENT_DATA_READY)) {
    for (size_t i = 0; i < length;) _serial->write(data[i++]);
    while (!_serial->available());
    if ((NEXBUF[0] = _serial->read()) && (receipt())) return NEXBUF[0];
  }
  return NEXBUF[0];
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

void INextion::detach(nextionComponent component, bool event) {
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
          nextionCallback *item = callbacks;
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
