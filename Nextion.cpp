#include "Nextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t INextion::begin(uint32_t speed) {
  const uint8_t rate[8] = {1, 2, 4, 8, 16, 24, 48, 0};
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

uint8_t INextion::write(String data) {
  uint8_t available = 255;
  _serial->print(data + char(0xFF) + char(0xFF) + char(0xFF));
  while (--available && (!_serial->available()));
  return available;
}
