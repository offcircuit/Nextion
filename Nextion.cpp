#include "Nextion.h"

INextion::INextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t INextion::begin(uint32_t speed) {
  const uint8_t rate[8] = {1, 2, 4, 8, 16, 24, 48, 0};
  uint8_t baud = 0;

  _serial->setTimeout(10);
  do _serial->begin(rate[baud] * 2400UL);
  while (!write("connect").length() && (7 > ++baud));

  if (speed) {
    write("baud=" + String(speed));
    _serial->begin(speed);
    return speed;
  }
  return (rate[baud] * 2400UL);
}

String INextion::read() {
   String string = "";

  if (string = _serial->readStringUntil(char(0xFF))) {

    switch ((uint8_t)string[0]) {

      case NEXTION_CMD_STRING_DATA_ENCLOSED:
        return string.substring(1, string.length() - 3);

      case NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
        return String((uint32_t(string[4]) << 24) + (uint32_t(string[3]) << 16) + (uint32_t(string[2]) << 8) + uint8_t(string[1]));

      case NEXTION_CMD_CURRENT_PAGE:
        return String(string[1], DEC);

      default:
        Serial.println(reading_(string));
        Serial.println((string.length()));
        return string;
    }
    return "";
  }
}

String INextion::wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
  if (write("addt " + String(id) + "," + String(channel) + "," + String(length)) == String(NEXTION_CMD_TRANSPARENT_DATA_READY)) {
    for (size_t i = 0; i < length;) _serial->write(data[i++]);
    return read();
  }
}

String INextion::write(String instruction) {
  _serial->print(instruction + char(0xFF) + char(0xFF) + char(0xFF));
  return read();
}
