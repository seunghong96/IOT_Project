#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> // HTTP 클라이언트
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <string>
#include "purifier.h"

#define D1 5
#define D2 4
#define D0 0

#define DELAY_MS1 60000
#define DELAY_MS2 1000

WiFiClient client; // 와이파이 클라이언트 객체
WiFiUDP s_udp;
HTTPClient http; // HTTP 클라이언트 객체
DynamicJsonDocument test(2048);
IPAddress s_purifier_ip(192,168,50,11);

const std::string s_token = "70f3fb848009bd8cec7dad45ae036ba0";
String key = "824nEXFPA5w%2BbQStRUNDo2QiDycTMUY39F9nb5Xe%2F3%2FyPg8BNA08g28qgzf1eMKqubBggebiXgd0i0OIXgr2eg%3D%3D";  //인코딩 된 값 그대로 넣으면됨
String url = "http://apis.data.go.kr/B552584/ArpltnInforInqireSvc/getMsrstnAcctoRltmMesureDnsty?serviceKey=824nEXFPA5w%2BbQStRUNDo2QiDycTMUY39F9nb5Xe%2F3%2FyPg8BNA08g28qgzf1eMKqubBggebiXgd0i0OIXgr2eg%3D%3D&returnType=json&numOfRows=1&pageNo=1&stationName=%EA%B9%80%EB%9F%89%EC%9E%A5%EB%8F%99&dataTerm=DAILY&ver=1.0";

unsigned long long lastairkoreaMs=0;
unsigned long long lastflagMs=0;
unsigned long long purifier_run_time=0;
int last_level,level,previous_level;
int flag =0;
int count =0;    

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

    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D0, OUTPUT); 
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

            last_level = atoi(pm10Grade);
            
            Serial.printf("Before\nlast_level : %d\n",last_level);
            Serial.printf("previous_level : %d\n",previous_level);   

            if(last_level != previous_level)  flag =1;
            else flag =0;

            previous_level = atoi(pm10Grade);
            
            Serial.printf("After\nlast_level : %d\n",last_level);
            Serial.printf("previous_level : %d\n",previous_level);   

            Serial.printf("flag : %d\n",flag);
        
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
    }

    
    if(flag == 1)
    { 
      level = 7000000000*last_level;
      handle_purifier(s_udp, s_purifier_ip, s_token,level);
      
      if(millis()-lastflagMs> DELAY_MS2)
      {
        lastflagMs =millis(); 
        count++;
      }
      
      if(count == 22)
      {
        if(last_level > 2)
        {
            analogWrite(D0,1000);  
            digitalWrite(D1,HIGH);
            digitalWrite(D2,LOW);
            delay(1000);
            analogWrite(D0,0);  
        }
        
        else if(last_level <= 2)
        {
            analogWrite(D0,1000);  
            digitalWrite(D1,LOW);
            digitalWrite(D2,HIGH);
            delay(1000);
            analogWrite(D0,0);  
        }
      
        count=0;
        flag=0;
      }     
    }
}
