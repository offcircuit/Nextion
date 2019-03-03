#include "nextionUI.h"

Nextion nextion(10, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  nextion.begin();
  nextion.callbacks.add({1, 1, NEXTION_EVENT_RELEASE}, cal);
  nextion.callbacks.add({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.callbacks.add({2, 2, NEXTION_EVENT_RELEASE}, two);
  nextion.callbacks.add({2, 3, NEXTION_EVENT_RELEASE}, three);

  Serial.println("------------------------------------------------------- ");


  nextion.callbacks.add({1, 1, NEXTION_EVENT_RELEASE}, call);
  nextion.callbacks.add({2, 2, NEXTION_EVENT_RELEASE}, two_);
  Serial.print("page... ");
  Serial.println(nextion.page());
}

void three() {
  Serial.println("executing... ");
  Serial.println("three... ");
  nextion.callbacks.list();
  Serial.print("page... ");
  Serial.println(nextion.page());
  nextion.show(0);
  Serial.print("page... ");
  Serial.println(nextion.page());
}

void call() {
  Serial.println("executing... ");
  Serial.println("call... ");
  nextion.callbacks.add({2, 2, NEXTION_EVENT_RELEASE}, two_);
  Serial.print("page... ");
  Serial.println(nextion.page());
  nextion.show(1);
  Serial.print("page... ");
  Serial.println(nextion.page());
}

void two_() {
  Serial.println("executing... ");
  Serial.println("two...two ");
  nextion.callbacks.remove({2, 2, NEXTION_EVENT_RELEASE});
  nextion.callbacks.list();
  Serial.print("page... ");
  Serial.println(nextion.page());
}

void two() {
  Serial.println("executing... ");
  Serial.println("two... ");
}

void clo() {
  Serial.println("executing... ");
  Serial.println("clock... ");
  Serial.print("page... ");
  Serial.println(nextion.page());
}

void cal() {
  Serial.println("executing... ");
  Serial.println("calendar... ");
}

void loop() {
  // put your main code here, to run repeatedly:
  nextion.loop();
}
