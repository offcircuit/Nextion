#include "INextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
};

bool INextion::begin(uint16_t baud = NEXTION_DEFAULT_BAUD) {
  Serial.println("baud");
  _serial->begin(baud);
  setbkcmd(3);
  _bkcmd = getbkcmd();
  return transmit("page 0");
}

uint8_t INextion::transmit(String instruction, uint8_t size = 4) {
  _serial->print(NEXTION_UART_INSTRUCTION(instruction));
  while (!_serial->available()); // bkcmd 0:1,2,3
  if ((__buffer[0] = _serial->read()) && (receipt(size))) return __buffer[0];
  return 0;
}

bool INextion::receipt(uint8_t size) {
  while (_serial->available() < (size - 1));
  for (uint8_t i = 1; i < size;) __buffer[i++] = _serial->read();
  return ((__buffer[size - 1] & __buffer[size - 2] & __buffer[size - 3]) == 0xFF);
}

int8_t INextion::getbkcmd() {
  if (transmit("get bkcmd", 8)) return __buffer[1];
  return -1;
}

uint8_t INextion::setbkcmd(uint8_t level) {
  return transmit("bkcmd=" + String(level));
}

int16_t INextion::current() {
  if (transmit("sendme", 5) && (__buffer[0] == NEXTION_CMD_CURRENT_PAGE)) return __buffer[1];
  return -1;
}

INextion::Callback *INextion::callback(nexTouch touch, Pointer pointer) {
  Callback *item = new Callback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}

void INextion::detach(nexTouch touch) {
  if (callbacks) {
    Callback *item = callbacks;
    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        item->pointer = NULL;
        return;
      }
    while (item->next && (item = item->next));
  }
}

void INextion::detach(nexComponent component, nexEvent event) {
  detach({component.page, component.id, event});
}

void INextion::event(nexTouch touch, Pointer pointer) {
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

void INextion::event(nexComponent component, nexEvent event, Pointer pointer) {
  INextion::event({component.page, component.id, event}, pointer);
}

uint8_t INextion::listen() {
  if (_serial->available() > 3) {
    switch ((__buffer[0] = _serial->read())) {
      case NEXTION_CMD_STARTUP:
        if (receipt(6) && (__buffer[1] == 0x00) && (__buffer[2] == 0x00)) return 6;
        break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (receipt(7)) {
          Callback *item = callbacks;
          while (item) {
            if ((item->touch.page == __buffer[1]) && (item->touch.id == __buffer[2]) && (item->touch.event == __buffer[3]) && (__buffer[7] = (bool)item->pointer)) {
              item->pointer();
              return 8;
            }
            item = item->next;
          }
          return 7;
        }
        break;

      case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
      case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
        if (receipt(9)) return 9;
        break;

      case NEXTION_CMD_SERIAL_BUFFER_OVERFLOW:
      case NEXTION_CMD_AUTO_ENTER_SLEEP:
      case NEXTION_CMD_AUTO_ENTER_WAKEUP:
      case NEXTION_CMD_READY:
      case NEXTION_CMD_START_MICROSD_UPDATE:
        if (receipt(4)) return 4;
        break;
    }
  }
  return 0;
}
