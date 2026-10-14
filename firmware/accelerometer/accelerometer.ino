/*
 * Speech system for mute people — ADXL345 hand tilt
 * Board: Arduino Mega 2560
 * ADXL345 on I2C address 0x53. Tilt along X or Y selects a WAV
 * on the microSD card and plays it on speaker pin 11 (TMRpcm).
 *
 * SD module uses SPI. On the Mega the hardware SPI pins are
 * 50 (MISO), 51 (MOSI), 52 (SCK). Chip select is digital pin 4.
 * I2C on the Mega is SDA 20 and SCL 21.
 *
 * WAV files must be 8-bit, 8–32 kHz, mono PCM, named 1.wav … 4.wav.
 */

#include <Wire.h>
#include <SD.h>
#include <TMRpcm.h>

#define SD_CHIP_SELECT 4
#define SPEAKER_PIN 11
#define ADXL345_ADDR 0x53
#define REG_POWER_CTL 0x2D
#define REG_DATA_X0 0x32
#define REG_OFSX 0x1E
#define REG_OFSY 0x1F
#define REG_OFSZ 0x20
#define TILT_THRESHOLD 150
#define PLAYBACK_MS 4000

TMRpcm tmrpcm;

float xOut;
float yOut;
float zOut;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  writeRegister(REG_POWER_CTL, 8);  // measurement mode, D3 = 1
  delay(10);

  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("SD card failed");
    return;
  }
  tmrpcm.speakerPin = SPEAKER_PIN;
  tmrpcm.setVolume(7);

  // Offsets recorded for this glove build.
  writeRegister(REG_OFSX, 1);
  delay(10);
  writeRegister(REG_OFSY, -2);
  delay(10);
  writeRegister(REG_OFSZ, -7);
  delay(10);
}

void loop() {
  readAxes();

  if (xOut > TILT_THRESHOLD) {
    playClip("1.wav");   // forward — Hello, good morning
  } else if (xOut < -TILT_THRESHOLD) {
    playClip("2.wav");   // backward — Can you please do me a favour
  } else if (yOut > TILT_THRESHOLD) {
    playClip("3.wav");   // right — Please help me
  } else if (yOut < -TILT_THRESHOLD) {
    playClip("4.wav");   // left — Can you please help me cross the road
  }
}

void writeRegister(byte reg, byte value) {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void readAxes() {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(REG_DATA_X0);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345_ADDR, 6, true);

  xOut = Wire.read() | (Wire.read() << 8);
  yOut = Wire.read() | (Wire.read() << 8);
  zOut = Wire.read() | (Wire.read() << 8);
}

void playClip(const char *filename) {
  tmrpcm.play(filename);
  delay(PLAYBACK_MS);
}
