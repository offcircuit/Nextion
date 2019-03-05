#include "nextionUI.h"

Nextion nextion(10, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  nextion.begin();
  nextion.add({1, 1, NEXTION_EVENT_RELEASE}, cal);
  nextion.add({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.add({2, 2, NEXTION_EVENT_RELEASE}, two);
  nextion.add({2, 3, NEXTION_EVENT_RELEASE}, three);

  nextion.list();
  Serial.println("------------------------------------------------------- ");

  nextion.add({1, 1}, NEXTION_EVENT_RELEASE, call);
  nextion.add({2, 2}, NEXTION_EVENT_RELEASE, two_);
  nextion.list();
}

void three() {
  Serial.println("executing... ");
  Serial.println("three... ");
}

void call() {
  Serial.println("executing... ");
  Serial.println("call... ");
}

void two_() {
  Serial.println("executing... ");
  Serial.println("two...two ");
}

void two() {
  Serial.println("executing... ");
  Serial.println("two... ");
}

void clo() {
  Serial.println("executing... ");
  Serial.println("clock... ");
}

void cal() {
  Serial.println("executing... ");
  Serial.println("calendar... ");
}

void loop() {
  uint8_t t;
  if ((t =  nextion.listen()) != 0xFF) {
    Serial.println((t));
  }
}
