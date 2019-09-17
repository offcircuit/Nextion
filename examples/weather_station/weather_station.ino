#include <DTime.h>
#include <Nextion.h>
#include <SDHT.h>

bool event = 0;
String text = "";

DTime dtime;
Nextion nextion(4, 5);
SDHT dht;

void setup() {
  nextion.begin(9600);

  for (int i = 1; i <= 10; i++) nextion.attach({2, i, NEXTION_EVENT_RELEASE}, numericEvent);
  nextion.attach({2, 11, NEXTION_EVENT_RELEASE}, OKEvent);
  nextion.attach({1, 2, NEXTION_EVENT_RELEASE}, configEvent);
  nextion.attach({1, 1, NEXTION_EVENT_RELEASE}, configEvent);

  sendData();
}

void loop() {
  if (!(millis() % 1000)) {
    dtime.tick();
    sendData();
  }
  nextion.listen();
}
