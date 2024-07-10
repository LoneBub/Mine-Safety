

// Define the analog pin connected to the MQ-2 sensor
#define MQ2_PIN A13

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);

  // Give some time for the serial monitor to open
  delay(1000);

  Serial.println("MQ-2 Smoke Sensor Test");
}

void loop() {
  // Read the analog value from the MQ-2 sensor
  int analogValue = analogRead(MQ2_PIN);

  // Print the analog value to the Serial Monitor
  Serial.print("MQ-2 Sensor Value: ");
  Serial.println(analogValue);

  // Wait 1 second before taking another reading
  delay(1000);
}
