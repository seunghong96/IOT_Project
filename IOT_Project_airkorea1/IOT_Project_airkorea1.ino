#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> // HTTP 클라이언트
#include <ArduinoJson.h>

#define DELAY_MS1 600000

WiFiClient client; // 와이파이 클라이언트 객체
HTTPClient http; // HTTP 클라이언트 객체
DynamicJsonDocument test(2048);

String key = "824nEXFPA5w%2BbQStRUNDo2QiDycTMUY39F9nb5Xe%2F3%2FyPg8BNA08g28qgzf1eMKqubBggebiXgd0i0OIXgr2eg%3D%3D";  //인코딩 된 값 그대로 넣으면됨
String url = "http://apis.data.go.kr/B552584/ArpltnInforInqireSvc/getMsrstnAcctoRltmMesureDnsty?serviceKey=824nEXFPA5w%2BbQStRUNDo2QiDycTMUY39F9nb5Xe%2F3%2FyPg8BNA08g28qgzf1eMKqubBggebiXgd0i0OIXgr2eg%3D%3D&returnType=json&numOfRows=1&pageNo=1&stationName=%EA%B9%80%EB%9F%89%EC%9E%A5%EB%8F%99&dataTerm=DAILY&ver=1.0";

unsigned long long lastairkoreaMs=0;

void setup() {

  Serial.begin(115200);

  WiFi.begin("ESL_LAB2G","esllab6373");
  while(1)
  {
    if(WiFi.status()==WL_CONNECTED) break;
    else 
    {
      Serial.printf(".");
    }
    delay(100);
  }
  Serial.printf("Connect...\r\n");
  Serial.printf("Please contact IP Addr...");
  Serial.println(WiFi.localIP());
}


void loop() {

  if(millis()-lastairkoreaMs> DELAY_MS1)
  {  
    lastairkoreaMs =millis(); 

    if (http.begin(url)) 
    {  // HTTP
      // 서버에 연결하고 HTTP 헤더 전송
      int httpCode = http.GET();
      Serial.printf("%d\n",httpCode);

      // httpCode 가 음수라면 에러
      if (httpCode > 0) 
      { // 에러가 없는 경우
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
        {
          String payload = http.getString(); // 받은 json 데이터를 String에 저장
          Serial.println(payload);

          deserializeJson(test,payload); //해석 완료
    
          const char* pm10Grade= test["response"]["body"]["items"][0]["pm10Grade"];
          const char* pm25Grade= test["response"]["body"]["items"][0]["pm25Grade"];
          
          Serial.printf("현재 pm10Grade는 : %s입니다\r\n",pm10Grade);
          Serial.printf("pm25Grade는 %s입니다.\r\n",pm25Grade);
         
          Serial.println(pm10Grade);
          Serial.println(pm25Grade);
        }
      } 
      else 
      {
        Serial.printf("[HTTP] GET... 실패, 에러코드: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } 
    else 
    {
      Serial.printf("[HTTP] 접속 불가\n");
    }

    Serial.printf("end\n");
  }
}
