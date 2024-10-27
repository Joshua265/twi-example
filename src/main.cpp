#include <Arduino.h>
#include <Wire.h>
#include <SparkFunBME280.h>

BME280 mySensor;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for Serial to initialize (for Leonardo boards)

  Serial.println("Initializing BME280 sensor...");

  // Initialize I2C communication
  Wire.begin();

  // Initialize the BME280 sensor
  if (mySensor.beginI2C() == false) {
    Serial.println("Sensor initialization failed. Check wiring or sensor address.");
    while (1);
  }

  Serial.println("BME280 sensor initialized successfully!");
}

void loop() {
  // Read temperature in Celsius
  float temperature = mySensor.readTempC();

  // Read pressure in Pascals and convert to hPa
  float pressure = mySensor.readFloatPressure() / 100.0F;

  // Read humidity percentage
  float humidity = mySensor.readFloatHumidity();

  // Display the results
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.println("---------------------------");

  delay(2000); // Wait for 2 seconds before next reading
}
