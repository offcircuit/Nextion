#include "nextionI.h"

NextionI::NextionI(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
};

bool NextionI::begin(uint16_t baud = 9600) {
  Serial.println("baud");
  _serial->begin(baud);
  Serial.print("bkcmd  ==  ");
  Serial.println(transmit("bkcmd=" + String(NEXTION_BKCMD_LEVEL)), HEX);

  Serial.print("page  ==  ");
  Serial.println(transmit("page 0"), HEX);
  return 1;

  return 0;
}

uint8_t NextionI::transmit(String instruction) {
  _serial->print(String(instruction + char(0xFF) + char(0xFF) + char(0xFF)));
  while (_serial->available() < 4);
  uint8_t buffer[4];
  for (uint8_t i = 0; i < 4; i++) buffer[i] = _serial->read();
  return (buffer[1] & buffer[2] & buffer[3] & buffer[0]);
}

bool NextionI::receipt(uint8_t buffer[], uint8_t size) {
  delayMicroseconds(200);
  for (uint8_t i = 1; i < size; i++) buffer[i] = _serial->read();
  return ((buffer[size - 1] & buffer[size - 2] & buffer[size - 3]) == 0xFF);
}

uint8_t NextionI::current() {
  _serial->print("sendme" + String(char(0xFF)) + String(char(0xFF)) + String(char(0xFF)));
  while (_serial->available() < 5);
  uint8_t buffer[5];
  if ((_serial->read() == NEXTION_CMD_CURRENT_PAGE) && (receipt(buffer, 5))) return buffer[1];
  return 0;
}

NextionI::Callback *NextionI::callback(nexTouch touch, Pointer pointer) {
  Callback *item = new Callback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}

void NextionI::event(nexTouch touch, Pointer pointer) {
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

void NextionI::event(nexComponent component, nexEvent event, Pointer pointer) {
  NextionI::event({component.page, component.id, event}, pointer);
}

uint8_t* NextionI::listen() {

  if (_serial->available() > 3) {

    static uint8_t buffer[9];
    buffer[0] = _serial->read();
    for (uint8_t i = 1; i < 9; i++) buffer[i] = 0;

    switch (buffer[0]) {
      case NEXTION_CMD_STARTUP:
        if (receipt(buffer, 6) && (buffer[1] == 0x00) && (buffer[2] == 0x00)) return buffer;
        break;

      case NEXTION_CMD_TOUCH_EVENT:
        if (receipt(buffer, 7)) {
          Callback *item = callbacks;
          while (item) {
            if ((item->touch.page == buffer[1]) && (item->touch.id == buffer[2]) && (item->touch.event == buffer[3]) && (buffer[7] = (bool)item->pointer)) {
              item->pointer();
              return buffer;
            }
            item = item->next;
          }
          return buffer;
        }
        break;

      case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
      case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
        if (receipt(buffer, 9)) return buffer;
        break;

      case NEXTION_CMD_SERIAL_BUFFER_OVERFLOW:
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
