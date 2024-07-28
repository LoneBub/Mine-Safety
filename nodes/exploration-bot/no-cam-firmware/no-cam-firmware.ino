#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "OnePlus Ce3";
const char* password = "manjutomar";

// Set GPIO pins for motor control
#define IN1 12
#define IN2 13
#define IN3 14
#define IN4 15

WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize GPIO pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password); 
  
  delay(1000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start server and define routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  
  server.begin();
  Serial.println("HTTP server started at : ");
  Serial.print(WiFi.localIP());

}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html>\
                  <head>\
                    <title>ESP32 Motor Control</title>\
                    <style>\
                      button {\
                        display: block;\
                        width: 100px;\
                        height: 50px;\
                        margin: 10px;\
                      }\
                    </style>\
                  </head>\
                  <body>\
                    <h1>Motor Control</h1>\
                    <button onclick=\"location.href='/forward'\">Forward</button>\
                    <button onclick=\"location.href='/backward'\">Backward</button>\
                    <button onclick=\"location.href='/left'\">Left</button>\
                    <button onclick=\"location.href='/right'\">Right</button>\
                    <button onclick=\"location.href='/stop'\">Stop</button>\
                  </body>\
                </html>";
  server.send(200, "text/html", html);
}

void handleForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  server.send(200, "text/html", "Moving forward<br><a href='/'>Go Back</a>");
}

void handleBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  server.send(200, "text/html", "Moving backward<br><a href='/'>Go Back</a>");
}

void handleLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  server.send(200, "text/html", "Turning left<br><a href='/'>Go Back</a>");
}

void handleRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  server.send(200, "text/html", "Turning right<br><a href='/'>Go Back</a>");
}

void handleStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  server.send(200, "text/html", "Stopped<br><a href='/'>Go Back</a>");
}
