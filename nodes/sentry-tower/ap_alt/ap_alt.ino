#include <ESP8266WiFi.h>

#define NWSETTINGS 1

#define AP_SSID     "esp"
#define AP_PASSWD   ""

#define STA_SSID    ""
#define STA_PASSWD  ""

IPAddress staticIP(192, 168, 0, 100); // ESP32 static IP
IPAddress gateway(192, 168, 0, 1);    // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask
IPAddress primaryDNS(192, 168, 0, 1); // Primary DNS (optional)
IPAddress secondaryDNS(0, 0, 0, 0);   // Secondary DNS (optional)

void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(STA_SSID);

  //WiFi.mode(STA);
  WiFi.begin(STA_SSID, STA_PASSWD);
  //WiFi.softAP(AP_SSID, AP_PASSWD);

  if(!WiFi.config(staticIP, gateway, subnet, primaryDNS)){
    Serial.println("Failed to configure Static IP");
  } else {
    Serial.println("Static IP configured!");
  }
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected..!");

#if NWSETTINGS
  Serial.print("Current ESP IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway (router) IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet Mask: " );
  Serial.println(WiFi.subnetMask());
  Serial.print("Primary DNS: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("Secondary DNS: ");
  Serial.println(WiFi.dnsIP(1));
#endif
}


void loop() {
}
