#include <WiFi.h>

IPAddress local_IP(192,168,4,2);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

const char *soft_ap_ssid = "ESP32";
const char *soft_ap_password = "";

const char* wifi_network_ssid = "Modify";
const char* wifi_network_password =  "isuq5478";

void setup()
{
  WiFi.mode(WIFI_MODE_APSTA);

  Serial.begin(115200);

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  
  Serial.print("Setting modes ... ");
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  WiFi.begin(wifi_network_ssid, wifi_network_password);
  //WiFi.softAP(ssid);
  //WiFi.softAP(ssid, passphrase, channel, ssdi_hidden, max_connection)
}

void loop() {
  Serial.print("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());
 
  Serial.print("ESP32 IP on the WiFi network: "); 
  Serial.println(WiFi.localIP());
  delay(5000);  

}
