#include "nextionUI.h"

Nextion::Nextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
};

bool Nextion::begin(uint16_t baud = 9600) {
  _serial->begin(baud);
  return transmit("page 0");
}

bool Nextion::transmit(String command) {
  _serial->print(String(command + char(0xFF) + char(0xFF) + char(0xFF)));
  return true;
}

Nextion::Callback *Nextion::callback(nexTouch touch, Pointer pointer) {
  Callback *item = new Callback;
  item->next = NULL;
  item->pointer = pointer;
  item->touch = touch;
  return item;
}

void Nextion::add(nexTouch touch, Pointer pointer) {
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

void Nextion::add(nexComponent component, nexEvent event, Pointer pointer) {
  add({component.page, component.id, event}, pointer);
}

size_t Nextion::receipt(char *buffer, size_t size) {
  if (_serial->readBytes((char*)buffer, size))
    for (uint8_t i = 0; i < 3; i++) size *= ((uint8_t)_serial->read() == 0xFF);
  return size;
}

uint8_t Nextion::listen() {
  if (_serial->available() > 3) {

    switch (_serial->read()) {

      case (NEXTION_CMD_STARTUP >> 8): {
          uint8_t buffer[(NEXTION_CMD_STARTUP & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_STARTUP & 0x00FF) - 4)) && (buffer[0] == 0x00) && (buffer[1] == 0x00)) return (NEXTION_CMD_STARTUP >> 8);
        }
        break;

      case (NEXTION_CMD_SERIAL_BUFFER_OVERFLOW >> 8): {
          uint8_t buffer[(NEXTION_CMD_SERIAL_BUFFER_OVERFLOW & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_SERIAL_BUFFER_OVERFLOW & 0x00FF) - 4)) && (buffer[0] == 0xFF) && (buffer[1] == 0xFF)) return (NEXTION_CMD_SERIAL_BUFFER_OVERFLOW >> 8);
        }
        break;

      case (NEXTION_CMD_TOUCH_EVENT >> 8): {
          uint8_t buffer[(NEXTION_CMD_TOUCH_EVENT & 0x00FF) - 4];
          if (receipt(buffer, (NEXTION_CMD_TOUCH_EVENT & 0x00FF) - 4)) {
            Callback *item = callbacks;
            while (item) {
              if ((item->touch.page == buffer[0]) && (item->touch.id == buffer[1]) && (item->touch.event == buffer[2])) {
                item->pointer();
              }
              item = item->next;
            }
          }
          return (NEXTION_CMD_TOUCH_EVENT >> 8);
        }
        break;

      case (NEXTION_CMD_TOUCH_COORDINATE_AWAKE >> 8): {
          uint8_t buffer[(NEXTION_CMD_TOUCH_COORDINATE_AWAKE & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_TOUCH_COORDINATE_AWAKE & 0x00FF) - 4))) {
            _x = word(buffer[0], buffer[1]);
            _y = word(buffer[2], buffer[3]);
            return (NEXTION_CMD_TOUCH_COORDINATE_AWAKE >> 8);
          }
        }
        break;

      case (NEXTION_CMD_TOUCH_COORDINATE_SLEEP >> 8): {
          uint8_t buffer[(NEXTION_CMD_TOUCH_COORDINATE_SLEEP & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_TOUCH_COORDINATE_SLEEP & 0x00FF) - 4))) {
            _x = word(buffer[0], buffer[1]);
            _y = word(buffer[2], buffer[3]);
            return (NEXTION_CMD_TOUCH_COORDINATE_SLEEP >> 8);
          }
        }
        break;

      case (NEXTION_CMD_AUTO_ENTER_SLEEP >> 8): {
          uint8_t buffer[(NEXTION_CMD_AUTO_ENTER_SLEEP & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_AUTO_ENTER_SLEEP & 0x00FF) - 4))) return (NEXTION_CMD_AUTO_ENTER_SLEEP >> 8);
        }
        break;

      case (NEXTION_CMD_AUTO_ENTER_WAKEUP >> 8): {
          uint8_t buffer[(NEXTION_CMD_AUTO_ENTER_WAKEUP & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_AUTO_ENTER_WAKEUP & 0x00FF) - 4))) return (NEXTION_CMD_AUTO_ENTER_WAKEUP >> 8);
        }
        break;

      case (NEXTION_CMD_READY >> 8): {
          uint8_t buffer[(NEXTION_CMD_READY & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_READY & 0x00FF) - 4))) return (NEXTION_CMD_READY >> 8);
        }
        break;

      case (NEXTION_CMD_START_MICROSD_UPDATE >> 8): {
          uint8_t buffer[(NEXTION_CMD_START_MICROSD_UPDATE & 0x00FF) - 4];
          if ((receipt(buffer, (NEXTION_CMD_START_MICROSD_UPDATE & 0x00FF) - 4))) return (NEXTION_CMD_START_MICROSD_UPDATE >> 8);
        }
        break;
    }
  }
  return 0xFF;
}
