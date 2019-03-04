#include "nextionUI.h"

Nextion::Nextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
};

bool Nextion::begin(uint16_t baud = 9600) {
  _serial->begin(baud);
  return send("page 0");
}

bool Nextion::send(String command) {
  _serial->print(String(command + char(0xFF) + char(0xFF) + char(0xFF)));
  return receipt();
}

bool Nextion::receipt() {
  uint8_t buffer[4] = {0};
  return (sizeof(buffer) != _serial->readBytes((char*)buffer, sizeof(buffer))) | (buffer[0] == 0x01 && buffer[1] == 0xFF && buffer[2] == 0xFF && buffer[3] == 0xFF);
}

Nextion::Callback *Nextion::callback(nexTouch touch, Pointer pointer) {
  Callback *item = new Callback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}
nexCommand Nextion::command(uint32_t code, nexEvent event) {
  nexCommand command;
  command.code = code;
  command.event = event;
}

void Nextion::add(nexTouch touch, Pointer pointer) {
  if (callbacks == NULL) callbacks = callback(touch, pointer);
  else {
    Callback *item = callbacks;
    while (item->next) {
      if ((item->touch.page == touch.page) && (item->touch.id == touch.id) && (item->touch.event == touch.event)) {
        item->pointer = pointer;
        return;
      }
      item = item->next;
    }
    item->next = callback(touch, pointer);
  }
}

void Nextion::add(nexComponent component, nexEvent event, Pointer pointer) {
  add({component.page, component.id, event}, pointer);
}

nexCommand Nextion::loop() {
  if (_serial->available() > 3) {
    delayMicroseconds(1000);
    uint8_t size  = _serial->available(), buffer[size] = {0};

    if ((size == _serial->readBytes((char*)buffer, size)) && ((buffer[size - 1] == 0xFF) && (buffer[size - 2] == 0xFF) && (buffer[size - 3] == 0xFF))) {

      switch (buffer[0]) {
        
        case (NEXTION_CMD_STARTUP >> 8):
          if ((size == (NEXTION_CMD_STARTUP & 0x00FF)) && (buffer[1] == 0x00) && (buffer[2] == 0x00)) return command(buffer[0], NULL);
          break;

        case (NEXTION_CMD_SERIAL_BUFFER_OVERFLOW >> 8):
          if ((size == (NEXTION_CMD_SERIAL_BUFFER_OVERFLOW & 0x00FF)) && (buffer[1] == 0xFF) && (buffer[2] == 0xFF)) return command(buffer[0], NULL);
          break;

        case (NEXTION_CMD_TOUCH_EVENT >> 8):
          if (size == (NEXTION_CMD_TOUCH_EVENT & 0x00FF)) {
            Callback *item = callbacks;
            while (item) {
              if ((item->touch.page == buffer[1]) && (item->touch.id == buffer[2]) && (item->touch.event == buffer[3])) {
                item->pointer();
                return command(word(buffer[1], buffer[2]), buffer[3]);
              }
              item = item->next;
            }
          }
          break;

        case (NEXTION_CMD_TOUCH_COORDINATE_AWAKE >> 8):
          if (size == (NEXTION_CMD_TOUCH_COORDINATE_AWAKE & 0x00FF)) return command((buffer[1] << 24) + (buffer[2] << 16) + (buffer[3] << 8) + (buffer[4]), buffer[5]);
          break;

        case (NEXTION_CMD_TOUCH_COORDINATE_SLEEP >> 8):
          if (size == (NEXTION_CMD_TOUCH_COORDINATE_SLEEP & 0x00FF)) return command((buffer[1] << 24) + (buffer[2] << 16) + (buffer[3] << 8) + (buffer[4]), buffer[5]);
          break;

        case (NEXTION_CMD_AUTO_ENTER_SLEEP >> 8):
          if (size == (NEXTION_CMD_AUTO_ENTER_SLEEP & 0x00FF)) return command(buffer[0], NULL);
          break;

        case (NEXTION_CMD_AUTO_ENTER_WAKEUP >> 8):
          if (size == (NEXTION_CMD_AUTO_ENTER_WAKEUP & 0x00FF)) return command(buffer[0], NULL);
          break;

        case (NEXTION_CMD_READY >> 8):
          if (size == (NEXTION_CMD_READY & 0x00FF)) return command(buffer[0], NULL);
          break;

        case (NEXTION_CMD_START_MICROSD_UPDATE >> 8):
          if (size == (NEXTION_CMD_START_MICROSD_UPDATE & 0x00FF)) return command(buffer[0], NULL);
          break;
      }
    }
  }
  return command(0, NULL);
}
