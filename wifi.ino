
void wifi_init(){
  WiFi.disconnect();
  WiFi.hostname(CLIENT_ID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
