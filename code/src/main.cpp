#include <Arduino.h>


#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>

#include <ESP8266mDNS.h>

// #include <MotionSensor.h>
#include <LEDs.h>
// #include "myTypes.h"
#include <Persistor.h>

#include "colorpicker_html.h"
#include "index_html.h"



/************************************************************
 * *                                                      * *
 * *        Start OF MOTION SENSOR CLASS HEADER           * *
 * *                                                      * *
 * *********************************************************/

/* This should live in its own file, but I do not get it to work then 🤷 */

#include <Wire.h>
#include "MPU6050_6Axis_MotionApps20.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
class MotionSensor
{
public:
    MotionSensor();
    void init();
    void updateGyroData();
    void updateAccData();
    void meanaccelgyro();
    void calibaccelgyro();

    bool zAccExceededLimit(long limit);
    bool xyAccExceededLimit(long limit);

    bool isAccLimitReached();
    bool isUpsideDown();

    void setAZLimit(int);
    int getAZLimit();

    void setShockLimit(long);

    int getAX();
    int getAY();
    int getAZ();
    int getGX();
    int getGY();
    int getGZ();

private:
    bool USESERIAL = false;
    MPU6050 mpu;
    // MPU control/status vars
    /*unntig?*/ bool dmpReady = false;  // set true if DMP init was successful
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer

    // orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    VectorInt16 aa;         // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
    VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity;    // [x, y, z]            gravity vector
    float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
    float yprd[3];           //ypr in degree
    float xyz[3];
    float yaw;
    float initYaw = 0;  //Initial Yaw after sense call
    int16_t ax, ay, az, gx, gy, gz;
    long ax_buff, ay_buff, az_buff, gx_buff, gy_buff, gz_buff;

    long shockLimit = 21000;
    bool issensed = false;

    int azLimit = -14000;
};

/************************************************************
 * *                                                      * *
 * *        END OF MOTION SENSOR CLASS HEADERS            * *
 * *                                                      * *
 * *********************************************************/

MotionSensor motion;
LEDs leds;
Persistor persistor;

AsyncWebServer asyncServer(80);

//I2C (MPU6050)
#define SCL 5
#define SDA 2

bool USESERIAL = true;
bool useSerial = USESERIAL;

bool updateCheckStartet = false;
unsigned long updateStartTime;
unsigned long durationToStartWiFi = 7000;

bool isUpdateMode = false;
bool wifiOff = true;

