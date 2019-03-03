#include "nextionUI.h"

// PRIVATE

uint8_t Nextion::read() {
  uint8_t buffer[4] = {0};
  if ((sizeof(buffer) != _serial->readBytes((char*)buffer, sizeof(buffer))))
    if ((buffer[1] == 0xFF) && (buffer[2] == 0xFF) && (buffer[3] == 0xFF)) return buffer[0];
  return NEXTION_READ_ERROR;
}

bool Nextion::write(String command) {
  _serial->print(String(command + char(0xFF) + char(0xFF) + char(0xFF)));
  return read();
}

//PUBLIC

Nextion::Nextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
};

uint8_t Nextion::begin(uint32_t baud = 9600) {
  _serial->begin(baud);
  return write("page 0");
}

int8_t Nextion::loop() {
  if (_serial->available() == 7) {
    uint8_t buffer[7] = {0};
    if (sizeof(buffer) == _serial->readBytes((char*)buffer, sizeof(buffer))) {
      if ((buffer[0] == 0x65) && (buffer[4] == 0xFF) && (buffer[4] == 0xFF) && (buffer[5] == 0xFF) && (buffer[6] == 0xFF)) {
        int8_t id;
        if ((id = callbacks.find({buffer[1], buffer[2], buffer[3]})) > -1) {
          if (callbacks.callbacks[id].pointer) {
            callbacks.callbacks[id].pointer();
            return NEXTION_LOOP_OK;
          } else NEXTION_LOOP_NOEVENT;
        }
      }
    }
  }
  return NEXTION_LOOP_NULL;
}

int8_t Nextion::page() {
  write("sendme");
  uint8_t buffer[5] = {0};
  if (sizeof(buffer) == _serial->readBytes((char *)buffer, sizeof(buffer)))
    if ((buffer[0] == 0x66) && (buffer[2] == 0xFF) && (buffer[3] == 0xFF) && (buffer[4] == 0xFF)) return buffer[1];
  return NEXTION_PAGE_ERROR;
}

bool Nextion::show(uint8_t page) {
  return write("page " + String(page));
}

bool Nextion::sleep() {
  return write("sleep=1");
}

bool Nextion::wakeup() {
  return write("sleep=0");
}
