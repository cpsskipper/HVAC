
void mqtt_init(){ 
  client.setServer(MQTT_SERVER, MQTT_PORT);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");  
    if (client.connect(CLIENT_ID, MQTT_USER, MQTT_PASSWORD )) {
       Serial.println("connected"); 
    } else { 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000); 
    }
  }
  client.setCallback(mqttCallback);
      client.subscribe(DEBUG_SET_TOPIC);
      client.subscribe(HA_POWER_SET_TOPIC);
      client.subscribe(HA_MODE_SET_TOPIC);
      client.subscribe(HA_FAN_SET_TOPIC);
      client.subscribe(HA_TEMP_SET_TOPIC);
      client.subscribe(HA_ROOMTEMP_SET_TOPIC);
      client.subscribe(HA_VANE_SET_TOPIC);
  client.publish(ONLINE_TOPIC, CLIENT_ID "online");

// Автоматическая регистрация устройства в Home Assistant. Требуется включение MQTT discovery на стороне HA
  haConfig();  
}


void haConfig() {

  // send HA config packet
  // setup HA payload device
  const size_t capacity = 3 * JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(7) + JSON_OBJECT_SIZE(23) + JSON_OBJECT_SIZE(50);
  DynamicJsonDocument haConfig(capacity);

  haConfig["name"]                          = CLIENT_ID;
  haConfig["mode_cmd_t"]                    = HA_MODE_SET_TOPIC;
  haConfig["mode_stat_t"]                   = STATE_TOPIC;
  haConfig["mode_stat_tpl"]                 = "{{ value_json.mode }}";
  haConfig["temp_cmd_t"]                    = HA_TEMP_SET_TOPIC;
  haConfig["temp_stat_t"]                   = STATE_TOPIC;
  haConfig["temp_stat_tpl"]                 = "{{ value_json.temperature }}";
//Температура для объекта HVAC в HA берется от датчика кондиционера
   haConfig["curr_temp_t"]                   = STATE_TOPIC;
   haConfig["current_temperature_template"]  = "{{ value_json.roomTemperature }}";
//Температура для объекта HVAC в HA берется от внешних датчиков
  //haConfig["curr_temp_t"]                  = ha_current_temperature_topic;
  haConfig["min_temp"]                       = "16";
  haConfig["max_temp"]                       = "31";
  haConfig["unique_id"]                      = CLIENT_ID;

  JsonArray haConfigModes = haConfig.createNestedArray("modes");
  haConfigModes.add("auto");
  haConfigModes.add("off");
  haConfigModes.add("cool");
  haConfigModes.add("heat");
  haConfigModes.add("dry");

  JsonArray haConfigFan_modes = haConfig.createNestedArray("fan_modes");
  haConfigFan_modes.add("AUTO");
  haConfigFan_modes.add("1");
  haConfigFan_modes.add("2");
  haConfigFan_modes.add("3");
  haConfigFan_modes.add("4");

  JsonArray haConfigSwing_modes = haConfig.createNestedArray("swing_modes");
  haConfigSwing_modes.add("AUTO");
  haConfigSwing_modes.add("1");
  haConfigSwing_modes.add("2");
  haConfigSwing_modes.add("3");
  haConfigSwing_modes.add("4");
  haConfigSwing_modes.add("5");
  haConfigSwing_modes.add("SWING");
  haConfig["pow_cmd_t"]                     = HA_POWER_SET_TOPIC;
  haConfig["fan_mode_cmd_t"]                = HA_FAN_SET_TOPIC;
  haConfig["fan_mode_stat_t"]               = STATE_TOPIC;
  haConfig["fan_mode_stat_tpl"]             = "{{ value_json.fan }}";
  haConfig["swing_mode_cmd_t"]              = HA_VANE_SET_TOPIC;
  haConfig["swing_mode_stat_t"]             = STATE_TOPIC;
  haConfig["swing_mode_stat_tpl"]           = "{{ value_json.vane }}";

  JsonObject haConfigDevice = haConfig.createNestedObject("device");

  haConfigDevice["ids"]   = CLIENT_ID;
  haConfigDevice["name"]  = CLIENT_ID;
  haConfigDevice["sw"]    = "Mitsu2MQTT .1ME";
  haConfigDevice["mdl"]   = "HVAC MITUBISHI";
  haConfigDevice["mf"]    = "MITSUBISHI";


  String mqttOutput;
  serializeJson(haConfig, mqttOutput);
  client.beginPublish(HA_CONFIG_TOPIC, mqttOutput.length(), true);
  client.print(mqttOutput);
  client.endPublish();
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Copy payload into message buffer
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  //Handle the Home Assistant Topics
  if (strcmp(topic, HA_POWER_SET_TOPIC) == 0) {
    hp.setPowerSetting(message);
    hp.update();
    lastTempSend = millis();
  } else if (strcmp(topic, HA_MODE_SET_TOPIC) == 0) {
    const size_t bufferSize = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<bufferSize> root;
    root["mode"] = message;
    String mqttOutput;
    serializeJson(root, mqttOutput);
    client.publish(STATE_TOPIC, mqttOutput.c_str());
    String modeUpper = message;
    modeUpper.toUpperCase();
    hp.setModeSetting(modeUpper.c_str());
    hp.update();
    lastTempSend = millis();
    
  } else if (strcmp(topic, HA_TEMP_SET_TOPIC) == 0) {
    float temperature = strtof(message, NULL);
    const size_t bufferSize = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<bufferSize> root;
    root["temperature"] = message;
    String mqttOutput;
    serializeJson(root, mqttOutput);
    client.publish(STATE_TOPIC, mqttOutput.c_str());
    hp.setTemperature(temperature);
    hp.update();
    lastTempSend = millis();
  } 
   else if (strcmp(topic, HA_ROOMTEMP_SET_TOPIC) == 0) {
    float temperature = strtof(message, NULL);
    const size_t bufferSize = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<bufferSize> root;
    root["roomTemperature"] = message;
    String mqttOutput;
    serializeJson(root, mqttOutput);
    client.publish(STATE_TOPIC, mqttOutput.c_str());
    hp.setRemoteTemperature(temperature);
    hp.update();
    lastTempSend = millis();
  } 
  else if (strcmp(topic, HA_FAN_SET_TOPIC) == 0) {
    const size_t bufferSize = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<bufferSize> root;
    root["fan"] = message;
    String mqttOutput;
    serializeJson(root, mqttOutput);
    client.publish(STATE_TOPIC, mqttOutput.c_str());
    hp.setFanSpeed(message);
    hp.update();
    lastTempSend = millis();
  }
  else if (strcmp(topic, HA_VANE_SET_TOPIC) == 0) {
    const size_t bufferSize = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<bufferSize> root;
    root["vane"] = message;
    String mqttOutput;
    serializeJson(root, mqttOutput);
    client.publish(STATE_TOPIC, mqttOutput.c_str());
    hp.setVaneSetting(message);
    hp.update();
    lastTempSend = millis();
  }
  else if (strcmp(topic, DEBUG_SET_TOPIC) == 0) { //if the incoming message is on the heatpump_debug_set_topic topic...
    if (strcmp(message, "on") == 0) {
      _debugMode = true;
      client.publish(DEBUG_TOPIC, "debug mode enabled");
    } else if (strcmp(message, "off") == 0) {
      _debugMode = false;
      client.publish(DEBUG_TOPIC, "debug mode disabled");
    }
  } 

  else {
    client.publish(DEBUG_TOPIC, strcat("heatpump: wrong mqtt topic: ", topic));
  }

}
