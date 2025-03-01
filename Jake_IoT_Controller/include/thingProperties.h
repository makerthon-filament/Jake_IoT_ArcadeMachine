#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[] = "feelon";      // Network SSID (name)
const char PASS[] = "01041531190"; // Network password (use for WPA, or use as key for WEP)

void onFHumidityChange();
void onFLightBrightnessChange();
void onFTemperatureChange();
void onSetRGBcolorChange();
void onIntSetModeChange();

float fHumidity;
float fLightBrightness;
float fTemperature;
CloudColoredLight setRGBcolor;
int intSetMode;

void initProperties()
{
  ArduinoCloud.addProperty(fHumidity, READWRITE, ON_CHANGE, onFHumidityChange);
  ArduinoCloud.addProperty(fLightBrightness, READWRITE, ON_CHANGE, onFLightBrightnessChange);
  ArduinoCloud.addProperty(fTemperature, READWRITE, ON_CHANGE, onFTemperatureChange);
  ArduinoCloud.addProperty(setRGBcolor, READWRITE, ON_CHANGE, onSetRGBcolorChange);
  ArduinoCloud.addProperty(intSetMode, READWRITE, ON_CHANGE, onIntSetModeChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

void onFHumidityChange()
{
  // 습도 변경 시 동작 코드
}

void onFLightBrightnessChange()
{
  // 조도 변경 시 동작 코드
}

void onFTemperatureChange()
{
  // 온도 변경 시 동작 코드
}

void onSetRGBcolorChange()
{
  // 색상 변경 시 동작 코드
}

void onIntSetModeChange()
{
  switch (intSetMode)
  {
  case 0:
    Serial.println("Mode 0");
    break;

  case 1:
    Serial.println("Mode 1");

    break;

  case 2:
    Serial.println("Mode 2");
    break;

  case 3:
    Serial.println("Mode 3");
    break;

  case 4:
    Serial.println("Mode 4");
    break;

  default:
    Serial.println("Unknown Mode");
    break;
  }
}