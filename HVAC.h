//Unique name of device
#define CLIENT_ID        "hvac_lr" // Must be unique!

// wifi settings from secret.h 
#define WIFI_SSID        wifi_ssid
#define WIFI_PASSWORD    wifi_password

// mqtt server settings from secret.h
#define MQTT_SERVER       mqtt_server
#define MQTT_PORT         mqtt_port
#define MQTT_USER         mqtt_user
#define MQTT_PASSWORD     mqtt_password

#define MAIN_TOPIC              "home"
#define ROOM                    "hall"
#define DEVICE_TYPE             "climate"
#define HOME_ASSISTANT_TOPIC    "homeassistant"

//Топики                     
#define HA_CONFIG_TOPIC            HOME_ASSISTANT_TOPIC "/" DEVICE_TYPE "/" CLIENT_ID "/config"
#define ONLINE_TOPIC               MAIN_TOPIC "/online"
#define STATE_TOPIC                MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/state"
#define DEBUG_TOPIC                MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/debug"
#define HEATPUMP_ONLINE            MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/online"


// command
#define HA_POWER_SET_TOPIC         MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/power/set"
#define HA_MODE_SET_TOPIC          MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/mode/set"
#define HA_TEMP_SET_TOPIC          MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/temp/set"
#define HA_ROOMTEMP_SET_TOPIC      MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/roomtemp/set"
#define HA_FAN_SET_TOPIC           MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/fan/set"
#define HA_VANE_SET_TOPIC          MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/vane/set"
#define HA_WIDEVANE_SET_TOPIC      MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/wideVane/set"
#define DEBUG_SET_TOPIC            MAIN_TOPIC "/" ROOM "/" CLIENT_ID "/debug/set"



// Timer to send status
const unsigned int SEND_ROOM_TEMP_INTERVAL_MS = 60000;
