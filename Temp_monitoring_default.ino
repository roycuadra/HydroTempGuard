// BLYNK AUTH HERE  

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>


const char* ssid = "YOUR_WIFI_SSID"; 
const char* password = "YOUR_WIFI_PASS";

#define THERMISTOR_PIN A0 // Analog pin to which thermistor is connected
#define Rref 10000.0 // Reference resistance value of the thermistor
#define nominal_resistance 10000.0 // Nominal resistance of the thermistor
#define nominal_temperature 25.0 // Nominal temperature of the thermistor in Celsius
#define beta 3950.0 // Beta value of the thermistor
#define steinhart_A 1.2087674e-03 // Steinhart-Hart equation coefficients
#define steinhart_B 2.2206477e-04
#define steinhart_C 8.3940264e-08
#define samplingrate 10 // Number of samples to average
#define BUZZER_PIN 5 // GPIO pin 5


void setup() {
    Serial.begin(115200);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(THERMISTOR_PIN, INPUT);
    Blynk.begin(BLYNK_AUTH_TOKEN , ssid, password);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}



void loop()
{
  Blynk.run();

  float temp = getTemperature(); 
  Blynk.virtualWrite(V5, temp);

  
 if (temp >= 30.0) {
    Serial.println("Notification sent!");
     Blynk.logEvent("temperature__warning");
  } else {
    Serial.println("Temperature is below 30°C. No notification needed.");
  }
  
  if (temp >= 30.0) {
    digitalWrite(BUZZER_PIN, HIGH); 
  } else {
    digitalWrite(BUZZER_PIN, LOW); 
  }
  delay(1000); 
}

float getTemperature()
{
  uint16_t samples = 0;
  digitalWrite(THERMISTOR_PIN, HIGH);
  
  for (uint8_t i = 0; i < samplingrate; i++)
  {
    samples += analogRead(THERMISTOR_PIN);
    delay(10);
  }
  
  digitalWrite(THERMISTOR_PIN, LOW);
  
  float average = samples / samplingrate;
  float thermistorResistance = Rref / (1023.0 / average - 1);
  
  float steinhart = thermistorResistance / nominal_resistance;    
  steinhart = log(steinhart);                 
  steinhart /= beta;                   
  steinhart += 1.0 / (nominal_temperature + 273.15); 
  steinhart = 1.0 / steinhart;                
  float temperature = steinhart - 273.15;                       
  
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  return temperature;
}

