// Don't like these here as a lib, would prefere it in inclue, but this worked 🤷

#pragma once
#include <Arduino.h>
#include <WString.h>

typedef struct {
    // We don't use a bool, because the bit might be accidentally set
    char isStored[3];
    long shockLimit;
    unsigned long blinkDuration;
    uint32_t color;
} Config;

typedef struct {
    char isStored[3];
    char ssid[32];
    char password[32];
} Credentials;