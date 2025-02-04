#include <NeoPixelConnect.h>
#include <DHT.h>

#define DEBUG_MODE_ENABLE 1

// NeoPixel LED 설정
#define PIXEL_CONTROL_PIN 15
#define MAXIMUM_NUM_NEOPIXELS 12

// DHT 온습도 센서 설정
#define DHT_PIN 2
#define DHTTYPE DHT11

// 전역 변수
unsigned char redColor = 0, greenColor = 0, blueColor = 0;
float fHumidity, fTemperature;

bool flgEmotionalModeEnable = true;

NeoPixelConnect myLEDStrip(PIXEL_CONTROL_PIN, MAXIMUM_NUM_NEOPIXELS, pio0, 0);
DHT myDHT(DHT_PIN, DHTTYPE);

void setup()
{
  Serial.begin(115200);
  myDHT.begin();
}

void loop()
{
  fHumidity = myDHT.readHumidity();
  fTemperature = myDHT.readTemperature();
  unsigned char getTempData;

#if (DEBUG_MODE_ENABLE == 1)
  Serial.print("Humidity: ");
  Serial.print(fHumidity);
  Serial.print("%   Temperature: ");
  Serial.print(fTemperature);
  Serial.print("^C");
#endif

  if (flgEmotionalModeEnable)
  {
    getTempData = fTemperature + fHumidity / 100;
    if (fHumidity > 70)
      getTempData += 5;
  }

#if (DEBUG_MODE_ENABLE == 1)
  Serial.print(getTempData);
  Serial.print(F("^C"));
#endif

  if (fTemperature >= 32)
  {
    redColor = 255;
    greenColor = 89;
    blueColor = 89;
  }
  else if (fTemperature >= 26)
  {
    redColor = 254;
    greenColor = 155;
    blueColor = 120;
  }
  else if (fTemperature >= 18)
  {
    redColor = 255;
    greenColor = 255;
    blueColor = 255;
  }
  else if (fTemperature >= 0)
  {
    redColor = 58;
    greenColor = 199;
    blueColor = 131;
  }
  else
  {
    redColor = 126;
    greenColor = 225;
    blueColor = 225;
  }

  myLEDStrip.neoPixelFill(redColor, greenColor, blueColor, true);
}