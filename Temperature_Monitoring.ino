#define BLYNK_TEMPLATE_ID "TMPL6rlBsbgK2"
#define BLYNK_TEMPLATE_NAME "Relay Control"
#define BLYNK_AUTH_TOKEN "_h8LAM84uaeeexl1NEu4CU0knjpG5q0N"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "nice"; // Your WiFi network SSID
char pass[] = "cuadra123"; // Your WiFi network password

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

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns and 2 rows

void setup()
{

  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(THERMISTOR_PIN, INPUT); // Set thermistor pin as input
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Use BLYNK_AUTH_TOKEN instead of auth
}

void loop()
{
  Blynk.run();
  float temp = getTemperature(); // Read temperature from thermistor
  Blynk.virtualWrite(V5, temp); // Send temperature value to Blynk app

  
 if (temp >= 30.0) {
    Serial.println("Notification sent!");
     Blynk.logEvent("temperature__high");
  } else {
    Serial.println("Temperature is below 30°C. No notification needed.");
  }
  
  if (temp >= 30.0) {
    digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer
  }

  lcd.clear(); // Clear the LCD screen
  lcd.setCursor(0, 0); // Set cursor to the first column of the first row
  lcd.print("Temp: "); // Display "Temp: "
  lcd.print(temp); // Display temperature on the LCD
  lcd.print(" C"); // Display " C" for Celsius
  delay(1000); // Delay for 1 second before reading temperature again
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
  
  float steinhart = thermistorResistance / nominal_resistance;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= beta;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (nominal_temperature + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  float temperature = steinhart - 273.15;                         // convert absolute temp to C
  
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  return temperature;
}
