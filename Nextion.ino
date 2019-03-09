#include "Nextion.h"

Nextion nextion(10, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.print("Start .... ");
  Serial.println(nextion.begin());
  Serial.println("------------------------------------------------------- ");

  Serial.println("transmit");
  
  Serial.println(nextion.transmit("tDate.txt=\"56etre801\""), HEX);

  Serial.println("read");
  Serial.println(nextion.read("tDate.txt"));
  Serial.println("------------------------------------------------------- ");
 /*     for (uint8_t i = 1; i < NEXBUFLEN; i++) {
      Serial.print("  ");
      Serial.print(i);
      Serial.print("  ");
      Serial.print((uint8_t)NEXBUF[i], DEC);
      Serial.print("  (");
      Serial.print(char(NEXBUF[i]));
      Serial.println(")  ");
    }*/


  nextion.event({1, 1}, NEXTION_EVENT_RELEASE, cal);
  nextion.event({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.event({2, 2, NEXTION_EVENT_RELEASE}, two);
  nextion.event({2, 3, NEXTION_EVENT_RELEASE}, three);
  nextion.event({2, 7, NEXTION_EVENT_RELEASE}, seven);
}

void seven() {
  Serial.println("seven... ");
  int p = nextion.page();
  Serial.print("page -> ");
  Serial.println(p, HEX);
}

void three() {
  Serial.println("three... ");
}

void two() {
  Serial.println("two... ");
}

void clo() {
  Serial.println("clock... ");
}

void cal() {
  Serial.println("calendar... ");
}

void loop() {
  uint8_t tt =  nextion.listen();
  if (tt) {
    Serial.print(" cmd -> ");
    Serial.println(tt, HEX);

    if (NEXBUF[2] == 3) nextion.reset();
    if (NEXBUF[2] == 4) {
    }
    for (uint8_t i = 1; i < NEXBUFLEN; i++) {
      Serial.print("  ");
      Serial.print(i);
      Serial.print("  ");
      Serial.println((uint8_t)NEXBUF[i], DEC);
    }
  }
}