void startUpdateMode() {
  leds.setColor(0,0,255);
  leds.on();
  asyncServer.end();
  WiFiServer server(5005);
  server.begin();

  isUpdateMode = true;
  delay(10);

  ArduinoOTA.onStart([]() {
    if (useSerial)
    {
      Serial.println("Start");
    }
  });
  ArduinoOTA.onEnd([]() {
    WiFi.mode(WIFI_OFF); //turn off wifi
    if (useSerial)
    {
      Serial.println("\nEnd");
    }
    ESP.restart();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    if (useSerial)
    {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
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

const char *PARAM_R = "r";
const char *PARAM_G = "g";
const char *PARAM_B = "b";
const char *PARAM_ID = "id";
const char *PARAM_SSID = "ssid";
const char *PARAM_PASSWORD = "password";
const char *PARAM_SHOCK_LIMIT = "limit";
const char *PARAM_BLINK_DURATION = "duration";
const char *PARAM_COLOR = "color";
const char *PARAM_MODE = "mode";

void createAPI() {
  asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  asyncServer.on("/colors", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", colors_html);
  });

  asyncServer.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    leds.on();
    request->send_P(200, "text/html", "its on");
  });

  asyncServer.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    leds.off();
    request->send_P(200, "text/html", "its off");
  });

  asyncServer.on("/rgball", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_R) && request->hasParam(PARAM_G) && request->hasParam(PARAM_B))
    {
      int r = request->getParam(PARAM_R)->value().toInt();
      int g = request->getParam(PARAM_G)->value().toInt();
      int b = request->getParam(PARAM_B)->value().toInt();
      if (leds.isRGBInvalid(r, g, b)) {
        request->send(400, "text/plain", "invalid value");
      } else {
        leds.setColor(r,g,b);
        leds.on();
        request->send_P(200, "text/html", "color changed");
      }
    } else {
      request->send(400, "text/plain", "missing parameters");
    }
  });

  asyncServer.on("/rgbsingle", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_ID) && request->hasParam(PARAM_R) && request->hasParam(PARAM_G) && request->hasParam(PARAM_B))
    {
      int r = request->getParam(PARAM_R)->value().toInt();
      int g = request->getParam(PARAM_G)->value().toInt();
      int b = request->getParam(PARAM_B)->value().toInt();
      int id = request->getParam(PARAM_ID)->value().toInt();
      if (leds.isRGBInvalid(r, g, b) || !leds.isIdValid(id)) {
        request->send(400, "text/plain", "invalid value");
      } else {
        leds.changeSingleColor(id, r, g, b);
        request->send_P(200, "text/html", "color changed");
      }
    } else {
      request->send(400, "text/plain", "missing parameters");
    }
  });

  asyncServer.on("/startupdate", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", "starting update mode...on port 5005...I am an ESP8266..");
    startUpdateMode();
  });

  asyncServer.on("/setcredentials", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_SSID) && request->hasParam(PARAM_PASSWORD))
    {
      String newSSID = request->getParam(PARAM_SSID)->value();
      String newPassword = request->getParam(PARAM_PASSWORD)->value();
      if (newSSID.length() > 32 || newPassword.length() > 32) {
        request->send(400, "text/plain", "max length of SSID and Password is 32 characters");
      } else if (newSSID.length() < 1 || newPassword.length() < 1) {
        request->send(400, "text/plain", "min length of SSID and Password is 1 character");
      } else {
        persistor.setCredentials(newSSID, newPassword);
        request->send_P(200, "text/html", "New credentials set. They are effective after next reboot. Don't forget to turn on Wifi again..");
      }
    } else {
      request->send(400, "text/plain", "missing parameters");
    }
  });

  asyncServer.on("/changeshocklimit", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_SHOCK_LIMIT))
    {
      long newLimit = request->getParam(PARAM_SHOCK_LIMIT)->value().toInt();
      if (newLimit > 32000 || newLimit < 0) {
        request->send(400, "text/plain", "invalid limit value");
      } else {
        persistor.config.shockLimit = newLimit;
        request->send_P(200, "text/html", "limit changed");
      }
    } else {
      request->send(400, "text/plain", "missing parameters");
    }
  });

  asyncServer.on("/changeblinkduration", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_BLINK_DURATION))
    {
      long newDuration = request->getParam(PARAM_BLINK_DURATION)->value().toInt();
      if (newDuration <= 0) {
        request->send(400, "text/plain", "invalid duration");
      } else {
        persistor.config.blinkDuration = newDuration;
        request->send_P(200, "text/html", "duration changed");
      }
    } else {
      request->send(400, "text/plain", "missing parameters");
    }
  });

  asyncServer.on("/persist", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(203, "text/plain", "your config now lives through a restart");
      persistor.saveConfig();
  });

  asyncServer.on("/singlecolor", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_ID) && request->hasParam(PARAM_COLOR))
    {
      int id = request->getParam(PARAM_ID)->value().toInt();
      uint32_t color = request->getParam(PARAM_COLOR)->value().toInt();
      Serial.print("color: ");
      Serial.println(color);
      if (!leds.isIdValid(id)) {
        request->send(400, "text/plain", "invalid id");
      } else {
        leds.changeSingleColor(id, color);
        request->send_P(200, "text/html", "color changed for single LED");
      }
    } else {
      request->send(400, "text/plain", "missing parameters");
    }
  });

  asyncServer.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam(PARAM_MODE))
    {
      int mode = request->getParam(PARAM_MODE)->value().toInt();
      leds.setMode(mode);
      request->send_P(200, "text/html", "mode test running");
    } else {
      request->send(400, "text/plain", "missing parameters");
    }
  });

  asyncServer.onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });
}

void startWiFi() {
  wifiOff = false;
  // leds.setColor(255,0,0);
  // leds.on();
  persistor.loadCredentials();
  WiFi.mode(WIFI_STA);
  WiFi.hostname("hackmelamp");
  WiFi.begin(persistor.credentials.ssid, persistor.credentials.password);
  unsigned long wifiStartupTime = millis();
  unsigned long wifiConnectWaitTime = 10000;
  bool startAp = false;
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() > (wifiStartupTime + wifiConnectWaitTime)) {
      if (USESERIAL) {
        Serial.println("Cannot connect, opening AP");
      }
      startAp = true;
      WiFi.disconnect(true);
      break;
    }
    delay(1000);
  }
  if(!startAp && USESERIAL) {
    Serial.print("hostname:");
    Serial.println(WiFi.hostname());
  }

  if(startAp) {
    // IPAddress Ip(192, 168, 4, 111);
    // IPAddress NMask(255, 255, 255, 0);
    WiFi.mode(WIFI_AP);
    // WiFi.softAPConfig(Ip, Ip, NMask);
    WiFi.softAP("LithoLamp", "LithoLamp");

    delay(500);
  }
  createAPI();
  asyncServer.begin();
  // leds.setColor(0,255,0),
  // leds.on();
}

