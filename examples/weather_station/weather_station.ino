#include <DTime.h>
#include <Nextion.h>
#include <SDHT.h>

bool event = 0;
String text = "";
uint32_t t = millis();
DTime dtime;
Nextion nextion(5, 4);
SDHT dht;

void setup() {
  Serial.begin(115200);
  nextion.begin();
  for (int i = 1; i <= 10; i++) nextion.attach({2, i, NEXTION_EVENT_RELEASE}, numericEvent);
  nextion.attach({2, 11, NEXTION_EVENT_RELEASE}, OKEvent);
  nextion.attach({1, 2, NEXTION_EVENT_RELEASE}, configEvent);
  nextion.attach({1, 1, NEXTION_EVENT_RELEASE}, configEvent);
  sendData();
}

void loop() {
  if (t + 1000 < millis()) {
    sendData();
    dtime.tick();
    t = millis();
  }
  nextion.listen();
}
