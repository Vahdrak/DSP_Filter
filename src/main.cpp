#include <Arduino.h>
#include <Wire.h>

#define MPU_ADDR 0x68 // I2C address of the MPU-6050

//Ratios de conversion
#define A_R 16384.0 // 32768/2
#define G_R 131.0 // 32768/250
 
//Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779

// Variables for storing sensor data
int16_t accelerometer_x;
int16_t accelerometer_y;
int16_t accelerometer_z;
int16_t temperature;
int16_t gyro_x;
int16_t gyro_y;
int16_t gyro_z;

void setup() {
  Serial.begin(115200);
  Wire.begin(4,5); // D2(GPIO4)=SDA / D1(GPIO5)=SCL
  // Wake up the MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop() {
  // Read accelerometer, gyroscope, and temperature data from MPU-6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Starting register for accelerometer data
  Wire.endTransmission(false); // Send restart condition
  Wire.requestFrom(MPU_ADDR, 14, true); // Request 14 bytes of data

  // Read accelerometer data
  accelerometer_x = Wire.read() << 8 | Wire.read();
  accelerometer_y = Wire.read() << 8 | Wire.read();
  accelerometer_z = Wire.read() << 8 | Wire.read();

  // Read temperature data
  temperature = Wire.read() << 8 | Wire.read();

  // Read gyroscope data
  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();

  // Print sensor data to serial monitor
  //Serial.print(" | aX = ");
  Serial.print(">gyro_x:");
  Serial.println(gyro_x);
  //Serial.print(" | aY = ");
  Serial.print(">gyro_y:");
  Serial.println(gyro_y);
  //Serial.print(" | aZ = ");
  Serial.print(">gyro_z:");
  Serial.println(gyro_z);


  Serial.println();

  delay(100); // Delay between readings
}