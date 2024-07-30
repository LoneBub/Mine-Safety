#define CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"
#include "camera_pins.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
//#include <DHT_U.h>

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "tp-link";
const char *password = "PITC'2023";

// Set your Static IP address
IPAddress local_IP(192, 168, 0, 184);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

// Define sensor type and pin
#define DHTPIN 1      // D15 
#define DHTTYPE DHT11 // DHT11 sensor
#define MQ2_PIN 3 //D13
#define LED_BUILTIN 33

DHT dht(DHTPIN, DHTTYPE);

// GPIO Settings
int gpLb    =  2;  // Left 1
int gpLf    = 14;  // Left 2
int gpRb    = 15;  // Right 1
int gpRf    = 13;  // Right 2
int gpLed   =  4; // Light

char* mq_value;
String WiFiAddr ="";

// MQTT broker address
const char* mqtt_server = ("192.168.4.102");
const int mqttPort = 1883;

// Initializing ESP client
WiFiClient espClient;
// Initializing ESP as MQTT Client
PubSubClient client(espClient);

long lastMsg = 0;

void blink_led(unsigned int times, unsigned int duration)
{
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(duration);
    digitalWrite(LED_BUILTIN, LOW); 
    delay(200);
  }
}

void startCameraServer();
void setupLedFlash(int pin);

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
    
          blink_led(3,200); //blink LED three times (200ms on duration) to show that MQTT server connection attempt failed
          // Wait 2 seconds before retrying
          delay(2000);
        }
  }
  
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Check if a message is received on the topic "rpi/broadcast"
  if (String(topic) == "rpi/broadcast") {
      if(messageTemp == "10"){
        Serial.println("Action: blink LED");
        blink_led(1,1250); //blink LED once (for 1250ms ON time)
      }
  }
}

void setup_wifi() {
  delay(50);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  if(!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);

  // reset esp after 10 seconds if it is not connected to any wifi
  int c=0;
  while (WiFi.status() != WL_CONNECTED) {
    blink_led(2,200); //blink LED twice (for 200ms ON time) to indicate that wifi not connected
    delay(1000); //
    Serial.print(".");
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

void setup() {
  Serial.begin(115200);

  Serial.setDebugOutput(true);
  Serial.println();

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

/////////////////// MOTOR DRIVER ENDS  /////////////////////////////


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
  config.xclk_freq_hz = 10000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; 
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


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  Serial.print("Camera PID : ");
  Serial.print(s->id.PID);
  
  s->set_framesize(s, FRAMESIZE_240X240);
  
  setup_wifi();

//////////////////////////////////    WiFi INITIALIZATION COMPLETE    //////////////////////

  client.setServer(mqtt_server,mqttPort);//1883 is the default port for MQTT server
  client.setCallback(callback);

  pinMode(MQ2_PIN, INPUT);


  Serial.println("Warming up the MQ2 sensor");
  //delay(20000);  // wait for the MQ2 to warm up

////////////////////////  SENSOR INITIALIZATION COMPLETE    ////////////////////////////////

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  WiFiAddr = WiFi.localIP().toString();
  Serial.println("' to connect");
}

void loop() {
  delay(10000);
    if (!client.connected()) {
    connect_mqttServer();
  }
  client.loop();

// MQ-2 sensor reading
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
  float dht_humidity = dht.readHumidity();
  float dht_temp = dht.readTemperature();

  delay(1000);
 
  // Check if any reads failed and exit early (to try again).
  if (isnan(dht_humidity) || isnan(dht_temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  delay(100);
  
  long now = millis();
  if (now - lastMsg > 4000) {
    lastMsg = now;

    char buffer1[10]; 
    dtostrf(dht_temp, 1, 2, buffer1);
    
    char buffer2[10];  
    dtostrf(dht_humidity,1,2, buffer2);

    char buffer3[500]; 
    memcpy(buffer3, mq_value,(size_t)sizeof(mq_value)* strlen(mq_value));

    client.publish("esp32/sensor/bot", buffer1);
    client.publish("esp32/sensor/bot", buffer2);
    client.publish("esp32/sensor/bot", buffer3);
  }
}
