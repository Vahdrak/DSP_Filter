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

// Variables para el filtro de media móvil
const int filterSize = 10; // Tamaño de la ventana del filtro de media móvil
float AccXBuffer[filterSize], AccYBuffer[filterSize], AccZBuffer[filterSize];
float GyroXBuffer[filterSize], GyroYBuffer[filterSize], GyroZBuffer[filterSize];
int filterIndex = 0;
float AccXSum = 0, AccYSum = 0, AccZSum = 0;
float GyroXSum = 0, GyroYSum = 0, GyroZSum = 0;

void setup() {
  Wire.begin(D2, D1); // D2(GPIO4)=SDA / D1(GPIO5)=SCL
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);
  
  // Inicializar los buffers del filtro de media móvil
  for (int i = 0; i < filterSize; i++) {
    AccXBuffer[i] = 0;
    AccYBuffer[i] = 0;
    AccZBuffer[i] = 0;
    GyroXBuffer[i] = 0;
    GyroYBuffer[i] = 0;
    GyroZBuffer[i] = 0;
  }
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

  // Actualizar el filtro de media móvil para el acelerómetro
  AccXSum = AccXSum - AccXBuffer[filterIndex] + AccX;
  AccYSum = AccYSum - AccYBuffer[filterIndex] + AccY;
  AccZSum = AccZSum - AccZBuffer[filterIndex] + AccZ;

  AccXBuffer[filterIndex] = AccX;
  AccYBuffer[filterIndex] = AccY;
  AccZBuffer[filterIndex] = AccZ;

  float AccXFiltered = AccXSum / filterSize;
  float AccYFiltered = AccYSum / filterSize;
  float AccZFiltered = AccZSum / filterSize;

  // Actualizar el filtro de media móvil para el giroscopio
  GyroXSum = GyroXSum - GyroXBuffer[filterIndex] + GyroX;
  GyroYSum = GyroYSum - GyroYBuffer[filterIndex] + GyroY;
  GyroZSum = GyroZSum - GyroZBuffer[filterIndex] + GyroZ;

  GyroXBuffer[filterIndex] = GyroX;
  GyroYBuffer[filterIndex] = GyroY;
  GyroZBuffer[filterIndex] = GyroZ;

  float GyroXFiltered = GyroXSum / filterSize;
  float GyroYFiltered = GyroYSum / filterSize;
  float GyroZFiltered = GyroZSum / filterSize;

  // Incrementar el índice del filtro
  filterIndex = (filterIndex + 1) % filterSize;

  // Mostrar los valores filtrados en el Serial Plotter
  Serial.print("Filtered AccX:"); Serial.print(AccXFiltered); Serial.print(" ");
  Serial.print("Filtered AccY:"); Serial.print(AccYFiltered); Serial.print(" ");
  Serial.print("Filtered AccZ:"); Serial.println(AccZFiltered);

  Serial.print("Filtered GyroX:"); Serial.print(GyroXFiltered); Serial.print(" ");
  Serial.print("Filtered GyroY:"); Serial.print(GyroYFiltered); Serial.print(" ");
  Serial.print("Filtered GyroZ:"); Serial.println(GyroZFiltered);

  delay(10); // Esperar un breve tiempo antes de la próxima lectura
}
