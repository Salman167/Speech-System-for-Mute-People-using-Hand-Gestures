/*
 * Serial calibration for the glove. No SD card and no speaker.
 * Open Serial Monitor at 9600 baud, hold each gesture, and note
 * the finger and tilt numbers before changing thresholds.
 *
 * Board: Arduino Mega 2560
 * Flex 1 -> A1, Flex 2 -> A2, ADXL345 -> SDA 20, SCL 21, address 0x53
 */

#include <Wire.h>

#define FLEX_1 A1
#define FLEX_2 A2
#define ADXL345_ADDR 0x53
#define REG_POWER_CTL 0x2D
#define REG_DATA_X0 0x32
#define SAMPLE_COUNT 200

void setup() {
  Serial.begin(9600);
  pinMode(FLEX_1, INPUT);
  pinMode(FLEX_2, INPUT);
  Wire.begin();

  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(REG_POWER_CTL);
  Wire.write(8);
  Wire.endTransmission();
  delay(10);

  Serial.println("f1,f2,x,y,z");
}

void loop() {
  int xOut;
  int yOut;
  int zOut;
  readAxes(xOut, yOut, zOut);

  Serial.print(averageReading(FLEX_1));
  Serial.print(",");
  Serial.print(averageReading(FLEX_2));
  Serial.print(",");
  Serial.print(xOut);
  Serial.print(",");
  Serial.print(yOut);
  Serial.print(",");
  Serial.println(zOut);
  delay(400);
}

int averageReading(int flexPin) {
  long sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += analogRead(flexPin);
  }
  return sum / SAMPLE_COUNT;
}

void readAxes(int &xOut, int &yOut, int &zOut) {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(REG_DATA_X0);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345_ADDR, 6, true);
  xOut = Wire.read() | (Wire.read() << 8);
  yOut = Wire.read() | (Wire.read() << 8);
  zOut = Wire.read() | (Wire.read() << 8);
}
