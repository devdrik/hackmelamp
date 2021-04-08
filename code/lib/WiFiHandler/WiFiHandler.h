#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

class WiFiHandler
{
public:
    WiFiHandler(const char *ssid, const char *password, byte lastIpByte, bool useSerial = false);
    void init();
    void startAP();
    void startUpdateMode();
    void handleOTA();

    bool isUpdateMode = false;

private:
    bool useSerial;
    const char *ssid;
    const char *password;
    byte lastIpByte;
};