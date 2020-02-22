#include "Nextion.h"

Nextion::Nextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t Nextion::begin(uint32_t speed) {
  uint32_t rate = baud();
  if (speed && (speed != rate)) return setBaud(speed);
  return rate;
}

void Nextion::attach() {
  _callbacks = NULL;
}

void Nextion::attach(nextionComponent component, bool state, nextionOnEvent pointer) {
  attach({component.page, component.id, state}, pointer);
}

void Nextion::attach(nextionEvent event, nextionOnEvent pointer) {
  if (_callbacks) {
    nextionCallback *item = _callbacks;
    do if ((item->event.page == event.page) && (item->event.id == event.id) && (item->event.state == event.state)) {
        item->pointer = pointer;
        return;
      } while (item->next && (item = item->next));
    item->next = callback(event, pointer);
  } else _callbacks = callback(event, pointer);
}

uint8_t Nextion::backlight(uint8_t value) {
  return print("dim=" + String(value));
}

uint32_t Nextion::baud() {
  const uint8_t map[8] = {48, 24, 16, 8, 4, 2, 1, 0};
  uint8_t index = 0;

  do _serial->begin(map[index] * 2400UL);
  while (!connect() && (7 > ++index));

  return map[index] * 2400UL;
}

uint8_t Nextion::bkcmd(uint8_t mode) {
  return print("bkcmd=" + String(mode));
}

uint8_t Nextion::brush(uint16_t c) {
  return print("thc=" + String(c));
}

Nextion::nextionCallback *Nextion::callback(nextionEvent event, nextionOnEvent pointer) {
  nextionCallback *item = new nextionCallback;
  item->next = NULL;
  item->pointer = pointer;
  item->event = event;
  return item;
}

uint8_t Nextion::circle(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
  return print("cir " + String(x) + "," + String(y) + "," + String(r) + "," + String(c));
}

uint8_t Nextion::clear(uint16_t c) {
  return print("cls " + String(c));
}

uint8_t Nextion::click(uint8_t id, bool event) {
  return print("click " + String(id) + "," + String(event));
}

bool Nextion::connect() {
  flush();
  send("DRAKJHSUYDGBNCJHGJKSHBDN");
  send("connect");
  send(String(char(0xFF) + char(0xFF)) + "connect");
  flush();
  send("");
  flush();
  send("connect");

  if (readln()) return _data.indexOf("comok") != -1;
}

size_t Nextion::content(uint8_t *&buffer) {
  if (_buffer[0] == NEXTION_CMD_STRING_DATA_ENCLOSED) {
    buffer = (uint8_t *) malloc(_data.length() + 2);
    String(String(char(NEXTION_CMD_STRING_DATA_ENCLOSED)) + _data).toCharArray((char *)buffer, _data.length() + 2);
    return _data.length() + 1;

  }  else if (_data.length()) {
    buffer = (uint8_t *) malloc(_data.length() + 1);
    String(_data).toCharArray((char *)buffer, _data.length() + 1);
    return _data.length();

  } else  {
    buffer = (uint8_t *) malloc(_length);
    memcpy(buffer, _buffer, _length);
    return _length;
  }
}

uint8_t Nextion::crop(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t resource) {
  return print("picq " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," + String(resource));
}

uint8_t Nextion::crop(uint16_t dx, uint16_t dy, uint16_t w, uint16_t h, uint16_t sx, uint16_t sy, uint8_t resource) {
  return print("xpic " + String(dx) + "," + String(dy) + "," + String(w) + "," + String(h) + "," + String(sx) + "," + String(sy) + "," + String(resource));
}

uint8_t Nextion::delay(uint16_t milliseconds) {
  return print("Delay=" + String(milliseconds));
}

void Nextion::detach() {
  _callbacks = NULL;
}

void Nextion::detach(nextionComponent component, bool state) {
  detach({component.page, component.id, state});
}

void Nextion::detach(nextionEvent event) {
  if (_callbacks) {
    nextionCallback *item = _callbacks, *preview;
    do if ((item->event.page == event.page) && (item->event.id == event.id) && (item->event.state == event.state)) {
        if (item == _callbacks) _callbacks = _callbacks->next;
        else preview->next = ((item->next) ? item->next : NULL);
        return;
      } while (item->next && (preview = item) && (item = item->next));
  }
}

uint8_t Nextion::disable(uint8_t id) {
  return print("tsw " + String(id) + ",0");
}

uint8_t Nextion::draw(bool state) {
  return print("thdra=" + String(state));
}

uint8_t Nextion::enable(uint8_t id) {
  return print("tsw " + String(id) + ",1");
}

uint8_t Nextion::erase(uint8_t id) {
  return print("cle " + String(id) + ",255");
}

uint8_t Nextion::erase(uint8_t id, uint8_t channel) {
  return print("cle " + String(id) + "," + String(channel));
}

uint8_t Nextion::fillCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t c) {
  return print("cirs " + String(x) + "," + String(y) + "," + String(r) + "," + String(c));
}

