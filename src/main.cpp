#include <Arduino.h>
#include <Wire.h>

// Dirección I2C de la IMU
#define MPU 0x68

// Ratios de conversión
#define A_R 16384.0 // 32768/2 para ±2g

// MPU-6050 da los valores en enteros de 16 bits
int16_t AcX, AcY, AcZ;

// Aceleración convertida (g)
float AccX, AccY, AccZ;

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

  // Convertir las lecturas del acelerómetro a g
  AccX = AcX / A_R;
  AccY = AcY / A_R;
  AccZ = AcZ / A_R;

  // Enviar las lecturas del acelerómetro al Serial Plotter
  Serial.print(">AccX:");
  Serial.print(AccX); Serial.print(" ,");
  Serial.print(">AccY:");
  Serial.print(AccY); Serial.print(" ");
  Serial.print(">AccZ:");
  Serial.println(AccZ);

  delay(10); // Esperar un breve tiempo antes de la próxima lectura
}
