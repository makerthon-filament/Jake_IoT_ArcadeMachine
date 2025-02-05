#include "thingProperties.h" // Arduino IoT Cloud 라이브러리
#include <Arduino.h>
#include <NeoPixelConnect.h>  // NeoPixel LED 라이브러리
#include <DHT.h>              // DHT 온습도 센서 라이브러리
#include <Adafruit_GFX.h>     // Adafruit 그래픽 라이브러리 (OLED 디스플레이 지원)
#include <Adafruit_SSD1306.h> // Adafruit OLED 디스플레이 라이브러리
#include <BH1750.h>           // BH1750 조도 센서 라이브러리
#include <WiFiNINA.h>
#include <wire.h>

#define DEBUG_MODE_ENABLE 0

// NeoPixel LED 설정
#define PIXEL_CONTROL_PIN 15
#define MAXIMUM_NUM_NEOPIXELS 12

// DHT 온습도 센서 설정
#define DHT_PIN 2
#define DHTTYPE DHT11

// OLED 디스플레이 설정
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// 터치 센서 설정
#define ON 1
#define OFF 0
#define TOUCH_SENSOR_PIN 4
#define KEY_IN_TIME_SHORT_CHECK_VALUE 20
#define KEY_IN_TIME_LONG_CHECK_VALUE 40
bool flgButtonPressedChk = false;
bool flgButtonPressedStatus = false;
bool flgButtonReleasedStatus = false;
bool flgExternalInterruptDetected = false;
bool flgOnOffCtrlStatus = false;
bool flgModeChangeCtrl = false;
unsigned char cntKeyInTimingCtrl = 0;
unsigned char seqSystemModeControl = 0;
unsigned long btnChkTimeInterval = 300;
unsigned long btnChkTimePreviousMillis = 0;

// 초기 화면 표시 시간 (3초 동안 초기 화면 표시 후 센서 데이터 표시)
const long displayDelay = 3000;

// 전역 변수 선언
unsigned char redColor = 0, greenColor = 0, blueColor = 0;
bool flgEmotionalModeEnable = true;
bool flgInitialDisplayDone = false;
unsigned long previousMillis = 0;
double hue, saturation, value;
void rgb_to_hsv(double r, double g, double b);

// Arduino IoT Cloud 관련 변수 선언
unsigned long lastCloudUpdate = 0;
const unsigned long cloudUpdateInterval = 100; // 클라우드 업데이트 주기 (100ms)

