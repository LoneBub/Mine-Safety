#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <string>

// Define DHT sensor type and pin
#define DHTPIN A13      // D15 pin connected to the DHT sensor
#define DHTTYPE DHT11 // DHT11 sensor
#define MQ2_PIN A14 //D13

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Replace the SSID/Password details as per your wifi router
const char* ssid = "Mine-Net";
const char* password = "";

// Replace your MQ vTT Broker IP address here:
const char* mqtt_server = ("192.168.4.102");
const int mqttPort = 1883;

char* sig1;


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

void setup() {
  pinMode(ledPin, OUTPUT);
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
}


void loop() {

  delay(1000);
  if (!client.connected()) {
    connect_mqttServer();
  }

  client.loop();

  
// MQ-2 sensor reading
  int mq2_val = digitalRead(MQ2_PIN);

  if (mq2_val == HIGH){
    Serial.println("The gas is NOT present");
    sig1 = "GAS is NOT Present";
    Serial.println(sizeof(sig1));


  }
  else{
    Serial.println("The gas is present");
    sig1 = "GAS is Present";
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

    char buffer3[10];  
    itoa(mq2_val,buffer3,10);

    char buffer4[500]; 
    memcpy(buffer4, sig1,(size_t)sizeof(sig1)* strlen(sig1));

    client.publish("esp32/sensor/sentry", buffer1);
    client.publish("esp32/sensor/sentry", buffer2);
    client.publish("esp32/sensor/sentry", buffer3);
    client.publish("esp32/sensor/sentry", buffer4);
  }
 
}
