#include "esp_camera.h"
#include <WiFi.h>
#include <DHT.h>

//#include <AsyncMqttClient.h>
#include <PubSubClient.h>

#define MQTT_HOST IPAddress(192, 168, 1, 2)
#define MQTT_PORT 1883
// Temperature MQTT Topics
#define MQTT_PUB_DHT "esp32/sensor/bot"

// Initializing ESP client
WiFiClient espClient;
// Initializing ESP as MQTT Client
PubSubClient mqttClient(espClient);


unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 4000;        // Interval at which to publish sensor readings

float temp;
float hum;

#define GAS_PRESENT 1
#define DHTTYPE 11
#define DHTPIN  12

extern int IN1 =  2; 
extern int IN2 = 14; 
extern int IN3 = 15; 
extern int IN4 = 13; 
extern int flash =  4; 


DHT dht(DHTPIN, DHTTYPE);
char * msg;

#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
#define WIFI_SSID       "Elife Networks 2.4G"
#define WIFI_PASSWORD   "elife@123"

void startCameraServer();

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");

    Serial.println(WIFI_SSID);
    WiFi.setSleep(false);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  
  int c=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); //
    Serial.print(c);
    c=c+1;
    if(c>10){
        ESP.restart(); //restart ESP after 10 seconds
    }
  }


  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  while (!mqttClient.connected()) {
        if(WiFi.status() != WL_CONNECTED){
          connectToWifi();
        }

        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect("ESP32_client3")) { // Change the name of client here if multiple ESP32 are connected
          //attempt successful
          Serial.println("connected");          
        } 
        else {
          //attempt not successful
          Serial.print("failed, rc=");
          Serial.print(mqttClient.state());
          Serial.println(" trying again in 2 seconds");
          delay(2000);
        }
  } 
}


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  dht.begin();
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(flash, OUTPUT);


  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); 
  digitalWrite(IN4, LOW);
  digitalWrite(flash, LOW);

  camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.frame_size = FRAMESIZE_UXGA;
    config.pixel_format = PIXFORMAT_JPEG;  // for streaming
    //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (config.pixel_format == PIXFORMAT_JPEG) {
      if (psramFound()) {
        config.jpeg_quality = 10;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
      } else {
        // Limit the frame size when PSRAM is not available
        config.frame_size = FRAMESIZE_SVGA;
        config.fb_location = CAMERA_FB_IN_DRAM;
      }
    } else {
      // Best option for face detection/recognition
      config.frame_size = FRAMESIZE_240X240;
    }

  
  // Post setup config
    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);

  connectToMqtt();

  delay(1500);

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  
  
  delay(1000);
  if (!mqttClient.connected()) {
     connectToMqtt();
  }

  mqttClient.loop();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
      // Save the last time a new reading was published
      previousMillis = currentMillis;
      // New DHT sensor readings
      hum = dht.readHumidity();
      // Read temperature as Celsius (the default)
      temp = dht.readTemperature();

      if (isnan(temp) || isnan(hum)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
    
    // Publish an MQTT message on topic esp32/node/bot
      uint16_t packetIdPub1 = mqttClient.publish(MQTT_PUB_DHT, String(temp).c_str());                            
      Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", MQTT_PUB_DHT, packetIdPub1);
      Serial.printf("Message: %.2f \n", temp);

      uint16_t packetIdPub2 = mqttClient.publish(MQTT_PUB_DHT, String(hum).c_str());                            
      Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", MQTT_PUB_DHT, packetIdPub2);
      Serial.printf("Message: %.2f \n", hum);
    }
  }

