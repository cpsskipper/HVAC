void heatpump_init(){
    //Получение обновления с пульта кондиционера
  hp.enableAutoUpdate();
  hp.enableExternalUpdate();
  // connect to the heatpump. Callbacks first so that the hpPacketDebug callback is available for connect()
  hp.setSettingsChangedCallback(hpSettingsChanged);
  hp.setStatusChangedCallback(hpStatusChanged);
  hp.setPacketCallback(hpPacketDebug);
// Подключение к HVAC
  boolean s= false;
  s = hp.connect(&Serial);
  if (!s){
    client.publish(DEBUG_TOPIC, "Connect to hvac failed");
  }
  else {
    client.publish(DEBUG_TOPIC, "Connect to hvac OK");
  }
  lastTempSend = millis();
}


//Вызывается для получения статуса кондиционера для отправки в HA
void hpStatusChanged(heatpumpStatus currentStatus) {
  // send room temp, operating info and all information
  heatpumpSettings currentSettings = hp.getSettings();
  
  const size_t bufferSizeInfo = JSON_OBJECT_SIZE(5);
  DynamicJsonDocument rootInfo(bufferSizeInfo);

  rootInfo["roomTemperature"] = hp.getRoomTemperature();
  rootInfo["temperature"]     = currentSettings.temperature;
  rootInfo["fan"]             = currentSettings.fan;
  rootInfo["vane"]            = currentSettings.vane;
  String hppower = String(currentSettings.power);
  String hpmode = String(currentSettings.mode);

  hppower.toLowerCase();
  hpmode.toLowerCase();  
 
  if (hpmode == "fan") {
    rootInfo["mode"] = "fan_only";
  }
  else if (hppower == "off") {
    rootInfo["mode"] = hppower.c_str();
  }
  else {
    rootInfo["mode"] = hpmode.c_str();
  }
//  rootInfo["mode"]            = getPowerAndModeToString(currentSettings);

  char bufferInfo[512];
  serializeJson(rootInfo, bufferInfo);

  if (!client.publish(STATE_TOPIC, bufferInfo, true)) {
    client.publish(DEBUG_TOPIC, "failed to publish to room temp and operation status to ha_state_topic topic");
  }
}


void hpSettingsChanged() {
  // send room temp, operating info and all information
  heatpumpSettings currentSettings = hp.getSettings();
  
  const size_t bufferSizeInfo = JSON_OBJECT_SIZE(5);
  DynamicJsonDocument rootInfo(bufferSizeInfo);

  rootInfo["roomTemperature"] = hp.getRoomTemperature();
  rootInfo["temperature"]     = currentSettings.temperature;
  rootInfo["fan"]             = currentSettings.fan;
  rootInfo["vane"]            = currentSettings.vane;
  //rootInfo["mode"]            = getPowerAndModeToString(currentSettings);

  String hppower = String(currentSettings.power);
  String hpmode = String(currentSettings.mode);

  hppower.toLowerCase();
  hpmode.toLowerCase();  
 
  if (hpmode == "fan") {
     rootInfo["mode"] = "fan_only";
  }
  else if (hppower == "off") {
    rootInfo["mode"] = hppower.c_str();
  }
  else {
    rootInfo["mode"] = hpmode.c_str();
  }
  char bufferInfo[512];
  serializeJson(rootInfo, bufferInfo);

  if (!client.publish(STATE_TOPIC, bufferInfo, true)) {
    client.publish(DEBUG_TOPIC, "failed to publish to room temp and operation status to heatpump/status topic");
  }
}

const char* getPowerAndModeToString(heatpumpSettings currentSettings) {
  String hppower = String(currentSettings.power);
  String hpmode = String(currentSettings.mode);

  hppower.toLowerCase();
  hpmode.toLowerCase();

  if (hpmode == "fan") {
    return "fan_only";
  }
  else if (hppower == "off") {
    return "off";
  }
  else {
    return hpmode.c_str();
  }
}


void hpPacketDebug(byte* packet, unsigned int length, char* packetDirection) {
  if (_debugMode) {
    String message;
    for (int idx = 0; idx < length; idx++) {
      if (packet[idx] < 16) {
        message += "0"; // pad single hex digits with a 0
      }
      message += String(packet[idx], HEX) + " ";
    }

    const size_t bufferSize = JSON_OBJECT_SIZE(6);
    DynamicJsonDocument root(bufferSize);

    root[packetDirection] = message;

    char buffer[512];
    serializeJson(root, buffer);

    if (!client.publish(DEBUG_TOPIC, buffer)) {
      client.publish(DEBUG_TOPIC, "failed to publish to heatpump/debug topic");
    }
  }
}
