#include "nextionI.h"

NextionI nextion(10, 11);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  nextion.begin();

  nextion.event({1, 1, NEXTION_EVENT_RELEASE}, cal);
  nextion.event({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.event({2, 2, NEXTION_EVENT_RELEASE}, two);
  nextion.event({2, 3, NEXTION_EVENT_RELEASE}, three);
  nextion.event({2, 7, NEXTION_EVENT_RELEASE}, seven);

  Serial.println("------------------------------------------------------- ");

  nextion.event({1, 1}, NEXTION_EVENT_RELEASE, call);
  nextion.event({2, 2}, NEXTION_EVENT_RELEASE, two_);
}

void seven() {
  //Serial.println("executing... ");
  //Serial.println("seven... ");
  int p = nextion.current();
  Serial.print("page -> ");
  Serial.println(p, HEX);
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
  uint8_t *t =  nextion.listen();
  if (t) {
    int p = nextion.current();
    Serial.print(" loop page -> ");
    Serial.println(p, HEX);
    Serial.print(" cmd == ");
      Serial.println(t[0]);
     /* Serial.print(" 2 ");
      Serial.println(t[1]);
      Serial.print(" 3 ");
      Serial.println(t[2]);
      Serial.print(" 4 ");
      Serial.println(t[3]);
      Serial.print(" 5 ");
      Serial.println(t[4]);
      Serial.print(" 6 ");
      Serial.println(t[5]);
      Serial.print(" 7 ");
      Serial.println(t[6]);
      Serial.print(" 8 ");
      Serial.println(t[7]);
      Serial.print(" 9 ");
      Serial.println(t[8]);*/
  }
}
