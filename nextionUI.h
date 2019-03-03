#ifndef NEXTION_H
#define NEXTION_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SoftwareSerial.h"
#include "nextionUIMSG.h"
#include "nextionUIEvents.h"

class Nextion {
  private:
    SoftwareSerial* _serial;
    uint8_t read();
    bool write(String command);
  public:
    NextionEvents callbacks;
    Nextion(uint8_t rx, uint8_t tx);
    uint8_t begin(uint32_t baud = 9600);

    int8_t loop();

    int8_t page();
    bool sleep();
    bool show(uint8_t page);
    bool wakeup();
};

#endif