void checkWiFiStartRoutine() {
  if (wifiOff) {
    // if (motion.isUpsideDown() && !updateCheckStartet) {
    //   updateStartTime = millis();
    //   updateCheckStartet = true;
    // } else if (motion.isUpsideDown() && updateCheckStartet) {
    //   if (millis() > updateStartTime + durationToStartWiFi) {
    //     startWiFi();
    //   }
    // } else if (updateCheckStartet && !motion.isUpsideDown()) {
    //   updateCheckStartet = false;
    // }
  }
}

void setup() {
  Serial.begin(115200);

  // start I2C for MPU6050
  Wire.begin(SDA, SCL);
  Wire.setClock(400000);
  
  WiFi.mode(WIFI_OFF);

  persistor.loadConfig();


  leds.init();
  leds.service();

  motion.init();
  motion.calibaccelgyro();

  startWiFi();
}

void loop() {
  if ( isUpdateMode ) {
    ArduinoOTA.handle();
  } else {
    // checkWiFiStartRoutine();
    if (motion.zAccExceededLimit(persistor.config.shockLimit)) {
      // leds.blinkFancy(persistor.config.blinkDuration);
      leds.colorEasterEggBunny();
    } else if (motion.xyAccExceededLimit(persistor.config.shockLimit)) {
      leds.testMode2();
    }
  }
  leds.service();
}






/************************************************************
 * *                                                      * *
 * *        Start OF MOTION SENSOR CLASS IMPLEMENTATION   * *
 * *                                                      * *
 * *********************************************************/

/* This should live in its own file, but I do not get it to work then 🤷 */

MotionSensor::MotionSensor() {}

void MotionSensor::init() {
    // Init Gyro

  // initialize device
  if(USESERIAL){
    Serial.println(F("Initializing I2C devices..."));
  }
  mpu.initialize();
  //pinMode(INTERRUPT_PIN, INPUT);

  // verify connection
  if(USESERIAL){
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  }

  // load and configure the DMP
  if(USESERIAL){
    Serial.println(F("Initializing DMP..."));
  }
  devStatus = mpu.dmpInitialize();
  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    if(USESERIAL){
      Serial.println(F("Enabling DMP..."));
    }
    mpu.setDMPEnabled(true);

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    if(USESERIAL){
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
    }
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    if(USESERIAL){
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
    }
  }
}

void MotionSensor::updateGyroData() {
  mpu.resetFIFO();
  fifoCount = 0;
  // wait for correct available data length, should be a VERY short wait
  while (fifoCount < packetSize) {
    fifoCount = mpu.getFIFOCount();
    //Serial.println("hanging in FIFO count..");
  }

  // read a packet from FIFO
  mpu.getFIFOBytes(fifoBuffer, packetSize);

  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  yprd[0] = ypr[0] * 180 / M_PI + 180;
  yprd[1] = ypr[1] * 180 / M_PI;
  yprd[2] = ypr[2] * 180 / M_PI;
  /*Serial.print("ypr\t");
    Serial.print(yprd[0]);
    Serial.print("\t");
    Serial.print(yprd[1]);
    Serial.print("\t");
    Serial.println(yprd[2]);*/
}

void MotionSensor::updateAccData() {
  mpu.getAcceleration(&ax, &ay, &az);
  if(USESERIAL){
    Serial.print("ax, ay, az:\t");
    Serial.print(ax);
    Serial.print("\t");
    Serial.print(ay);
    Serial.print("\t");
    Serial.println(az);
  }
}

int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz;

void MotionSensor::meanaccelgyro() {
  long i = 0, buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;
  int agmeanbuffer = 50;   //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
  int disg = 30;

  while (i < (agmeanbuffer + disg + 1)) {
    // read raw accel/gyro measurements from device
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if (i > disg && i <= (agmeanbuffer + disg)) { //First disg measures are discarded
      buff_ax = buff_ax + ax;
      buff_ay = buff_ay + ay;
      buff_az = buff_az + az;
      buff_gx = buff_gx + gx;
      buff_gy = buff_gy + gy;
      buff_gz = buff_gz + gz;
    }
    if (i == (agmeanbuffer + disg)) {
      mean_ax = buff_ax / agmeanbuffer;
      mean_ay = buff_ay / agmeanbuffer;
      mean_az = buff_az / agmeanbuffer;
      mean_gx = buff_gx / agmeanbuffer;
      mean_gy = buff_gy / agmeanbuffer;
      mean_gz = buff_gz / agmeanbuffer;
    }
    i++;
    delay(1); //Needed so we don't get repeated measures
  }
}



