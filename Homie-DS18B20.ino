#include <Homie.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 14

HomieNode temperatureNode("temperature", "temperaure");
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

const int sleepTimeS = 10;
bool sentOnce = false;

unsigned long previousMillis = 0;
const long interval = 5000; 

void loopHandler() {
  unsigned long currentMillis = millis();
  //if (!sentOnce) {
  if (currentMillis - previousMillis >= interval) {
     previousMillis = currentMillis;
     float temperature;
     DS18B20.requestTemperatures(); 
     temperature = DS18B20.getTempCByIndex(0);
     Serial.print("Temperature: ");
     Serial.println(temperature);
     if (Homie.isConnected()) {
       sentOnce = true;
       temperatureNode.setProperty("Fahrenheit").send(String(temperature)); 
     }
  }
}
void onHomieEvent(HomieEvent event) {
  switch(event.type) {
    case HomieEventType::MQTT_CONNECTED:
      sentOnce = false;
      //Homie.prepareToSleep();
      break;
    case HomieEventType::READY_TO_SLEEP:
     // ESP.deepSleep(sleepTimeS * 1000000, RF_NO_CAL);
      break;
  }
}
void setup() {
  Serial.begin(115200);
  DS18B20.begin();
  Homie_setFirmware("Sonoff-DS18B20", "0.0.1");
  Homie.disableLedFeedback();
  Homie.setLoopFunction(loopHandler);
  Homie.onEvent(onHomieEvent);
  temperatureNode.advertise("temperature");
  Homie.setup();
}

void loop() {
  Homie.loop();
}
