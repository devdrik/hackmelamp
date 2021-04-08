#include <WS2812FX.h>

#define LED_COUNT 16
#define LED_PIN 0

/*
To use this class you need to call service in the main loop!
*/

class LEDs
{
public:
    LEDs();
    void init();
    void service();
    void on();
    void off();
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void flash(uint8_t r, uint8_t g, uint8_t b, long duration);
    void changeSingleColor(uint8_t id, uint8_t r, uint8_t g, uint8_t b);
    void changeSingleColor(uint8_t id, uint32_t color);
    bool isRGBInvalid(uint8_t r, uint8_t g, uint8_t b);
    bool isIdValid(uint8_t id);
    void setMode(uint8_t mode);

    void blinkFancy();
    void blinkFancy(long duration);

    void testMode(); 
    void testMode2(); 
    void colorEasterEggBunny(); 

    
private:
    bool USESERIAL = true;
    WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
    uint32_t color = ws2812fx.Color(255,255,255);
    bool isLightOn = false;

    unsigned long eventStartTime = 0;
    unsigned long eventDuration = 0;
    bool isEventActive = false;

    uint8_t lastMode;
    uint8_t lastBrightness;
    uint32_t lastColor;

    void startTimedMode(long duration);
    void endEvent();

};
