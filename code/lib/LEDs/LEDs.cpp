#include "LEDs.h"

LEDs::LEDs(){};

void LEDs::init() {
  ws2812fx.init();
  ws2812fx.setBrightness(255);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(color);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
}

void LEDs::service() {  
  if ( isEventActive ) {
    ws2812fx.service();
  }
  if (isEventActive && millis() > eventStartTime + eventDuration) {
    endEvent();
  }
}

void LEDs::on() {
  ws2812fx.setColor(color);
  ws2812fx.show();
  isLightOn = true;
}

void LEDs::off() {
  ws2812fx.clear();
  isLightOn = false;
  if(USESERIAL) {
    Serial.println("LEDs turned off ");
  }
}

void LEDs::setColor(uint8_t r, uint8_t g, uint8_t b) {
  color = ws2812fx.Color(r,g,b);
  ws2812fx.setColor(color);
}

void LEDs::changeSingleColor(uint8_t id, uint8_t r, uint8_t g, uint8_t b) {
  ws2812fx.setPixelColor(id, ws2812fx.Color(r, g, b));
  ws2812fx.show();
  if(USESERIAL) {
    Serial.print("changed color of LED ");
    Serial.print(id);
    Serial.print(" to r: ");
    Serial.print(r);
    Serial.print(" g: ");
    Serial.print(g);
    Serial.print(" b: ");
    Serial.println(b);
  }
}

void LEDs::changeSingleColor(uint8_t id, uint32_t color) {
  ws2812fx.setPixelColor(id, color);
  ws2812fx.show();
}

bool LEDs::isRGBInvalid(uint8_t r, uint8_t g, uint8_t b) {
  return r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255;
}

bool LEDs::isIdValid(uint8_t id) {
  return id >= 0 && id < 16;
}

void LEDs::startTimedMode(long duration) {
  if(USESERIAL) {
    Serial.print("Event started, duation: ");
    Serial.println(duration);
  }
  if ( !isEventActive ) {
    // only save state if no event is active
    lastMode = ws2812fx.getMode();
    lastBrightness = ws2812fx.getBrightness();
    lastColor = ws2812fx.getColor();
  }
    eventDuration = duration;
    eventStartTime = millis();
    isEventActive = true;
}

void LEDs::setMode(uint8_t mode) {
  ws2812fx.setMode(mode);
  startTimedMode(3000);
}

void LEDs::blinkFancy() {
  // ws2812fx.setMode(FX_MODE_RAINBOW);
  // ws2812fx.setMode((ws2812fx.getMode() + 1) % ws2812fx.getModeCount());
  // Serial.print("mode: ");
  // Serial.println(ws2812fx.getMode());
  // ws2812fx.setMode(FX_MODE_TWINKLE_RANDOM);
  ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
}

void LEDs::blinkFancy(long duration) {
  startTimedMode(duration);
  blinkFancy();
}

void LEDs::endEvent() {
  if(USESERIAL) {
    Serial.println("Event finished");
  }
  isEventActive = false;
  ws2812fx.setColor(lastColor);
  ws2812fx.setBrightness(lastBrightness);
  ws2812fx.setMode(lastMode);
}

void LEDs::flash(uint8_t r, uint8_t g, uint8_t b, long duration) {
  startTimedMode(duration);
  ws2812fx.setColor(ws2812fx.Color(r,g,b));
}

void LEDs::testMode() {
  startTimedMode(0);

  ws2812fx.setColor(ws2812fx.Color(255,255,255));
  
  unsigned long startTime = millis();
  unsigned long duration = 5000;
  ws2812fx.setMode(FX_MODE_RAINBOW);
  while (millis() < startTime + duration) {
    ws2812fx.service();
    delay(1);
  }

  // startTime = millis();
  // duration = 5000;
  // ws2812fx.setSpeed(500);
  // ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
  // while (millis() < startTime + duration) {
  //   ws2812fx.service();
  //   delay(1);
  // }

  startTime = millis();
  duration = 5000;
  ws2812fx.setSpeed(500);
  ws2812fx.setMode(FX_MODE_COLOR_SWEEP_RANDOM);
  while (millis() < startTime + duration) {
    ws2812fx.service();
    delay(1);
  }

  startTime = millis();
  duration = 5000;
  ws2812fx.setSpeed(100);
  ws2812fx.setMode(FX_MODE_FIREWORKS_RANDOM);
  while (millis() < startTime + duration) {
    ws2812fx.service();
    delay(1);
  }

  endEvent();
}

void LEDs::testMode2() {
  startTimedMode(0);

  ws2812fx.setColor(ws2812fx.Color(255,255,255));
  
  unsigned long startTime = millis();
  unsigned long duration = 5000;

  ws2812fx.setSpeed(100);
  ws2812fx.setMode(FX_MODE_MULTI_DYNAMIC);
  // ws2812fx.setMode(FX_MODE_SINGLE_DYNAMIC);
  while (millis() < startTime + duration) {
    ws2812fx.service();
    delay(1);
  }

  endEvent();
  colorEasterEggBunny();
}

void LEDs::colorEasterEggBunny() {
  ws2812fx.setPixelColor(0,255,0,0);
  ws2812fx.setPixelColor(1,133,77,0);
  ws2812fx.setPixelColor(2,133,77,0);
  ws2812fx.setPixelColor(3,0,255,0);
  ws2812fx.setPixelColor(4,0,130,0);
  ws2812fx.setPixelColor(5,168,50,0);
  ws2812fx.setPixelColor(6,168,50,0);
  ws2812fx.setPixelColor(7,0,200,200);
  ws2812fx.setPixelColor(8,0,99,0);
  ws2812fx.setPixelColor(9,255,0,0);
  ws2812fx.setPixelColor(10,100,0,0);
  ws2812fx.setPixelColor(11,0,0,90);
  ws2812fx.setPixelColor(12,0,99,0);
  ws2812fx.setPixelColor(13,255,0,0);
  ws2812fx.setPixelColor(14,120,0,0);
  ws2812fx.setPixelColor(15,0,0,0);
  ws2812fx.show();
}