#include "Nextion.h"

Nextion nextion(10, 11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

 Serial.print("Start .... ");
  nextion.begin();
  delay(100);
   Serial.println("------------------------------------------------------- END");

  Serial.println(nextion.property("0x04030201"));
  size_t k = 500;
  uint8_t r[k];
  for (size_t i = 0; i < k; i++) {
    r[i] = random(254);
  }

  delay(100);
  Serial.println(nextion.wave(2, 0, r, k), HEX);
  /*
  */
  Serial.println(nextion.page());
  Serial.println("------------------------------------------------------- END");
}

void loop() {
  uint8_t tt =  nextion.listen();
  if (sizeof(tt)) {
    Serial.println(tt, HEX);
  }
  delay(2000);
}
