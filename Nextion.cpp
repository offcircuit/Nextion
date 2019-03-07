#include "Nextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
  _serial->begin(9600);
};

bool INextion::begin() {
  return transmit("bkcmd=3") && transmit("page 0");
}

uint8_t INextion::transmit(String instruction, uint8_t size = 4) {
  _serial->print(NEXTION_UART_INSTRUCTION(instruction));
  while (!_serial->available());
  if ((__buffer[0] = _serial->read()) && (receipt(size))) return __buffer[0];
  return 0;
}

bool INextion::receipt(uint8_t size) {
  while (_serial->available() < (size - 1));
  for ((__length = size); size - 1;) __buffer[(__length - --size)] = _serial->read();
  return ((__buffer[__length - 1] & __buffer[__length - 2] & __buffer[__length - 3]) == 0xFF);
}

Callback *INextion::callback(nexTouch touch, Pointer pointer) {
  Callback *item = new Callback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}

void INextion::detach(nexTouch touch) {
  if (callbacks) {
    Callback *item = callbacks, *preview;
    do if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        if (item == callbacks) callbacks = callbacks->next;
        else preview->next = ((item->next) ? item->next : NULL);
        return;
      }
    while (item->next && (preview = item) && (item = item->next));
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
        if (receipt(6) && (__buffer[1] == 0x00) && (__buffer[2] == 0x00)) return NEXTION_CMD_STARTUP;
        break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (receipt(7)) {
          Callback *item = callbacks;
          while (item) {
            if ((item->touch.page == __buffer[1]) && (item->touch.id == __buffer[2]) && (item->touch.event == __buffer[3]) && (__buffer[7] = (bool)item->pointer) && __length++) {
              item->pointer();
              return NEXTION_CMD_TOUCH_EVENT;
            }
            item = item->next;
          }
          return NEXTION_CMD_TOUCH_EVENT;
        }
        break;

      case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
      case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
        if (receipt(9)) return __buffer[0];
        break;

      case NEXTION_CMD_SERIAL_BUFFER_OVERFLOW:
      case NEXTION_CMD_AUTO_ENTER_SLEEP:
      case NEXTION_CMD_AUTO_ENTER_WAKEUP:
      case NEXTION_CMD_READY:
      case NEXTION_CMD_START_MICROSD_UPDATE:
        if (receipt(4)) return __buffer[0];
        break;
    }
  }
  return 0;
}

int16_t Nextion::current() {
  if (transmit("sendme", 5) && (__buffer[0] == NEXTION_CMD_CURRENT_PAGE)) return __buffer[1];
  return -1;
}
