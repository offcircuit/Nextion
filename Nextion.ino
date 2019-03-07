#include "Nextion.h"

Nextion nextion(10, 11);
nextionComponent bt {1, 1};
nextionTouch bt2 {1, 2, NEXTION_EVENT_RELEASE};

uint32_t r(int8_t b1, int8_t b2, int8_t b3, int8_t b4)
{
  uint32_t tt = (uint32_t)b4 << 24 | (uint32_t)b3 << 16 | (uint32_t)b2 << 8 | b1;
  return  tt;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  Serial.print("Start .... ");
  Serial.println(nextion.begin());
  delay(100);
  Serial.println("------------------------------------------------------- ");

  nextion.transmit("home.tDate.txt=\"hello\"");
  for (uint8_t i = 0; i < nextion.__length__; i++) {
    Serial.print("  ");
    Serial.print(i);
    Serial.print("  ");
    Serial.println((uint8_t)nextion.__buffer__[i], HEX);
  }

  Serial.println("------------------------------------------------------- ");

  Serial.print("home.tDate.objname =  ");
  Serial.println(nextion.getAttribute("home.tDate.txt"));
  Serial.print("len =  ");
  Serial.println(nextion.__length__);

  for (uint8_t i = 0; i < nextion.__length__; i++) {
    Serial.print("  ");
    Serial.print(i);
    Serial.print("  ");
    Serial.println((uint8_t)nextion.__buffer__[i], HEX);
  }

  Serial.println("------------------------------------------------------- ");

  Serial.print("len =  ");
  Serial.println(nextion.__length__);

  Serial.println("------------------------------------------------------- ");

  for (uint8_t i = 0; i < nextion.__length__; i++) {
    Serial.print("  ");
    Serial.print(i);
    Serial.print("  ");
    Serial.println((uint8_t)nextion.__buffer__[i], HEX);
  }

  Serial.println("------------------------------------------------------- ");


  nextion.event(bt, NEXTION_EVENT_RELEASE, cal);
  nextion.event(bt2, clo);
  nextion.event({2, 2, NEXTION_EVENT_RELEASE}, two);
  nextion.event({2, 3, NEXTION_EVENT_RELEASE}, three);
  nextion.event({2, 7, NEXTION_EVENT_RELEASE}, seven);

  Serial.println("------------------------------------------------------- ");
  nextion.event({1, 1, NEXTION_EVENT_RELEASE}, call);
  nextion.event({2, 2, NEXTION_EVENT_RELEASE}, two_);
  nextion.detach({2, 3, NEXTION_EVENT_RELEASE});
  Serial.println("------------------------------------------------------- ");
  nextion.event({2, 3, NEXTION_EVENT_RELEASE}, three);
  Serial.println("------------------------------------------------------- ");
  nextion.detach({2, 3, NEXTION_EVENT_RELEASE});
  Serial.println("------------------------------------------------------- ");
  nextion.detach({1, 1, NEXTION_EVENT_RELEASE});
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
  //  list();
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

    for (uint8_t i = 0; i < nextion.__length__; i++) {
      Serial.print("  ");
      Serial.print(i);
      Serial.print("  ");
      Serial.println((uint8_t)nextion.__buffer__[i], DEC);
    }
  }
}
