#include <Arduino.h>
#include <Wire.h>

// Dirección I2C de la IMU
#define MPU 0x68

// Ratios de conversión
#define A_R 16384.0 // 32768/2 para ±2g
#define G_R 131.0   // 32768/250 para ±250°/s

// MPU-6050 da los valores en enteros de 16 bits
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;

// Aceleración convertida (g)
float AccX, AccY, AccZ;

// Giroscopio convertido (°/s)
float GyroX, GyroY, GyroZ;

void setup() {
  Wire.begin(D2, D1); // D2(GPIO4)=SDA / D1(GPIO5)=SCL
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);
}

void loop() {
  // Leer los valores del Acelerómetro de la IMU
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Pedir el registro 0x3B - corresponde al AcX
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // A partir del 0x3B, se piden 6 registros
  AcX = Wire.read() << 8 | Wire.read(); // Cada valor ocupa 2 registros
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // Leer los valores del Giroscopio de la IMU
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Pedir el registro 0x43 - corresponde al GyX
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // A partir del 0x43, se piden 6 registros
  GyX = Wire.read() << 8 | Wire.read(); // Cada valor ocupa 2 registros
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // Convertir las lecturas del acelerómetro a g
  AccX = AcX / A_R;
  AccY = AcY / A_R;
  AccZ = AcZ / A_R;

  // Convertir las lecturas del giroscopio a °/s
  GyroX = GyX / G_R;
  GyroY = GyY / G_R;
  GyroZ = GyZ / G_R;

  // Mostrar los valores convertidos en el formato esperado por el Serial Plotter
  // Nota: El Serial Plotter espera que cada valor sea separado por espacios o comas y cada conjunto de valores en una nueva línea
  
  Serial.print("AccX:"); Serial.print(AccX); Serial.print(" ");
  Serial.print("AccY:"); Serial.print(AccY); Serial.print(" ");
  Serial.print("AccZ:"); Serial.println(AccZ);

  Serial.print("GyroX:"); Serial.print(GyroX); Serial.print(" ");
  Serial.print("GyroY:"); Serial.print(GyroY); Serial.print(" ");
  Serial.print("GyroZ:"); Serial.println(GyroZ);


}
