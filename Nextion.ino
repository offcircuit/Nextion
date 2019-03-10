#include "Nextion.h"

Nextion nextion(10, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.print("Start .... at baud ");
  Serial.println(nextion.begin(19200));
  out();
  Serial.println("------------------------------------------------------- ");
  delay(100);
  Serial.println(nextion.read("bkcmd"));
  //Serial.println(nextion.page(), HEX);
  //Serial.println(nextion.read("bkcmd"));
  /*Serial.println(nextion.transmit("get tDate.txt"), HEX);
  delay(700);
  Serial.println(NEXBUF[0], HEX);
  Serial.println(NEXBUF[1], HEX);
  delay(100);
  Serial.println(nextion.read("tDate.txt"));*/

  Serial.println("------------------------------------------------------- ");

  nextion.event({1, 1}, NEXTION_EVENT_RELEASE, cal);
  nextion.event({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.event({2, 7, NEXTION_EVENT_RELEASE}, seven);
}

void seven() {
  Serial.println("seven... ");
  Serial.print("page -> ");
  Serial.println(nextion.page());
}

void clo() {
  Serial.println("clock... ");
  nextion.reset();
}

void cal() {
  Serial.println("calendar... ");
}

void loop() {
  uint8_t listen =  nextion.listen();
  if (listen) {
    out();
  }
}
