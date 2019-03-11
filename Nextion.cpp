#include "Nextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t INextion::begin(uint32_t speed = 0) {
  const uint8_t rate[8] = {1, 2, 4, 8, 16, 24, 48, 0};
  uint8_t baud = 0;

  do _serial->begin(rate[baud] * 2400UL);
  while (!transmit("connect") && (7 > ++baud));

  if (speed) {
    transmit("bauds=" + String(speed));
    _serial->begin(speed);
    return speed;
  }
  return (rate[baud] * 2400UL);
}

bool INextion::response() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  uint16_t length = 0;

  if (_command == NEXTION_CMD_STRING_DATA_ENCLOSED) {
    do if (_serial->available() && (_string += (char)_serial->read()) && length++) cycle = NEXTION_SERIAL_CYCLES;
    while (cycle-- && ((length < 3) || ((_string[length - 1] & _string[length - 2] & _string[length - 3]) != 0xFF)));
    return (cycle && (_string = _string.substring(0, length - 3)));
  }

  do if (_serial->available()) _buffer[length++] = _serial->read();
  while (cycle-- && ((length < 3) || ((_buffer[length - 1] & _buffer[length - 2] & _buffer[length - 3]) != 0xFF)));

  if (cycle) {
    if (_command == NEXTION_CMD_NUMERIC_DATA_ENCLOSED) _value = (((uint32_t)_buffer[3] << 24) | ((uint32_t)_buffer[2] << 16) | ((uint32_t)_buffer[1] << 8) | _buffer[0]);
    return true;
  }
  return false;
}

bool INextion::wait() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  while (--cycle && (!_serial->available()));
  return cycle;
}

uint8_t INextion::transmit(String instruction) {
  if (_serial->print(instruction + char(0xFF) + char(0xFF) + char(0xFF)) && wait() && (_command = _serial->read()) && response()) return _command;
  return 0;
}
