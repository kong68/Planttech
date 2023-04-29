#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <time.h>

#include <DHT.h>  // 온습도 센서 라이블러리

#include <OneWire.h>
#include <DallasTemperature.h>  //DS18B20 물온도 센서 라이브러리

#include <LiquidCrystal_I2C.h>   //LCD 라이브러리

#define ONE_WIRE_BUS D7   //물 온도센서
#define DHTPIN D6         // DHT11 센서의 DATA를 연결한 DIGITAL 포트 (온습도 센서)
#define DHTTYPE DHT11       // DHT11을 사용함을 명시

#define SERVER_IP "  "  // db서버의 주소
#define STASSID ";"   // WiFi SSID 입력
#define STAPSK  ""   // WiFi 비밀번호 입력


OneWire oneWire(ONE_WIRE_BUS);
DHT dht(DHTPIN, DHTTYPE);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD 주소

// 전역변수 선언
/////////////////////////////////////////// 온습도 센서
float temp = 0.0;
float humi = 0.0; 
float f = 0.0;
/////////////////////////////////////////// 조도센서

int cds = 0;

////////////////////////////////////////// 물온도 센서

float waterTemp = 0.0;

/////////////////////////////////////////

struct tm *lc;          // 내가 원하는대로 날짜형식 작성을 위해 필요한 구조체
WiFiClient client;
HTTPClient http;

void temp_humi(void){ //온습도 센서 함수
temp = dht.readTemperature();
humi = dht.readHumidity(); 
f = dht.readTemperature(true);

if (isnan(humi) || isnan(temp) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
    Serial.print("temperature:"); 

    Serial.print(temp); //온도값 출력

    Serial.print(" humidity:");

    Serial.print(humi); //습도값 출력

    Serial.println();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("temp   humi");
    lcd.setCursor(0,1);
    lcd.print(temp);
    lcd.print("  ");
    lcd.print(humi);
    
}

void CDS(void){  //조도센서 함수
  cds = analogRead(A0);
  Serial.print("CDS: ");
  Serial.println(cds);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("illuminance");
  lcd.setCursor(0,1);
  lcd.print(cds);
  
}

void WTemp(){
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  waterTemp = sensors.getTempCByIndex(0);

// Check if reading was successful
  if(waterTemp != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(waterTemp);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WaterTemp");
    lcd.setCursor(0,1);
    lcd.print(waterTemp);
  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }


}

void Display_LCD(void){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(cds); 

}



void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  configTime(-(3600 * 9), 0, "1.kr.pool.ntp.org"); // 9시간 시차, 서머타임 적용 X

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  Serial.println("");
  Serial.print("WiFi Connected! IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println(F("Gather Temperature, Humidity, Concentrations Of Gases !"));
  Serial.println(F("Wait 60 Seconds...\n"));
  dht.begin();
  sensors.begin();  // 물온도
  lcd.init(); //LCD 초기화
  lcd.backlight(); //LCD 백라이트

}

String pluszero(int value) {    // localtime을 통해 생성된 구조체의 값이 10 미만이면
  String out;           // 한자리 숫자로 나온다. 예를들어 9일이면 09가 아닌
  if (value < 10) {       // 9로 나와서 한자리 숫자일 경우 앞에 0을 추가하는
    out.concat("0");        // 작업을 하는 함수이다.
    out.concat(String(value));
    return out;
  } else {
    out = String(value);
    return out;
  }
}

String calcdate(struct tm *t) {       // 현재 시간을 DATETIME 형식으로 바꾸어
  String out = String((1900 + t->tm_year)); // String 타입으로 return 한다.
  out.concat("-");
  out.concat(pluszero((t->tm_mon) + 1));
  out.concat("-");
  out.concat(pluszero(t->tm_mday));
  out.concat(" ");
  out.concat(pluszero(t->tm_hour));
  out.concat(":");
  out.concat(pluszero(t->tm_min));
  out.concat(":");
  out.concat(pluszero(t->tm_sec));
  return out;
}

void loop() {

  delay(2000);  // 측정 간 30간의 지연을 설정한다.


  time_t now = time(nullptr);
  lc = localtime(&now);   // 현재 시간을 localtime을 통한 구조체 설정

  //float temp, humi; //온도와 습도 값이 저장될 변수 온습도값 소수 float 


  temp_humi(); //온습도센서 함수


  //delay(DHT11_RETRY_DELAY); // DHT11에서 권장하는 딜레이함수를 사용, 정상적인 값
  delay(2000);


  CDS();

  delay(2000);

  WTemp();

  delay(2000);


  // 시간 영역
  Serial.print(F("기준 시간 : "));
  String date = calcdate(lc);
  Serial.println(date);
  Serial.println("");

  if ((WiFi.status() == WL_CONNECTED)) {

    Serial.print("[HTTP] 서버 연결을 시도합니다...\n");

    http.begin(client, ""); // 요청을 보낼 URL 입력
    //http.begin(client, "  "); // 요청을 보낼 URL 입력
    http.addHeader("Content-Type", "application/json");
    // POST 요청을 할때 전송 방식을 정한다.

    Serial.print("[HTTP] 수집한 값의 POST 요청을 시도합니다...\n");

    String POSTBODY = "";
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["plantSensorNo"] = 0;
    root["plantNo"] = 0;
    root["warehousePlantNo"] = 0;
    root["dhtNo"] = 0;
    root["photoRegistorNo"] = 0;
    root["waterTempNo"] = 0;
  
    root["humi"] = humi;
    root["temp"] = temp;
    root["waterTemp"] = waterTemp;
    root["light"] = cds;
    
    root.printTo(POSTBODY);


    int httpCode = http.POST(POSTBODY); // 위에 작성한 URL로 POST 요청을 보낸다.

    if (httpCode > 0) {
      // HTTP 헤더를 전송하고 그에 대한 응답을 핸들링하는 과정
      Serial.printf("[HTTP] 응답 Code : %d\n", httpCode);

      // HTTP 응답 200, 즉 정상응답이면 서버로부터 수신된 응답을 출력한다.
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.print("서버로부터 수신된 응답 : ");
        Serial.println(payload);
        Serial.println("");
      }
    } else {    // 에러발생시 에러내용을 출력한다.
      Serial.printf("[HTTP]초 POST 요청이 실패했습니다. 오류 : %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}