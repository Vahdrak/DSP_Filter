#include <Arduino.h>
#include <Wire.h>
#include <math.h>

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

// Parámetro para el filtro complementario (ajustable)
float alpha = 0.95; // Controla la contribución del filtro de paso bajo

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

  // Filtrar las lecturas usando el filtro complementario
  static float filtered_AccX = AccX, filtered_AccY = AccY, filtered_AccZ = AccZ;
  static float filtered_GyroX = GyroX, filtered_GyroY = GyroY, filtered_GyroZ = GyroZ;

  filtered_AccX = alpha * filtered_AccX + (1 - alpha) * AccX;
  filtered_AccY = alpha * filtered_AccY + (1 - alpha) * AccY;
  filtered_AccZ = alpha * filtered_AccZ + (1 - alpha) * AccZ;

  filtered_GyroX = alpha * filtered_GyroX + (1 - alpha) * GyroX;
  filtered_GyroY = alpha * filtered_GyroY + (1 - alpha) * GyroY;
  filtered_GyroZ = alpha * filtered_GyroZ + (1 - alpha) * GyroZ;

  // Calcular magnitudes de vibración en cada eje
  float vibration_magnitude_AccX = abs(filtered_AccX);
  float vibration_magnitude_AccY = abs(filtered_AccY);
  float vibration_magnitude_AccZ = abs(filtered_AccZ);
  float vibration_magnitude_GyroX = abs(filtered_GyroX);
  float vibration_magnitude_GyroY = abs(filtered_GyroY);
  float vibration_magnitude_GyroZ = abs(filtered_GyroZ);

  // Mostrar las magnitudes de vibración junto con sus ángulos correspondientes
  float angle_AccX = atan2(filtered_AccY, filtered_AccZ) * 180.0 / M_PI;
  float angle_AccY = atan2(filtered_AccX, filtered_AccZ) * 180.0 / M_PI;
  float angle_AccZ = atan2(filtered_AccX, filtered_AccY) * 180.0 / M_PI;
  float angle_GyroX = filtered_GyroX;
  float angle_GyroY = filtered_GyroY;
  float angle_GyroZ = filtered_GyroZ;

  Serial.println("Acelerómetro:");
  Serial.print("X: ");
  Serial.print(vibration_magnitude_AccX);
  Serial.print(" en ángulo: ");
  Serial.println(angle_AccX);
  
  Serial.print("Y: ");
  Serial.print(vibration_magnitude_AccY);
  Serial.print(" en ángulo: ");
  Serial.println(angle_AccY);
  
  Serial.print("Z: ");
  Serial.print(vibration_magnitude_AccZ);
  Serial.print(" en ángulo: ");
  Serial.println(angle_AccZ);

  Serial.println("Giroscopio:");
  Serial.print("X: ");
  Serial.print(vibration_magnitude_GyroX);
  Serial.print(" en ángulo: ");
  Serial.println(angle_GyroX);
  
  Serial.print("Y: ");
  Serial.print(vibration_magnitude_GyroY);
  Serial.print(" en ángulo: ");
  Serial.println(angle_GyroY);
  
  Serial.print("Z: ");
  Serial.print(vibration_magnitude_GyroZ);
  Serial.print(" en ángulo: ");
  Serial.println(angle_GyroZ);

  delay(10); // Esperar un breve tiempo antes de la próxima lectura
}