uint8_t Nextion::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c) {
  return print("fill " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," + String(c));
}

void Nextion::flush() {
  _signal = NEXTION_SERIAL_CYCLES;
  do while (_serial->available()) {
      _serial->read();
      _signal = NEXTION_SERIAL_CYCLES;
    } while (_signal--);
}

String Nextion::get(String data) {
  switch (print("get " + data)) {

    case NEXTION_CMD_STRING_DATA_ENCLOSED:
      return _data.substring(0, _data.length() - 3);

    case NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
      return String((uint32_t(_buffer[4]) << 24) + (uint32_t(_buffer[3]) << 16) + (uint32_t(_buffer[2]) << 8) + uint8_t(_buffer[1]));
  }
  return String(_buffer[0]);
}

uint8_t Nextion::hide(uint8_t id) {
  return print("vis " + String(id) + ",0");
}

uint8_t Nextion::line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
  return print("line " + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "," + String(c));
}

int16_t Nextion::listen() {
  if (_serial->available() > 3)
    if (read()) {
      uint16_t data = int16_t(_buffer[0]);
      switch (data) {

        case NEXTION_CMD_STARTUP:
          if (_onStart) _onStart();
          break;

        case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
        case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
          if (_onTouch) _onTouch((uint16_t(_buffer[1]) << 8) | uint8_t(_buffer[2]), (uint16_t(_buffer[3]) << 8) | uint8_t(_buffer[4]), _buffer[5]);
          break;

        case NEXTION_CMD_TOUCH_EVENT: {
            nextionCallback *item = _callbacks;
            while (item) {
              if ((item->event.page == _buffer[1]) && (item->event.id == _buffer[2]) && (item->event.state == _buffer[3]) && (item->pointer)) {
                item->pointer(_buffer[1], _buffer[2], _buffer[3]);
                break;
              }
              item = item->next;
            }
          }
          break;

        case NEXTION_CMD_AUTO_ENTER_SLEEP:
        case NEXTION_CMD_AUTO_ENTER_WAKEUP:
          if (_onChange) _onChange(_buffer[0] == NEXTION_CMD_AUTO_ENTER_WAKEUP);
          break;

        case NEXTION_CMD_READY:
          if (_onReady) _onReady();
          break;

        case NEXTION_CMD_START_MICROSD_UPDATE:
          if (_onUpdate) _onUpdate();
          break;
      }
      return data;
    }
  return -1;
}

void Nextion::onChange(nextionOnChange pointer) {
  _onChange = pointer;
}

void Nextion::onReady(nextionOnPointer pointer) {
  _onReady = pointer;
}

void Nextion::onStart(nextionOnPointer pointer) {
  _onStart = pointer;
}

void Nextion::onTouch(nextionOnTouch pointer) {
  _onTouch = pointer;
}

void Nextion::onUpdate(nextionOnPointer pointer) {
  _onUpdate = pointer;
}

bool Nextion::open(size_t length) {
  uint32_t rate = baud();
  if (rate) {
    _index = 0;
    _map = length;
    send("whmi-wri " + String(_map) + "," + String(rate) + ",0");
    _signal = NEXTION_SERIAL_CYCLES;
    while ((uint8_t(_serial->read()) != 0x05) && _signal--);
    return _signal;
  }
}

int16_t Nextion::page() {
  if (print("sendme") == NEXTION_CMD_CURRENT_PAGE) return uint8_t(_buffer[1]);
  else return -1;
}

