# 🏆 Jake IoT Smart Arcade Machine
**글로벌 라이프 스타일 가전 메이커톤 대상 수상작**

Arduino Nano RP2040 Connect 기반 AI 음성인식과 IoT 기술을 융합한 스마트 조명 제어 시스템

---

## 🎯 프로젝트 개요

스마트홈 환경에서 사용자 맞춤형 조명 제어 기능을 제공하는 혁신적인 IoT 디바이스입니다. Edge Impulse AI 모델을 활용한 음성 명령 인식, Arduino Cloud를 통한 원격 제어, 그리고 React Native 블루투스 앱을 통한 직관적인 사용자 인터페이스를 제공합니다.

### 🏅 수상 내역
- **글로벌 라이프 스타일 가전 메이커톤 대상 (1등)**

---
## ⚡ 핵심 기능

### 1. 🎤 AI 음성 명령 제어
- **Edge Impulse** 기반 음성분류 AI 모델 직접 제작
- "Red", "Green", "Blue", "On", "Off" 음성 명령 인식
- 실시간 마이크 입력 처리 및 추론
- Arduino Nano RP2040의 PDM 마이크 활용

### 2. 📱 React Native 블루투스 앱
- 직관적인 색상 선택 인터페이스
- 실시간 블루투스 통신을 통한 LED 제어
- RGB 색상 조합 및 밝기 조절 기능
- [블루투스 앱 GitHub](https://github.com/makerthon-filament/BluetoothArduinoApp)

### 3. ☁️ Arduino Cloud 통합
- 클라우드 기반 원격 조명 제어
- 센서 데이터 실시간 모니터링
- HSV 색상 체계를 통한 정밀한 색상 제어
- IoT 대시보드를 통한 디바이스 상태 확인

### 4. 🗣️ Amazon Alexa 연동
- "Alexa, turn on the lights" 음성 명령 지원
- AWS Alexa Skills Kit 활용
- 스마트홈 생태계 완벽 통합

### 5. 🌡️ 환경 센서 모니터링
- **DHT11**: 온도 및 습도 측정
- **BH1750**: 조도 센서를 통한 자동 밝기 조절
- **OLED 디스플레이**: 실시간 센서 데이터 표시

### 6. 🎨 스마트 LED 제어
- **NeoPixel LED 스트립** (12개 LED)
- 터치 센서를 통한 물리적 제어
- 다양한 모드별 색상 효과
- 레인보우 이펙트 및 동적 패턴

---

## 🛠️ 기술 스택

### Hardware
- **MCU**: Arduino Nano RP2040 Connect
- **센서류**: DHT11 (온습도), BH1750 (조도), 터치 센서
- **출력 장치**: NeoPixel LED Strip (WS2812B), OLED Display (SSD1306)
- **통신**: WiFi, Bluetooth Low Energy, 내장 마이크

### Software & Platforms
- **개발 환경**: PlatformIO, Arduino IDE
- **AI/ML**: Edge Impulse Studio
- **클라우드**: Arduino IoT Cloud
- **모바일 앱**: React Native
- **음성 AI**: Amazon Alexa Skills Kit
- **프로토콜**: MQTT, BLE, HTTP/HTTPS

### Libraries & Dependencies
```cpp
// 핵심 라이브러리
#include <ArduinoIoTCloud.h>        // Arduino Cloud 연동
#include <ArduinoBLE.h>             // 블루투스 저전력 통신
#include <NeoPixelConnect.h>        // WS2812B LED 제어
#include <PDM.h>                    // 마이크 입력 처리
#include <NanoRp2040MicTest_inferencing.h> // Edge Impulse AI 모델
#include <DHT.h>                    // 온습도 센서
#include <BH1750.h>                 // 조도 센서
#include <Adafruit_SSD1306.h>       // OLED 디스플레이
```

---

## 🏗️ 시스템 아키텍처

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   사용자 음성    │───▶│  Edge Impulse AI │───▶│   LED 제어      │
│   명령 입력      │    │   음성 분류      │    │   (색상 변경)    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│ React Native    │◄──▶│ Arduino Nano     │◄──▶│ Arduino Cloud   │
│ 블루투스 앱     │    │ RP2040 Connect   │    │ IoT Dashboard   │
└─────────────────┘    └──────────────────┘    └─────────────────┘
         │                       │                       │
         ▼                       ▼                       ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│ 직관적인 UI로   │    │ 센서 데이터 수집 │    │ Amazon Alexa    │
│ 색상 제어       │    │ 및 OLED 표시     │    │ 음성 제어       │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

---

## 🔧 주요 구현 내용

### 1. AI 음성 인식 시스템
```cpp
void runEdgeImpulseInference() {
    bool m = microphone_inference_record();
    if (!m) {
        Serial.println("ERR: Failed to record audio...");
        return;
    }
    
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &microphone_audio_signal_get_data;
    ei_impulse_result_t result = {0};
    
    EI_IMPULSE_ERROR res = run_classifier_continuous(&signal, &result, debug_nn);
    if (res != EI_IMPULSE_OK) {
        Serial.println("ERR: Failed to run classifier");
        return;
    }
    process_inference_result(result);
}

void process_inference_result(ei_impulse_result_t result) {
    if (result.classification[5].value > 0.8) {
        Serial.println("Detected: Red");
        myLEDstrip.neoPixelFill(255, 0, 0, true);
    }
    else if (result.classification[1].value > 0.8) {
        Serial.println("Detected: Green");
        myLEDstrip.neoPixelFill(0, 255, 0, true);
    }
    // ... 추가 색상 처리
}
```

### 2. 다중 모드 시스템 구조
```cpp
// 시스템 모드 정의
#define WIFI_MODE 0
#define BLE_MODE 1
#define BLE_CHANGE_ACTIVE 2
#define BLE_CHANGE_INACTIVE 3
#define AI_PREPARE 4
#define AI_RUN 5
#define AI_END 6

void loop() {
    switch (setIotMode) {
        case WIFI_MODE:
            // WiFi 및 Arduino Cloud 처리
            ArduinoCloud.update();
            break;
        
        case BLE_MODE:
            // 블루투스 통신 처리
            BLE.poll();
            break;
            
        case AI_RUN:
            // AI 음성 인식 실행
            runEdgeImpulseInference();
            break;
    }
}
```

### 3. 센서 통합 및 환경 반응형 조명
```cpp
void displaySensorData(float temperature, float humidity, float lightLevel) {
    myOledDisplay.clearDisplay();
    myOledDisplay.setTextSize(1);
    myOledDisplay.setTextColor(SSD1306_WHITE);
    
    myOledDisplay.print("Temp: "); myOledDisplay.print(temperature); myOledDisplay.println("°C");
    myOledDisplay.print("Hum: "); myOledDisplay.print(humidity); myOledDisplay.println("%");
    myOledDisplay.print("Light: "); myOledDisplay.print(lightLevel); myOledDisplay.println(" lx");
    
    myOledDisplay.display();
}
```

---

## 📊 프로젝트 구조

```
Jake_IoT_ArcadeMachine/
├── Jake_IoT_Controller/          # 메인 컨트롤러 코드
│   ├── src/main.cpp             # 핵심 제어 로직
│   ├── include/thingProperties.h # Arduino Cloud 설정
│   └── platformio.ini           # 프로젝트 설정
├── test/                        # 개별 기능 테스트 모듈
│   ├── BLE/                     # 블루투스 + AI 음성인식
│   ├── Arduino_Iot_Cloud/       # 클라우드 연동 테스트
│   ├── alexa/                   # Alexa 연동 테스트
│   ├── Touch_onOff/             # 터치 센서 제어
│   ├── BH1750IC_senser/         # 조도 센서 테스트
│   ├── NeoPixel_OnOff/          # LED 제어 테스트
│   └── interval_LED/            # LED 패턴 효과
└── README.md                    # 프로젝트 문서
```

---

## 🚀 실행 방법

### 1. 하드웨어 준비
- Arduino Nano RP2040 Connect
- DHT11 온습도 센서 (Pin 2)
- BH1750 조도 센서 (I2C)
- NeoPixel LED Strip (Pin 15)
- OLED Display SSD1306 (I2C)
- 터치 센서 (Pin 4)

### 2. 소프트웨어 설치
```bash
# PlatformIO 설치
pip install platformio

# 프로젝트 클론
git clone https://github.com/makerthon-filament/Jake_IoT_ArcadeMachine.git
cd Jake_IoT_ArcadeMachine

# 라이브러리 설치 및 빌드
pio lib install
pio run
```

### 3. Edge Impulse AI 모델 설정
1. [Edge Impulse Studio](https://studio.edgeimpulse.com/)에서 계정 생성
2. 음성 데이터 수집 ("Red", "Green", "Blue", "On", "Off")
3. 모델 학습 및 Arduino 라이브러리 다운로드
4. `NanoRp2040MicTest_inferencing.h` 포함

### 4. Arduino Cloud 설정
1. [Arduino Cloud](https://create.arduino.cc/iot/)에서 Thing 생성
2. Wi-Fi 자격 증명 설정
3. 변수 연결 (온도, 습도, 조도, RGB 색상)

---

## 🎥 시연 영상 및 사진

### 📸 하드웨어 구성
![하드웨어 설정 예시]
- Arduino Nano RP2040 Connect를 중심으로 한 센서 및 액추에이터 연결
- 깔끔한 브레드보드 배선으로 전문적인 프로토타입 완성

### 🎬 주요 기능 시연
1. **음성 명령 인식**: "Red"라고 말하면 LED가 빨간색으로 변환
2. **블루투스 앱 제어**: 스마트폰 앱에서 실시간 색상 변경
3. **Alexa 연동**: "Alexa, turn on the lights" 명령 실행
4. **센서 반응**: 환경 변화에 따른 자동 조명 조절

### 📱 React Native 앱 스크린샷
- 직관적인 색상 선택 휠
- 실시간 연결 상태 표시
- 밝기 조절 슬라이더

---

## 🏆 기술적 성과 및 혁신점

### 1. **AI-IoT 융합 기술력**
- Edge AI를 활용한 실시간 음성 인식 처리
- 클라우드와 엣지 컴퓨팅의 효율적인 하이브리드 구조

### 2. **다중 플랫폼 통합**
- Arduino Cloud, React Native, Amazon Alexa 완벽 연동
- 하나의 디바이스로 다양한 제어 방식 제공

### 3. **확장 가능한 모듈러 구조**
- 각 기능별 독립적인 테스트 모듈 구성
- 새로운 센서나 기능 추가 용이

### 4. **사용자 중심 설계**
- 직관적인 터치, 음성, 앱 제어 방식
- 실시간 피드백을 통한 우수한 사용자 경험


---

## 📞 연락처 및 리소스

### 프로젝트 링크
- **메인 GitHub**: [Jake_IoT_ArcadeMachine](https://github.com/makerthon-filament/Jake_IoT_ArcadeMachine)
- **블루투스 앱**: [BluetoothArduinoApp](https://github.com/makerthon-filament/BluetoothArduinoApp)

### 기술 스택 문서
- [Arduino Nano RP2040 Connect](https://docs.arduino.cc/hardware/nano-rp2040-connect)
- [Edge Impulse Documentation](https://docs.edgeimpulse.com/)
- [Arduino IoT Cloud](https://docs.arduino.cc/arduino-cloud/)
- [React Native BLE Manager](https://github.com/innoveit/react-native-ble-manager)
