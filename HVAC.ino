#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <HeatPump.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include "secret.h"
#include "HVAC.h"



//----------------------------------------------------------MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//----------------------------------------------------------HeatPump
HeatPump hp;
unsigned long lastTempSend;

//---------------------------------------------------------DEBUG
// debug mode, when true, will send all packets received from the heatpump to topic heatpump_debug_topic
// this can also be set by sending "on" to heatpump_debug_set_topic
bool _debugMode = false;

//------------------------------------------------------------------------------------------SETUP

void setup() {
  wifi_init();
  ota_init();
  mqtt_init();
  heatpump_init();
}




//------------------------------------------------------------------------------------------LOOP

void loop() {

  
//----------------------------------- Проверка WiFi
  if (WiFi.status() != WL_CONNECTED){
   wifi_init();
  }
  
//----------------------------------- Обработка прошивки по воздуху
  ArduinoOTA.handle();
  
//----------------------------------- Проверка MQTT
  if (!client.connected()) {
    Serial.println("Attempting to reconnect MQTT...");
    mqtt_init();
  }

//----------------------------------- считывание очереди MQTT
client.loop();
//------------------------------------считывание очереди с HVAC
  hp.sync();

//------------------------------------Отправка состояния в HA по таймеру
  if (millis() > (lastTempSend + SEND_ROOM_TEMP_INTERVAL_MS) || (lastTempSend > millis())) { // only send the temperature every 60s
    hpStatusChanged(hp.getStatus());
    client.publish(HEATPUMP_ONLINE, "ON");   
    lastTempSend = millis();
  }

}
