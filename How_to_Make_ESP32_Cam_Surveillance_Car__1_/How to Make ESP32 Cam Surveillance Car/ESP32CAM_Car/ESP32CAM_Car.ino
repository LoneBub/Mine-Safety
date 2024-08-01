#include "esp_camera.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <string.h>

#define CAMERA_MODEL_AI_THINKER

const char* ssid = "Galaxy S21 FE 5G 2F0D";   //Enter SSID WIFI Name
const char* password = "kkiz0275";   //Enter WIFI Password

//const char* ssid = "Mine-Net";   //Enter SSID WIFI Name
//const char* password = "";   //Enter WIFI Password


// Set your Static IP address
IPAddress local_IP(192, 168, 4, 149);
// Set your Gateway IP address
IPAddress gateway(192, 168, 4, 2);
IPAddress subnet(255, 255, 255, 0);

// MQTT broker address
const char* mqtt_server = ("192.168.73.86");
const int mqttPort = 1883;

// Initializing ESP client
WiFiClient espClient;
// Initializing ESP as MQTT Client
PubSubClient client(espClient);

#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#else
#error "Camera model not selected"
#endif

// GPIO Setting
extern int gpLb =  2; // Left 1
extern int gpLf = 14; // Left 2
extern int gpRb = 15; // Right 1
extern int gpRf = 13; // Right 2
extern int gpLed =  4; // Light
extern String WiFiAddr ="";

void startCameraServer();

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
 // if(!WiFi.config(local_IP, gateway, subnet)) {
 //  Serial.println("STA Failed to configure");
  //}

  WiFi.begin(ssid, password);

  // reset esp after 10 seconds if it is not connected to any wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); //
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void connect_mqttServer() {
  // Loop until we're reconnected
  while (!client.connected()) {
        //now attemt to connect to MQTT server
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32_client3")) { // Change the name of client here if multiple ESP32 are connected
          //attempt successful
          Serial.println("connected");
          // Subscribe to topics here
          client.subscribe("rpi/broadcast");          
        }
        else {
          //attempt not successful
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" trying again in 2 seconds");
    
          delay(2000);
        }
  }
  
}

#define DHTPIN 12        
#define DHTTYPE DHT11   
#define MQ2_PIN 0     
DHT dht(DHTPIN, DHTTYPE);

char* mq_value = "";


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  dht.begin();


  pinMode(gpLb, OUTPUT); //Left Backward
  pinMode(gpLf, OUTPUT); //Left Forward
  pinMode(gpRb, OUTPUT); //Right Forward
  pinMode(gpRf, OUTPUT); //Right Backward
  pinMode(gpLed, OUTPUT); //Light

  //initialize
  digitalWrite(gpLb, LOW);
  digitalWrite(gpLf, LOW);
  digitalWrite(gpRb, LOW);
  digitalWrite(gpRf, LOW);
  digitalWrite(gpLed, LOW);

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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //drop down frame size for higher initial frame rate
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QQVGA);

  setup_wifi();
  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  WiFiAddr = WiFi.localIP().toString();
  Serial.println("' to connect");

 // client.setServer(mqtt_server,mqttPort);//1883 is the default port for MQTT server

  pinMode(MQ2_PIN, INPUT);
  Serial.println("Warming up the MQ2 sensor");
}

void loop() {
  
  delay(10000);
/*
  if (WiFi.status() != WL_CONNECTED){
    setup_wifi();
  }
 if (!client.connected()) {
    connect_mqttServer();
  }

// MQ2
  int mq2_val = digitalRead(MQ2_PIN);
  if (mq2_val == HIGH){
    Serial.println("The gas is NOT present");
    mq_value = "GAS is NOT Present";
  }
  else{
    Serial.println("The gas is present");
    mq_value = "GAS is Present";
  }

// DHT sensor reading
  float dht_humidity  =  dht.readHumidity() ; 
  float dht_temp      =  dht.readTemperature()   ;     
  Serial.println(dht_humidity);
  Serial.println(dht_temp);

  if (isnan(dht_humidity) || isnan(dht_temp)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  client.loop();

  long lastMsg = 0;
  long now = millis();
  if (now - lastMsg > 4000) {
    lastMsg = now;

    char buffer1[10]; 
    dtostrf(dht_temp, 1, 2, buffer1);
    
    char buffer2[10];  
    dtostrf(dht_humidity,1,2, buffer2);

    char buffer3[500]; 
    strcpy(buffer3, mq_value);

    char buffer4[500]; 
    strcpy(buffer4, mq_value);

    client.publish("esp32/sensor/bot", buffer1);
    client.publish("esp32/sensor/bot", buffer2);
    client.publish("esp32/sensor/bot", buffer3);
  }
  */
}
