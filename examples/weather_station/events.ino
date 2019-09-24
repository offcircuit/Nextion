void configEvent(uint8_t page, uint8_t id, bool state) {
  if (event = (2 - id)) sendFormat(text = decimate(dtime.day) + decimate(dtime.month) + dtime.year);
  else sendFormat(text = decimate(dtime.hour) + decimate(dtime.minute) + decimate(dtime.second));
}

void numericEvent(uint8_t page, uint8_t id, bool state) {
  sendFormat(text = String(text + String(id - ((id / 10) * 10))).substring(1));
}

void OKEvent(uint8_t page, uint8_t id, bool state) {
  if (event) dtime.setDate(text.substring(4).toInt(), text.substring(2, 4).toInt(), text.substring(0, 2).toInt());
  else dtime.setTime(text.substring(0, 2).toInt(), text.substring(2, 4).toInt(), text.substring(4).toInt());
  sendData();
}
