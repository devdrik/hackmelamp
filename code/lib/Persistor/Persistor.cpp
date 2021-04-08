#include "Persistor.h"

Persistor::Persistor(){}

void Persistor::saveConfig() {
  // Save configuration from RAM into EEPROM
  String isStored = "OK";
  isStored.toCharArray(config.isStored, 3);
  EEPROM.begin(eepromSize);
  EEPROM.put( configStart, config );
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
  if (USESERIAL) {
    Serial.println("Saved config:");
    Serial.print("shockLimit: ");
    Serial.println(config.shockLimit);
    Serial.print("blinkDuration: ");
    Serial.println(config.blinkDuration);
    Serial.print("color: ");
    Serial.println(config.color);
    Serial.print("isStored: ");
    Serial.println(config.isStored);
  }
}

void Persistor::loadConfig() {
  // Serial.print("ConfigStart: ");
  // Serial.println(configStart);
  EEPROM.begin(eepromSize);
  EEPROM.get( configStart, config );
  EEPROM.end();
  if (!isStored(config.isStored)) {
    // Serial.println("No config available, using initial values");
    // TODO: inject config?
    config.blinkDuration = 5000;
    config.shockLimit = 5000;
    config.color = 351746;
  }
  if(USESERIAL) {
    Serial.println("Loaded config:");
    Serial.print("shockLimit: ");
    Serial.println(config.shockLimit);
    Serial.print("blinkDuration: ");
    Serial.println(config.blinkDuration);
    Serial.print("color: ");
    Serial.println(config.color);
    Serial.print("isStored: ");
    Serial.println(config.isStored);
  }
}

void Persistor::eraseConfig() {
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(eepromSize);
  for (int i = 0 ; i < eepromSize ; i++) {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}

/** Load WLAN credentials from EEPROM */
void Persistor::loadCredentials() {
  EEPROM.begin(eepromSize);
  EEPROM.get(0, credentials);
  EEPROM.end();
  if (!isStored(credentials.isStored)) {
    credentials.ssid[0] = 0;
    credentials.password[0] = 0;
  }
  if (USESERIAL) {
    Serial.println("Recovered credentials:");
    Serial.println(credentials.ssid);
    Serial.println(strlen(credentials.password)>0?"********":"<no password>");
  }
}

/** Store WLAN credentials to EEPROM */
void Persistor::saveCredentials() {
  String isStored = "OK";
  isStored.toCharArray(credentials.isStored, 3);
  EEPROM.begin(eepromSize);
  EEPROM.put(0, credentials);
  EEPROM.commit();
  EEPROM.end();
}

void Persistor::setCredentials(String ssid, String password) {
    ssid.toCharArray(credentials.ssid, 32);
    password.toCharArray(credentials.password, 32);
    saveCredentials();
}

bool Persistor::isStored(char isStored[3]) {
  return isStored[0] == 'O' && isStored[1] == 'K';
}