#include <Arduino.h>
#include <Wire.h>

// BME280 default I2C address
#define BME280_ADDRESS 0x77

// Registers for BME280 sensor (Calibration)
#define BME280_DIG_T1_LSB_REG			0x88
#define BME280_DIG_T1_MSB_REG			0x89
#define BME280_DIG_T2_LSB_REG			0x8A
#define BME280_DIG_T2_MSB_REG			0x8B
#define BME280_DIG_T3_LSB_REG			0x8C
#define BME280_DIG_T3_MSB_REG			0x8D

//Reading all compensation data, range 0x88:A1, 0xE1:E7
uint16_t dig_T1;
int16_t dig_T2;
int16_t	dig_T3;

// Function prototypes
void writeRegister(uint8_t reg, uint8_t value);
uint8_t readRegister(uint8_t reg);
void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t length);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // **Start Condition and Addressing**
  Serial.println("Scanning for I2C devices...");
  Wire.beginTransmission(BME280_ADDRESS); // Start condition + addressing (write)
  uint8_t error = Wire.endTransmission(); // Acknowledgment and stop condition

  if (error == 0) {
    Serial.println("BME280 sensor found!");
  } else {
    Serial.println("BME280 sensor not found.");
    while (1); // Halt execution
  }

  // **Data Transfer**: Write configuration to the sensor
  writeRegister(0xF2, 0x01); // Humidity oversampling x1
  writeRegister(0xF4, 0x27); // Temperature and pressure oversampling x1, normal mode

  //Reading all compensation data, range 0x88:A1, 0xE1:E7
	dig_T1 = ((uint16_t)((readRegister(BME280_DIG_T1_MSB_REG) << 8) + readRegister(BME280_DIG_T1_LSB_REG)));
	dig_T2 = ((int16_t)((readRegister(BME280_DIG_T2_MSB_REG) << 8) + readRegister(BME280_DIG_T2_LSB_REG)));
	dig_T3 = ((int16_t)((readRegister(BME280_DIG_T3_MSB_REG) << 8) + readRegister(BME280_DIG_T3_LSB_REG)));
}

void loop() {
  // Data Transfer: Read temperature data
  uint8_t data[3]; // Temperature data is 3 bytes long
  readRegisters(0xFA, data, 3); // Read temperature registers

  // Convert the raw data to temperature
  // From SparkFun_BME280_Arduino_Library
  int32_t adc_T = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4);
	//By datasheet, calibrate
	int64_t var1, var2;

	var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *
	((int32_t)dig_T3)) >> 14;
	int64_t t_fine = var1 + var2;
	float output = (t_fine * 5 + 128) >> 8;

	output = output / 100;

  float temperature = output;
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  delay(2000); // Wait for 2 seconds before next reading
}

void writeRegister(uint8_t reg, uint8_t value) {
  // **Start Condition and Addressing**
  Wire.beginTransmission(BME280_ADDRESS); // Start condition + addressing (write)
  Wire.write(reg);                        // **Data Transfer**: Register address
  Wire.write(value);                      // **Data Transfer**: Register value
  Wire.endTransmission();                 // **Acknowledgment and Stop Condition**
}

uint8_t readRegister(uint8_t reg) {
  uint8_t value;
  // **Start Condition and Addressing** (write mode)
  Wire.beginTransmission(BME280_ADDRESS);
  Wire.write(reg); // **Data Transfer**: Register address
  Wire.endTransmission(false); // **Repeated Start Condition**

  // **Start Condition and Addressing** (read mode)
  Wire.requestFrom(BME280_ADDRESS, (uint8_t)1); // **Data Transfer**: Read data
  value = Wire.read();
  return value; // **Acknowledgment and Stop Condition** handled by Wire library
}

void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t length) {
  // **Start Condition and Addressing** (write mode)
  Wire.beginTransmission(BME280_ADDRESS); // Start condition + Device Address
  Wire.write(reg); //  Register address
  uint8_t error = Wire.endTransmission(false); // Stop condition + ACK from slave

  // Check if slave did not acknowledge
  if (error != 0) {
    Serial.print("Error reading from register: ");
    Serial.println(reg);
    return;
  }

  Wire.requestFrom(BME280_ADDRESS, length); // Read Device address + Data
  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = Wire.read();
  }
  // Acknowledgment and Stop Condition handled by Wire library
}
