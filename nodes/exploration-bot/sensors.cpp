#ifdef SENSORS

const char* mqtt_server = ("192.168.4.7");
const int mqttPort = 1883;


void connect_mqttServer() {
  // Loop until we're reconnected
  while (!client.connected()) {

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


void node_setup() {
}
  
void node_loop() {

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
  
  delay(1000);
  if (!client.connected()) {
    connect_mqttServer();
  }

  client.loop();
  
  long now = millis();
  if (now - lastMsg > 4000) {
    lastMsg = now;

    char buffer1[10]; 
    dtostrf(dht_temp, 1, 2, buffer1);
    
    char buffer2[10];  
    dtostrf(dht_humidity,1,2, buffer2);

    char buffer3[10];  
    itoa(mq2_val,buffer3,10);

    client.publish("esp32/sensor/bot", buffer1);
    client.publish("esp32/sensor/bot", buffer2);
    client.publish("esp32/sensor/bot", buffer3);
  }
 
}
  
#endif