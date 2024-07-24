#include <WiFi.h>
#include <PubSubClient.h>
#include "Adafruit_Sensor.h"
#include <DHT.h>
#include <DHT_U.h>

// Define DHT sensor type and pin
#define DHTPIN 14      // D15 pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT11 sensor
#define MQ2_PIN A14
#define pushButton 22

#define samples 50
#define x_out  33 /* connect x_out of ADXL to 33 of ESP board */
#define y_out  26 /* connect y_out of ADXL to 26 of ESP board */
#define z_out  1 /* connect z_out of ADXL to 14 of ESP board */
// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

int xsample=0;
int ysample=0;
int zsample=0;
const char* sig = "SOS";
long start;

int maxVal = 850; // max change limit
int minVal = -850; // min change limit
int warningButton;

// Replace the SSID/Password details as per your wifi router
const char* ssid = "MINE-AP";
const char* password = "";

// Replace your MQ vTT Broker IP address here:
const char* mqtt_server = ("192.168.177.86");
const int mqttPort = 1883;
const int buzz = 5;

// Initializing ESP client
WiFiClient espClient;
// Initializing ESP as MQTT Client
PubSubClient client(espClient);

long lastMsg = 0;

#define ledPin 2

void blink_led(unsigned int times, unsigned int duration)
{
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(duration);
    digitalWrite(ledPin, LOW); 
    delay(200);
  }
}

                                                                                                               

void setup_wifi() {
  delay(50);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
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

void connect_mqttServer() {
  // Loop until we're reconnected
  while (!client.connected()) {
        //first check if connected to wifi
        if(WiFi.status() != WL_CONNECTED){
          //if not connected, then first connect to wifi
          setup_wifi();
        }

        //now attemt to connect to MQTT server
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32_client1")) { // Change the name of client here if multiple ESP32 are connected
          //attempt successful
          Serial.println("connected");
          // Subscribe to topics here
          client.subscribe("rpi/broadcast");
          //client.subscribe("rpi/xyz"); //subscribe more topics here
          
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

//this function will be executed whenever there is data available on subscribed topics
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

  //Similarly add more if statements to check for other subscribed topics 
}

void setup(void) {
  pinMode(ledPin, OUTPUT);
  pinMode(buzz,OUTPUT); // buzzer
  pinMode(pushButton,INPUT_PULLUP); // PUSH BUTTON
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);

  dht.begin();

  setup_wifi();
  
  client.setServer(mqtt_server,mqttPort);//1883 is the default port for MQTT server
  client.setCallback(callback);

  pinMode(MQ2_PIN, INPUT);


  Serial.println("Warming up the MQ2 sensor");
  //delay(20000);  // wait for the MQ2 to warm up

  for(int i=0;i<samples;i++) // taking samples for calibration
  {
    xsample+=analogRead(x_out);
    ysample+=analogRead(y_out);
    zsample+=analogRead(z_out);
  }
 
  xsample/=samples; // taking avg for x
  ysample/=samples; // taking avg for y
  zsample/=samples; // taking avg for z
 
  delay(3000);
}


void loop() {
  
  delay(1000);
  if (!client.connected()) {
    connect_mqttServer();
  }

  client.loop();

  delay(2000);

// MQ-2 sensor reading
  int mq2_val = digitalRead(MQ2_PIN);

  if (mq2_val == HIGH){
    Serial.println("The gas is present");
  }
  else{
    Serial.println("The gas is NOT present");
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
  int value1 , value2 , value3; 
  int fall_flag = 0;

  value1 = analogRead(x_out); /* Digital value of voltage on x_out pin */ 
  value2 = analogRead(y_out); /* Digital value of voltage on y_out pin */ 
  value3 = analogRead(z_out); /* Digital value of voltage on z_out pin */ 
 
  int xValue=xsample-value1; // finding change in x
  int yValue=ysample-value2; // finding change in y
  int zValue=zsample-value3; // finding change in z

  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  int gasState = digitalRead(MQ2_PIN);
  warningButton = digitalRead(pushButton);

  
  if(gasState == LOW) {
    tone(buzz, 1000);  // 1000 Hz frequency
    //Send MQTT info for Gas presence
  }
   
  /* yaha tak humne x y aur z ki deviation value nikal li h , ab agar 1000 ya -1000 cross karega toh alert karna 
  h ki worker gir gaya h 
  */

  /* yaha pe alert karna h , LED jalani h aur Buzzer on karna h , reset button ki functionalty banani h */
  if(xValue < minVal || xValue > maxVal || yValue < minVal || yValue > maxVal || zValue < minVal || zValue > maxVal || gasState != HIGH){
    fall_flag = 1;
    warningButton = 1;
    while(fall_flag == 1){
      tone(buzz, 1000);  // 1000 Hz frequency
      start = millis();  // starts a timer , counts in milli seconds

      if((fall_flag == 1) && (warningButton == 0) && (start < 10000)){
        noTone(buzz);
        fall_flag = 0;
      }

      else if((fall_flag == 1) && (warningButton == 1) && (start >= 10000)){
        tone(buzz , 1000);
        fall_flag = 0;
        client.publish("esp32/sensor/helmet", sig);//Code here to send it to MQTT Dashboard
      }
      break;
    }
  }
 
  delay(100);
  
  
  
  long now = millis();
  if (now - lastMsg > 4000) {
    lastMsg = now;

    char buffer1[10]; 
    dtostrf(dht_temp, 1, 2, buffer1);
    
    char buffer2[10];  
    dtostrf(dht_humidity,1,2, buffer2);

    char buffer3[10];  
    itoa(mq2_val,buffer3,10);

    char buffer4[10];  
    itoa(xValue,buffer4,10);

    char buffer5[10];  
    itoa(yValue,buffer5,10);

    char buffer6[10];  
    itoa(zValue,buffer6,10);

    client.publish("esp32/sensor/helmet", buffer1);
    client.publish("esp32/sensor/helmet", buffer2);
    client.publish("esp32/sensor/helmet", buffer3);
    client.publish("esp32/sensor/helmet", buffer4);
    client.publish("esp32/sensor/helmet", buffer5);
    client.publish("esp32/sensor/helmet", buffer6);
  }
 
}
