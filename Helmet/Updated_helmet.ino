
/*
Isme hum ADXL 335 ko calibrate karenge taaki excessive movements hone pe wo alert generate kare
isme hum ek push button ka bhi use karenge , if someone falls, buzzer aur LED bajega , uspe reset dabane ka 10 sec time hoga
agar 10 second me usne reset nahi press kiya toh system me alert send kar dega
*/

//Remove Serial.Print lines and add wifi connectivity code

#include <DHT.h>
#define DHTPIN A13      // D15 pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT11 sensor
#define MQ2_PIN 2
#define pushButton 22


// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

int xsample=0;
int ysample=0;
int zsample=0;
long start;

 
/*Macros ADXL */
#define samples 50
int maxVal = 850; // max change limit
int minVal = -850; // min change limit

#define x_out  33 /* connect x_out of ADXL to 33 of ESP board */
#define y_out  26 /* connect y_out of ADXL to 26 of ESP board */
#define z_out  14 /* connect z_out of ADXL to 14 of ESP board */

// BUZZER 
const int buzz = 5;    // CONNECT buzzer 


// push button
int warningButton;


void setup(void){
  Serial.begin(115200);                 /*Set the baudrate to 115200*/
  dht.begin();
  pinMode(buzz,OUTPUT); // buzzer
  pinMode(MQ2_PIN, INPUT); // MQ2
  pinMode(pushButton,INPUT_PULLUP); // PUSH BUTTON

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
        //Code here to send it to MQTT Dashboard
      }
      break;
    }
  }
 
  delay(100);
}