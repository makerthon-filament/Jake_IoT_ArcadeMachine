#include <BH1750.h> // BH1750 조도 센서 라이브러리

BH1750 lightMeter; // BH1750 센서 객체 생성

void setup()
{
  Serial.begin(115200); // 시리얼 통신 시작 (속도: 115200bps)
  Wire.begin();         // I2C 통신 초기화

  // BH1750 센서 초기화
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println("BH1750 초기화 성공!"); // 센서 초기화 성공 메시지
  }
  else
  {
    Serial.println("BH1750 초기화 실패. 연결 확인 필요."); // 센서 초기화 실패 시 메시지
    while (1)
      ; // 초기화 실패 시 무한 루프
  }
}

void loop()
{
  float lux = lightMeter.readLightLevel(); // 조도(lux) 값 읽기

  // 조도 값 출력
  Serial.print("현재 조도: ");
  Serial.print(lux);
  Serial.println(" lx"); // 조도의 단위: lux (lx)

  delay(1000); // 1초마다 조도 값 갱신
}
