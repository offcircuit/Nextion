#include "Nextion.h"

Nextion nextion(10, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.print("Start .... at baud ");
  Serial.print("no baud ");
  Serial.println(nextion.begin(9600));
  Serial.println("------------------------------------------------------- ");
  Serial.println(nextion.page(), HEX);
  Serial.println(nextion.page(1), HEX);
  Serial.println(nextion.page(), HEX);
  Serial.println(nextion.transmit("page 0"), HEX);
  Serial.println(nextion.transmit("tDate.txt=\"12345678900987654321\""), HEX);
  Serial.println(nextion.transmit("get tDate.txt"), HEX);
  Serial.println(nextion.transmit("get bkcmd"), HEX);
  Serial.println(nextion.read("tDate.txt"));


  nextion.event({1, 1}, NEXTION_EVENT_RELEASE, cal);
  nextion.event({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.event({2, 7, NEXTION_EVENT_RELEASE}, seven);
}

void seven() {
  Serial.println("seven... ");
  Serial.print("page -> ");
  Serial.println(nextion.page(), HEX);
}

void clo() {
  Serial.println("clock... ");
}

void cal() {
  Serial.println("calendar... ");
}

void loop() {
  uint8_t listen =  nextion.listen();
  if (listen) {
    Serial.print(" cmd -> ");
    Serial.println(NEXBUF[0], HEX);
  }
}