uint8_t Nextion::page(uint8_t page) {
  return print("page " + String(page));
}

uint8_t Nextion::picture(uint16_t x, uint16_t y, uint8_t resource) {
  return print("pic " + String(x) + "," + String(y) + "," + String(resource));
}

uint8_t Nextion::print(String data) {
  flush();
  send(data);
  if (read()) return _buffer[0];
}

uint8_t Nextion::read() {
  uint8_t exit = 0;
  restore();
  do if (_serial->available()) {
      uint8_t data = uint8_t(_serial->read());
      _signal = NEXTION_SERIAL_CYCLES;

      switch (_buffer[0]) {

        case NEXTION_CMD_STRING_DATA_ENCLOSED:
          _data += char(data);
          _length = 1;
          exit = (data == 0xFF) * (exit + 1);
          break;

        case NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
          _buffer[_length++] = uint8_t(data);
          exit = (_length == 8) * 3;
          break;

        default:
          _buffer[_length++] = uint8_t(data);
          exit = (data == 0xFF) * (exit + 1);
      }
    } while (_signal-- && (exit != 3));
  return _length;
}

uint8_t Nextion::readln() {
  restore();
  do while (_serial->available()) {
      _data += char(_serial->read());
      _signal = NEXTION_SERIAL_CYCLES;
    } while (_signal--);
  return _data.length();
}

uint8_t Nextion::rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
  return print("draw " + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + "," + String(c));
}

uint8_t Nextion::reply(bool state) {
  return print("thup=" + String(state));
}

uint8_t Nextion::reset() {
  return print("rest");
}

void Nextion::restore() {
  _buffer[0] = 0x00;
  _data = "";
  _length = 0;
  _signal = NEXTION_SERIAL_CYCLES;
}

void Nextion::send(String data) {
  _serial->print(data + char(0xFF) + char(0xFF) + char(0xFF));
}

uint8_t Nextion::sendxy(bool state) {
  return print("sendxy=" + String(state));
}

uint32_t Nextion::setBaud(uint32_t speed) {
  flush();
  send("baud=" + String(speed));
  return baud();
}

uint8_t Nextion::show(uint8_t id) {
  return print("vis " + String(id) + ",1");
}

uint8_t Nextion::sleep() {
  return print("sleep=1");
}

uint8_t Nextion::text(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t font, uint16_t foreground, uint16_t background, uint8_t alignX, uint8_t alignY, uint8_t fill, String text) {
  return print("xstr " + String(x) + "," + String(y) + "," + String(w) + "," + String(h) + "," +
               String(font) + "," + String(foreground) + "," + String(background) + "," + String(alignX) + "," + String(alignY) + "," + String(fill) + "," + text);
}

bool Nextion::upload(uint8_t *buffer, size_t length) {
  if (open(length))
    do if (!write(buffer[_index++])) return false;
    while (_index < length);
  return true;
}

uint8_t Nextion::waitSerial(uint16_t seconds) {
  return print("ussp=" + String(seconds));
}

uint8_t Nextion::waitTouch(uint16_t seconds) {
  return print("thsp=" + String(seconds));
}

uint8_t Nextion::wakeup() {
  return print("sleep=0");
}

uint8_t Nextion::wakeupPage(uint8_t page) {
  return print("wup=" + String(page));
}

uint8_t Nextion::wave(uint8_t id, uint8_t channel, uint8_t data) {
  return print("add " + String(id) + "," + String(channel) + "," + String(data));
}

uint8_t Nextion::wave(uint8_t id, uint8_t channel, uint8_t *data, size_t length) {
  if (print("addt " + String(id) + "," + String(channel) + "," + String(length)) == NEXTION_CMD_TRANSPARENT_DATA_READY)
    for (size_t i = 0; i < length;) _serial->write(data[i++]);
  if (read()) return _buffer[0];
}

bool Nextion::write(uint8_t data) {
  if (_index < _map) {
    _serial->write(data);
    if (!(_index++ % 4096) || (_index == _map)) {
      _signal = NEXTION_SERIAL_CYCLES;
      while ((uint8_t(_serial->read()) != 0x05) && _signal--);
      return _signal;
    }
    return true;
  }
}
