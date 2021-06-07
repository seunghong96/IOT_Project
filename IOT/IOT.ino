#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> // HTTP 클라이언트
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <string>
#include "purifier.h"

#define DELAY_MS1 600000

WiFiClient client; // 와이파이 클라이언트 객체
WiFiUDP s_udp;
HTTPClient http; // HTTP 클라이언트 객체
DynamicJsonDocument test(2048);
IPAddress s_purifier_ip(192,168,50,11);

const std::string s_token = "9fa924eb745032c50274714b657b928d";
String key = "824nEXFPA5w%2BbQStRUNDo2QiDycTMUY39F9nb5Xe%2F3%2FyPg8BNA08g28qgzf1eMKqubBggebiXgd0i0OIXgr2eg%3D%3D";  //인코딩 된 값 그대로 넣으면됨
String url = "http://apis.data.go.kr/B552584/ArpltnInforInqireSvc/getMsrstnAcctoRltmMesureDnsty?serviceKey=824nEXFPA5w%2BbQStRUNDo2QiDycTMUY39F9nb5Xe%2F3%2FyPg8BNA08g28qgzf1eMKqubBggebiXgd0i0OIXgr2eg%3D%3D&returnType=json&numOfRows=1&pageNo=1&stationName=%EA%B9%80%EB%9F%89%EC%9E%A5%EB%8F%99&dataTerm=DAILY&ver=1.0";

unsigned long long lastairkoreaMs=0;
unsigned long long purifier_run_time=0;
int level;

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.print("Connecting to ");

    const char *ssid = "ESL_LAB2G";
    const char *password = "esllab6373";
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");

    delay(100);
    s_udp.begin(54320);
    
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
            level = atoi(pm10Grade);
            level = 3*level;
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
      
    Serial.printf("%d\n",level);
    
    }
    
    purifier_run_time = millis();
    
    if((millis()-purifier_run_time) < 210000) 
    handle_purifier(s_udp, s_purifier_ip, s_token,level);
        
    /*
    while(1)
    {    
      if((millis()-purifier_run_time) == 210000) break;
      handle_purifier(s_udp, s_purifier_ip, s_token,level);
    }
    */
}