void MotionSensor::calibaccelgyro() {
  int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;
  int giro_deadzone = 3;   //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)
  int acel_deadzone = 20;   //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);

  meanaccelgyro();

  ax_offset = -mean_ax / 8;
  ay_offset = -mean_ay / 8;
  az_offset = - mean_az / 8;

  gx_offset = -mean_gx / 4;
  gy_offset = -mean_gy / 4;
  gz_offset = -mean_gz / 4;
  int agready = 0;
  int agcount = 0;
  while (agready < 6 && agcount < 11) {
    agready = 0;
    agcount++;
    mpu.setXAccelOffset(ax_offset);
    mpu.setYAccelOffset(ay_offset);
    mpu.setZAccelOffset(az_offset);

    mpu.setXGyroOffset(gx_offset);
    mpu.setYGyroOffset(gy_offset);
    mpu.setZGyroOffset(gz_offset);

    meanaccelgyro();

    if (abs(mean_ax) <= acel_deadzone) agready++;
    else ax_offset = ax_offset - mean_ax / acel_deadzone;

    if (abs(mean_ay) <= acel_deadzone) agready++;
    else ay_offset = ay_offset - mean_ay / acel_deadzone;

    if (abs(mean_az) <= acel_deadzone) agready++;
    else az_offset = az_offset  - mean_az / acel_deadzone;

    if (abs(mean_gx) <= giro_deadzone) agready++;
    else gx_offset = gx_offset - mean_gx / (giro_deadzone + 1);

    if (abs(mean_gy) <= giro_deadzone) agready++;
    else gy_offset = gy_offset - mean_gy / (giro_deadzone + 1);

    if (abs(mean_gz) <= giro_deadzone) agready++;
    else gz_offset = gz_offset - mean_gz / (giro_deadzone + 1);

    if(USESERIAL){
      Serial.print(".");
    }
  }
  if(USESERIAL){
    Serial.print("\nSensor readings with offsets:\t");
    Serial.print(mean_ax);
    Serial.print("\t");
    Serial.print(mean_ay);
    Serial.print("\t");
    Serial.print(mean_az);
    Serial.print("\t");
    Serial.print(mean_gx);
    Serial.print("\t");
    Serial.print(mean_gy);
    Serial.print("\t");
    Serial.println(mean_gz);
    Serial.print("Your offsets:\t");
    Serial.print(ax_offset);
    Serial.print("\t");
    Serial.print(ay_offset);
    Serial.print("\t");
    Serial.print(az_offset);
    Serial.print("\t");
    Serial.print(gx_offset);
    Serial.print("\t");
    Serial.print(gy_offset);
    Serial.print("\t");
    Serial.println(gz_offset);
  }
}

bool MotionSensor::isAccLimitReached() {
    updateAccData();
    return abs(ax) > shockLimit || abs(ay) > shockLimit || abs(az) > shockLimit;
}

bool MotionSensor::isUpsideDown() {
    updateAccData();
    return az < azLimit;
}

int MotionSensor::getAZLimit() {
    return azLimit;
}

void MotionSensor::setAZLimit(int newLimit) {
    azLimit = newLimit;
}

void MotionSensor::setShockLimit(long newLimit) {
  shockLimit = newLimit;
}

int MotionSensor::getAX(){
    // mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    return ax;
};
int MotionSensor::getAY(){
    // mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    return ay;
};
int MotionSensor::getAZ(){
    // mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    return az;
};
int MotionSensor::getGX(){
    // mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    return gx;
};
int MotionSensor::getGY(){
    // mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    return gy;
};
int MotionSensor::getGZ(){
    // mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    return gz;
};

bool MotionSensor::zAccExceededLimit(long limit) {
  updateAccData();
  return abs(az) > limit;
}

bool MotionSensor::xyAccExceededLimit(long limit) {
  updateAccData();
  return abs(ax) > limit || abs(ay) > limit;
}

/************************************************************
 * *                                                      * *
 * *        END OF MOTION SENSOR CLASS IMPLEMENTATION     * *
 * *                                                      * *
 * *********************************************************/