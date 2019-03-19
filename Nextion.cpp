#include "Nextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t INextion::begin(uint32_t speed = 0) {
  const uint8_t rate[8] = {1, 2, 4, 8, 16, 24, 48, 0};
  uint8_t baud = 0;

  do _serial->begin(rate[baud] * 2400UL);
  while ((write("connect") == "") && (7 > ++baud));

  if (speed) {
    write("baud=" + String(speed));
    _serial->begin(speed);
    return speed;
  }

  return (rate[baud] * 2400UL);
}

String INextion::read() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  uint16_t length = 0;
  String string = "";

  do if (_serial->available() && (string += char(_serial->read())) && length++) cycle = NEXTION_SERIAL_CYCLES;
  while (cycle-- && ((length < 4) || (((char)string[length - 1] & (char)string[length - 2] & (char)string[length - 3]) != 0xFF)));

  if (cycle) switch ((uint8_t)string[0]) {

      case NEXTION_CMD_STRING_DATA_ENCLOSED:
        return string.substring(1, length - 3);

      case NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
        return String((uint32_t(string[4]) << 24) + (uint32_t(string[3]) << 16) + (uint32_t(string[2]) << 8) + uint8_t(string[1]));

      case NEXTION_CMD_CURRENT_PAGE:
        return String((uint8_t)string[1]);

      default:
        return string;
    }
    
  return "";
}

bool INextion::wait() {
  uint8_t cycle = NEXTION_SERIAL_CYCLES;
  while (--cycle && (!_serial->available()));
  return cycle;
}

String INextion::wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
  if (write("addt " + String(id) + "," + String(channel) + "," + String(length)) == String(NEXTION_CMD_TRANSPARENT_DATA_READY)) {
    for (size_t i = 0; i < length;) _serial->write(data[i++]);
    if (wait()) return read();
  }
  return "";
}

String INextion::write(String instruction) {
  if (_serial->print(instruction + char(0xFF) + char(0xFF) + char(0xFF)) && wait()) return read();
  return "";
}
