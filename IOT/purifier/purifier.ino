#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <string>
#include "purifier.h"
                            
const std::string s_token = "9fa924eb745032c50274714b657b928d";
int level;
WiFiUDP s_udp;

IPAddress s_purifier_ip(192,168,50,11);

void setup()
{
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

void loop()
{
   // level = 1;
   // handle_purifier(s_udp, s_purifier_ip, s_token,level);
    level = 14;
    handle_purifier(s_udp, s_purifier_ip, s_token,level);
}