// OLED 초기 화면 비트맵 데이터 (로고나 초기 화면 이미지)
static const unsigned char epd_bitmap_oled_background[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x80, 0x00, 0x01, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0xfd, 0x83, 0xfc, 0x80, 0x18, 0xf9, 0xb3, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0xf9, 0x80, 0x00, 0xbf, 0x98, 0xf9, 0xb3, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x81, 0x81, 0x98, 0x9f, 0x98, 0x8d, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0xfd, 0x80, 0x90, 0x80, 0x98, 0x8f, 0xb3, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x81, 0x80, 0xf0, 0x9f, 0x9e, 0x8d, 0xb3, 0xfe, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0xfd, 0x83, 0xfc, 0x9f, 0x9e, 0xf9, 0xb3, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3c, 0x00, 0x01, 0xfd, 0x80, 0x00, 0x98, 0x18, 0xf9, 0xb3, 0xfe, 0x00, 0x00, 0x3c, 0x00,
    0x00, 0xc3, 0x00, 0x00, 0x00, 0x00, 0xff, 0x98, 0x18, 0x01, 0xb3, 0xfe, 0x00, 0x00, 0xc3, 0x00,
    0x01, 0x00, 0x80, 0x00, 0xff, 0x80, 0x01, 0x9f, 0x98, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x80,
    0x02, 0x3c, 0x40, 0x00, 0xff, 0x80, 0xff, 0x9f, 0xd8, 0x30, 0x07, 0xff, 0x00, 0x02, 0x3c, 0x40,
    0x02, 0x3c, 0x40, 0x00, 0x80, 0x80, 0xff, 0x80, 0x18, 0x38, 0x07, 0xff, 0x80, 0x02, 0x3c, 0x40,
    0x04, 0x32, 0x20, 0x00, 0xff, 0x80, 0xff, 0x80, 0x18, 0x3f, 0xf0, 0x00, 0x00, 0x04, 0x32, 0x20,
    0x04, 0x3e, 0x20, 0x00, 0xff, 0x80, 0xff, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00, 0x04, 0x3e, 0x20,
    0x04, 0x26, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x26, 0x20,
    0x04, 0x62, 0x21, 0x80, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x80, 0x07, 0x04, 0x62, 0x20,
    0x02, 0x7e, 0x41, 0x80, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x01, 0x80, 0x07, 0x02, 0x7e, 0x40,
    0x02, 0x30, 0x41, 0x80, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 0x02, 0x30, 0x40,
    0x01, 0x00, 0x81, 0x80, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 0x01, 0x00, 0x80,
    0x00, 0xc3, 0x01, 0x9f, 0x8f, 0xc7, 0xcf, 0xef, 0xc0, 0xf1, 0xf9, 0x9f, 0xc7, 0x00, 0xc3, 0x00,
    0x00, 0x3c, 0x01, 0x9f, 0xcc, 0x4c, 0xcf, 0xe7, 0x01, 0xf9, 0xf9, 0x9f, 0xe7, 0x00, 0x3c, 0x00,
    0x00, 0x00, 0x01, 0x98, 0xce, 0x0c, 0x6e, 0x07, 0x01, 0x83, 0x8d, 0x9c, 0x67, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x98, 0x47, 0x8f, 0xee, 0x07, 0x01, 0x83, 0x0d, 0x9c, 0x67, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x98, 0x41, 0xcc, 0x0e, 0x07, 0x01, 0x83, 0x0d, 0x9c, 0x67, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x98, 0x40, 0xec, 0x6e, 0x07, 0x01, 0x8b, 0x8d, 0x9c, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x98, 0x4f, 0xcf, 0xee, 0x07, 0x01, 0xf9, 0xf9, 0x9c, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x98, 0x47, 0x87, 0xce, 0x07, 0x00, 0xf0, 0xf1, 0x9c, 0x67, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 객체 생성 (NeoPixel, DHT 센서, OLED 디스플레이)
NeoPixelConnect myLEDStrip(PIXEL_CONTROL_PIN, MAXIMUM_NUM_NEOPIXELS, pio0, 0);
DHT myDHT(DHT_PIN, DHTTYPE);
Adafruit_SSD1306 myOledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
BH1750 lightMeter;

// 함수 선언 (프로토타이핑)
void initializeOLED();
void displaySensorData(float temperature, float humidity, float lightLevel);
void sendHSV_ColorDataToCloud(void);
void onInterruptTriggerChangeHandler();
void myBtnStatusCheckHandler();

// 초기 설정 함수 (한 번만 실행)
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  myDHT.begin();
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
  pinMode(TOUCH_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(TOUCH_SENSOR_PIN, onInterruptTriggerChangeHandler, CHANGE);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  initializeOLED();
}

// 메인 루프 (계속 반복 실행)
void loop()
{
  myBtnStatusCheckHandler();
  unsigned long currentMillis = millis();
  if (currentMillis - lastCloudUpdate >= cloudUpdateInterval)
  {
    ArduinoCloud.update();
    lastCloudUpdate = currentMillis;
  }

  if (!flgInitialDisplayDone && currentMillis - previousMillis >= displayDelay)
  {
    flgInitialDisplayDone = true;
    myOledDisplay.clearDisplay();
  }

  if (flgInitialDisplayDone)
  {
    fHumidity = myDHT.readHumidity();
    fTemperature = myDHT.readTemperature();
    fLightBrightness = lightMeter.readLightLevel();

#if (DEBUG_MODE_ENABLE == 1)
    Serial.print("Humidity: ");
    Serial.print(fHumidity);
    Serial.print("%   Temperature: ");
    Serial.print(fTemperature);
    Serial.print("^C   Light: ");
    Serial.print(fLightBrightness);
    Serial.println(" lx");
#endif

    // 온습도 및 조도 값 OLED에 표시
    displaySensorData(fTemperature, fHumidity, fLightBrightness);
  }
}

// OLED 초기화 및 초기 화면 표시 함수
void initializeOLED()
{
  if (!myOledDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
#if (DEBUG_MODE_ENABLE == 1)
    Serial.println(F("SSD1306 allocation failed"));
#endif
    for (;;)
      ;
  }

  myOledDisplay.clearDisplay();
  myOledDisplay.drawBitmap(0, 0, epd_bitmap_oled_background, 128, 32, 1);
  myOledDisplay.display();

  previousMillis = millis();
}

// OLED에 온도, 습도, 조도 데이터 표시 함수
void displaySensorData(float temperature, float humidity, float lightLevel)
{
  myOledDisplay.clearDisplay();
  myOledDisplay.setTextSize(1);
  myOledDisplay.setTextColor(SSD1306_WHITE);
  myOledDisplay.setCursor(0, 0);

  myOledDisplay.print("Temp: ");
  myOledDisplay.print(temperature);
  myOledDisplay.print((char)247);
  myOledDisplay.println("C");

  myOledDisplay.print("Hum: ");
  myOledDisplay.print(humidity);
  myOledDisplay.println("%");

  myOledDisplay.print("Light: ");
  myOledDisplay.print(lightLevel);
  myOledDisplay.println(" lx");

  myOledDisplay.display();
}

/*Arduino Iot Cloud*/
// 현재 NeoPixel LED의 RGB 값을 HSV로 변환하여 클라우드로 전송하는 함수
void sendHSV_ColorDataToCloud(void)
{
  rgb_to_hsv(redColor, greenColor, blueColor);

  setRGBcolor.setHue(hue);
  setRGBcolor.setSaturation(saturation);
  setRGBcolor.setBrightness(value);
}

// RGB 값을 HSV로 변환하는 함수
void rgb_to_hsv(double r, double g, double b)
{
  r = r / 255.0;
  g = g / 255.0;
  b = b / 255.0;

  double cmax = max(r, max(g, b)); // r, g, b 중 최대값
  double cmin = min(r, min(g, b)); // r, g, b 중 최소값
  double diff = cmax - cmin;       // 최대값과 최소값의 차이
  hue = -1;
  saturation = -1;

  // Hue 계산
  if (cmax == cmin)
    hue = 0;
  else if (cmax == r)
    hue = fmod(60 * ((g - b) / diff) + 360, 360);
  else if (cmax == g)
    hue = fmod(60 * ((b - r) / diff) + 120, 360);
  else if (cmax == b)
    hue = fmod(60 * ((r - g) / diff) + 240, 360);

  // Saturation 계산
  if (cmax == 0)
    saturation = 0;
  else
    saturation = (diff / cmax) * 100;

  // Brightness (Value) 계산
  value = cmax * 100;
}

/*Touch Senser*/
void onInterruptTriggerChangeHandler()
{
  if (digitalRead(TOUCH_SENSOR_PIN))
  {
    flgButtonPressedChk = true;
    flgExternalInterruptDetected = true;
  }
  else
  {
    flgButtonPressedChk = false;
    flgExternalInterruptDetected = true;
  }
}

void myBtnStatusCheckHandler()
{
  if (!flgExternalInterruptDetected)
    return;
  if ((unsigned long)(millis() - btnChkTimePreviousMillis) >= btnChkTimeInterval)
  {
    if (!digitalRead(TOUCH_SENSOR_PIN))
    {
      flgButtonPressedStatus = true;
      flgButtonReleasedStatus = false;
    }
    if (flgButtonPressedChk)
    {
      if (!flgButtonPressedStatus)
      {
        Serial.println("Touch Button Pressed");
        cntKeyInTimingCtrl++;
        if (!flgOnOffCtrlStatus && (cntKeyInTimingCtrl <= KEY_IN_TIME_SHORT_CHECK_VALUE))
        {
          Serial.println("Mode 1: White Color");
          cntKeyInTimingCtrl = 0;
          flgButtonPressedStatus = true;
          flgButtonReleasedStatus = false;
          flgOnOffCtrlStatus = true;
          digitalWrite(LED_BUILTIN, ON);
          myLEDStrip.neoPixelFill(255, 255, 255, true);
          flgExternalInterruptDetected = false;
          flgModeChangeCtrl = true;
          seqSystemModeControl = 5;
        }
        else if (flgOnOffCtrlStatus && (cntKeyInTimingCtrl >= KEY_IN_TIME_LONG_CHECK_VALUE))
        {
          Serial.println("Power Off");
          cntKeyInTimingCtrl = 0;
          flgButtonPressedStatus = true;
          flgButtonReleasedStatus = false;
          seqSystemModeControl = 0;
          flgOnOffCtrlStatus = false;
          digitalWrite(LED_BUILTIN, OFF);
          myLEDStrip.neoPixelFill(0, 0, 0, true);
          flgExternalInterruptDetected = false;
        }

        if (flgOnOffCtrlStatus)
        {
          switch (seqSystemModeControl)
          {
          case 5:
            if (!flgModeChangeCtrl)
              seqSystemModeControl = 10;
            break;
          case 10:
            Serial.println("Mode 2: Red Color");
            myLEDStrip.neoPixelFill(255, 0, 0, true);
            flgModeChangeCtrl = true;
            seqSystemModeControl = 15;
            break;
          case 15:
            if (!flgModeChangeCtrl)
              seqSystemModeControl = 20;
            break;
          case 20:
            Serial.println("Mode 3: Green Color");
            myLEDStrip.neoPixelFill(0, 255, 0, true);
            flgModeChangeCtrl = true;
            seqSystemModeControl = 25;
            break;
          case 25:
            if (!flgModeChangeCtrl)
              seqSystemModeControl = 30;
            break;
          case 30:
            Serial.println("Mode 4: Blue Color");
            myLEDStrip.neoPixelFill(0, 0, 255, true);
            flgModeChangeCtrl = true;
            seqSystemModeControl = 35;
            break;
          case 35:
            if (!flgModeChangeCtrl)
              seqSystemModeControl = 40;
            break;
          case 40:
            Serial.println("Mode 5: All OFF");
            myLEDStrip.neoPixelFill(0, 0, 0, true);
            flgModeChangeCtrl = true;
            seqSystemModeControl = 45;
            break;
          case 45:
            if (!flgModeChangeCtrl)
              seqSystemModeControl = 50;
            break;
          case 50:
            Serial.println("Mode 1: White Color");
            myLEDStrip.neoPixelFill(255, 255, 255, true);
            flgModeChangeCtrl = true;
            seqSystemModeControl = 55;
            break;
          case 55:
            if (!flgModeChangeCtrl)
              seqSystemModeControl = 10;
            break;
          default:
            break;
          }
        }
      }
    }
    else
    {

      if (!flgButtonReleasedStatus)
      {
        Serial.println("Touch Button Released");
        cntKeyInTimingCtrl = 0;
        flgButtonPressedStatus = false;
        flgButtonReleasedStatus = true;
        flgModeChangeCtrl = false;
        flgExternalInterruptDetected = false;
      }
    }
    btnChkTimePreviousMillis = millis();
  }
}