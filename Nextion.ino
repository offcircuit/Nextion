#include "Nextion.h"

Nextion nextion(10, 11);

void list() {
  Callback *item = nextion.callbacks;
  while (item) {
    Serial.print("Event ["); Serial.print(item->touch.page); Serial.print(", "); Serial.print(item->touch.id); Serial.print(", "); Serial.print(item->touch.event); Serial.println("]");
    item = item->next;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Start .... ");
  Serial.println(nextion.begin());

  nextion.event({1, 1, NEXTION_EVENT_RELEASE}, cal);
  nextion.event({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.event({2, 2, NEXTION_EVENT_RELEASE}, two);
  nextion.event({2, 3, NEXTION_EVENT_RELEASE}, three);
  nextion.event({2, 7, NEXTION_EVENT_RELEASE}, seven);
  list();

  Serial.println("------------------------------------------------------- ");
  nextion.event({1, 1, NEXTION_EVENT_RELEASE}, call);
  nextion.event({2, 2, NEXTION_EVENT_RELEASE}, two_);
  nextion.detach({2, 3, NEXTION_EVENT_RELEASE});
  list();
  Serial.println("------------------------------------------------------- ");
  nextion.event({2, 3, NEXTION_EVENT_RELEASE}, three);
  list();
  Serial.println("------------------------------------------------------- ");
  nextion.detach({2, 3, NEXTION_EVENT_RELEASE});
  list();
  Serial.println("------------------------------------------------------- ");
  nextion.detach({1, 1, NEXTION_EVENT_RELEASE});
  list();
}

void seven() {
  Serial.println("executing... ");
  Serial.println("seven... ");
  int p = nextion.current();
  Serial.print("page -> ");
  Serial.println(p, HEX);
}

void three() {
  Serial.println("executing... ");
  Serial.println("three... ");
  list();
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
  uint8_t tt =  nextion.listen();
  if (tt) {
    Serial.print(" cmd -> ");
    Serial.println(tt, HEX);

    for (uint8_t i = 0; i < nextion.__length; i++) {
      Serial.print("  ");
      Serial.print(i);
      Serial.print("  ");
      Serial.println((uint8_t)nextion.__buffer[i], DEC);
    }
  }
}
