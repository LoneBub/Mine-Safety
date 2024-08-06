#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <string>

#define BUTTON_PIN 19
#define DHTTYPE 11
#define DHTPIN  14
#define MQPIN   27
#define x 33 // x_out pin of Accelerometer
#define y 25 // y_out pin of Accelerometer
#define z 26 // z_out pin of Accelerometer
#define BUTTON_PIN 19

long lastMsg = 0;

const char* ssid = "Mine-Net";
const char* password = "";
const char* mqtt_server = ("192.168.4.102");
const int mqttPort = 1883;
const int buzz = 12;
int xsample=0;
int ysample=0;
int zsample=0;
// Initializing ESP client
WiFiClient espClient;
// Initializing ESP as MQTT Client
PubSubClient client(espClient);

long now = 0;

#define ledPin 2

void blink_led(unsigned int times, unsigned int duration){
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
  
  IPAddress local_ip(192,168,4,103);
  IPAddress gateway(192,168,4,2);
  IPAddress subnet(255,255,255,0);

  if(!WiFi.config(local_ip)){
    Serial.println("STA Failed to configure");
  };

  WiFi.begin(ssid, password);

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
        
        //now attemt to connect to MQTT server
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32_client4")) { // Change the name of client here if multiple ESP32 are connected
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

// initialize dht object
DHT dht(DHTPIN, DHTTYPE);

void setup(){

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);

  dht.begin();

  WiFi.mode(WIFI_STA);
  setup_wifi();
    
  client.setServer(mqtt_server,mqttPort);//1883 is the default port for MQTT server
  client.setCallback(callback);
}
 
void loop(){
 if (!client.connected()) {
    connect_mqttServer();
  }
  client.loop();

// STRING
  char * sig2 = "SOS";
  char *message;
  int value1=analogRead(x); // reading x out
  int value2=analogRead(y); //reading y out
  int value3=analogRead(z); //reading z out
  int buttonState = digitalRead(BUTTON_PIN); 
  int xValue=xsample-value1; // finding change in x
  int yValue=ysample-value2; // finding change in y
  int zValue=zsample-value3; // finding change in z
  int randnum = random(200,400);
  int randnum1 = random(300,350);
  int gas_detected = digitalRead(MQPIN);
  


  Serial.println(gas_detected);
  Serial.print(xValue);
  Serial.print("\t");
  Serial.print(randnum);
  Serial.print("\t");
  Serial.print(randnum1);
  Serial.print("\t");
  Serial.println();
  Serial.println(buttonState);
  Serial.print("\n");
  delay(1000);

// DHT11 
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

// MQ
  
  if (gas_detected == LOW && buttonState == HIGH){
    message = "GAS Present";
    tone(buzz,1000);
    delay(1000);
  } else {
    message = "GAS Not Present";
    noTone(buzz);
  }

  if (xValue > 2300 && buttonState == LOW){
    tone(buzz,1000);
    delay(1000);
  } else {
    noTone(buzz);
  }


  long now = millis();
  if (now - lastMsg > 4000) {
    lastMsg = now;

    char temp_buffer[10];
    dtostrf(t,1,2,temp_buffer);

    char humidity_buffer[10];
    dtostrf(h,1,2,humidity_buffer);

    char gas_buffer[50];
    strcpy(gas_buffer, message);

    char buffer4[10];  
    itoa(xValue,buffer4,10);

    char buffer5[10];  
    itoa(randnum,buffer5,10);

    char buffer6[10];  
    itoa(randnum1,buffer6,10);

    char buffer8[50]; 
    strcpy(buffer8, sig2);
    
    client.publish("esp32/sensor/helmet", temp_buffer);
    client.publish("esp32/sensor/helmet", humidity_buffer);
    client.publish("esp32/sensor/helmet", gas_buffer);
    client.publish("esp32/sensor/helmet", buffer4);
    client.publish("esp32/sensor/helmet", buffer5);
    client.publish("esp32/sensor/helmet", buffer6);
    client.publish("esp32/sensor/helmet", buffer8);
  }

}
