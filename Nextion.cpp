#include "Nextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t INextion::begin(uint32_t speed) {
  const uint8_t rate[8] = {1, 2, 4, 8, 16, 24, 48};
  uint8_t baud = 0;

  do _serial->begin(rate[baud] * 2400UL);
  while (!write("connect") && (7 > ++baud));

  if (speed) {
    write("baud=" + String(speed));
    _serial->begin(speed);
    return speed;
  }

  return (rate[baud] * 2400UL);
}

char* INextion::read() {
  char data[1024] = {};
  uint16_t length = 0;
  do while (_serial->available()) data[length++] = _serial->read();
  while ((data[length - 1] & data[length - 2] & data[length - 3]) != 0xFF);
  return data;
}

bool INextion::wait() {
  uint8_t cycle = 255;
  while (--cycle && (!_serial->available()));
  return cycle;
}

char* INextion::wave(uint8_t id, uint8_t channel, uint8_t *buffer, size_t length) {
  char* data = write("addt " + String(id) + "," + String(channel) + "," + String(length));
  if (data[0] == NEXTION_CMD_TRANSPARENT_DATA_READY) {
    for (size_t i = 0; i < length;) _serial->write(buffer[i++]);
    if (wait()) return read();
  }
  return data;
}

char* INextion::write(String data) {
  if (_serial->print(String(data + char(0xFF) + char(0xFF) + char(0xFF))) && wait()) return read();
}
