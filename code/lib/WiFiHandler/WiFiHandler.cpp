#include "WiFiHandler.h"

WiFiHandler::WiFiHandler(const char *ssid, const char *password, byte lastIpByte, bool useSerial)
    : ssid{ssid}, password{password}, lastIpByte{lastIpByte}, useSerial{useSerial} {};

void WiFiHandler::init()
{
  WiFi.mode(WIFI_OFF); //turn off wifi
}

void WiFiHandler::startAP()
{
  IPAddress Ip(192, 168, 4, lastIpByte);
  IPAddress NMask(255, 255, 255, 0);

  WiFiServer server(5005);

  // Init wifi AP and Websocket
  if (useSerial)
  {
    Serial.println();
    Serial.print("Configuring access point...");
  }
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(Ip, Ip, NMask);
  delay(500);

  WiFi.softAP(ssid, password);

  delay(500);
  IPAddress myIP = WiFi.softAPIP();
  if (useSerial)
  {
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    Serial.println("\nStarting server...");
  }
  // start the server:
  server.begin();
}

void WiFiHandler::startUpdateMode()
{
  isUpdateMode = true;
  delay(10);

  ArduinoOTA.onStart([this]() {
    if (useSerial)
    {
      Serial.println("Start");
    }
  });
  ArduinoOTA.onEnd([this]() {
    WiFi.mode(WIFI_OFF); //turn off wifi
    if (useSerial)
    {
      Serial.println("\nEnd");
    }
    ESP.restart();
  });
  ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
    if (useSerial)
    {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    }
  });
  ArduinoOTA.onError([this](ota_error_t error) {
    if (useSerial)
    {
      Serial.printf("Error[%u]: ", error);
    }
    if (error == OTA_AUTH_ERROR && useSerial)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR && useSerial)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR && useSerial)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR && useSerial)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR && useSerial)
      Serial.println("End Failed");
    WiFi.mode(WIFI_OFF); //turn off wifi
    ESP.restart();
  });
  ArduinoOTA.begin();
}

void WiFiHandler::handleOTA()
{
  ArduinoOTA.handle();
}
