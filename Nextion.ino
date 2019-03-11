#include "Nextion.h"

Nextion nextion(10, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.print("Start .... at baud ");
  Serial.println(nextion.begin(57600));
  Serial.println("------------------------------------------------------- ");
  nextion.transmit("page 0");
  delay(100);
  nextion.reply(true);
  delay(100);
/*  Serial.println(nextion.transmit("bkcmd=3"), HEX);
  delay(100);
  Serial.println(nextion.transmit("sendxy=1"), HEX);
  delay(100);
  Serial.println(nextion.transmit("tDate.txt=\"12345678901234567890\""), HEX);
  delay(100);
  Serial.println("------------------------------------------------------- ");
  Serial.println(nextion.read("tDate.txt"));
  delay(100);
  Serial.println(nextion.transmit("get tDate.txt"), HEX);
  delay(100);
  Serial.println(nextion.read("tDate.txt"));
  delay(100);
  Serial.println(nextion.string);
  Serial.println("------------------------------------------------------- ");
  Serial.println(nextion.transmit("get bkcmd"), HEX);
  Serial.println(nextion.value);
  Serial.println(nextion.read("bkcmd"));
  out();
  //Serial.println(nextion.read("bkcmd"));
  /*
    delay(700);
    Serial.println(NEXBUF[0], HEX);
    Serial.println(NEXBUF[1], HEX);
    delay(100);
    Serial.println(nextion.read("tDate.txt"));*/

  Serial.println("------------------------------------------------------- ");

  nextion.attach({1, 1}, NEXTION_EVENT_RELEASE, cal);
  nextion.attach({1, 2, NEXTION_EVENT_RELEASE}, clo);
  nextion.attach({2, 7, NEXTION_EVENT_RELEASE}, seven);
  nextion.attach({2, 3, NEXTION_EVENT_RELEASE}, three);
}

void three() {
  Serial.println("three... ");
  Serial.println(nextion.sleep());
}

void seven() {
  Serial.println("seven... ");
  Serial.print("page -> ");
  Serial.println(nextion.page());
}

void clo() {
  Serial.println("clock... ");
  nextion.reboot();
  delay(1000);
  nextion.reply(true);
}

void cal() {
  Serial.println("calendar... ");
}

void loop() {
  uint8_t listen =  nextion.listen();
  if (listen) {
  if((nextion.command == NEXTION_CMD_TOUCH_COORDINATE_SLEEP) ||(nextion.command == NEXTION_CMD_TOUCH_COORDINATE_AWAKE) ) {
       Serial.print("  X ==  ");
        Serial.print(nextion.x);
        Serial.print("  Y ==  ");
        Serial.println(nextion.y);    
  }
    Serial.println(" listen *************************************************************************");
    Serial.println(nextion.command, HEX);
    Serial.println(" listen *************************************************************************");
    /*Serial.println(" listen *************************************************************************");
    switch (NEXBUF[0]) {
      case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
      case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
        Serial.print("  X == ");
        Serial.print(nextion.x);
        Serial.print(" Y ==");
        Serial.println(nextion.y);
    }*/
    //out();
  }
}

void out() {
  /*Serial.println("");
  Serial.print(" len -> ");
  Serial.println(NEXBUFLEN, HEX);
  Serial.print(" cmd -> ");
  Serial.println(NEXBUF[0], HEX);
  for (uint8_t i = 1; i < min(NEXBUFLEN, 16); i++) {
    Serial.print("  ");
    Serial.print(i);
    Serial.print("  0x");
    Serial.println((uint8_t)NEXBUF[i], HEX);
  }*/
}
