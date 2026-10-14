/*
 * Combined glove: flex sensors plus ADXL345.
 * Board: Arduino Mega 2560
 *
 * A finger gesture is spoken first. If the fingers are between bands,
 * tilt selects a phrase. A resting hand stays silent, and the same
 * phrase is not repeated until the hand returns to rest.
 *
 * WAV files 1.wav–6.wav on the SD card. See README for the phrase table.
 */

#include <Wire.h>
#include <SD.h>
#include <TMRpcm.h>

#define SD_CHIP_SELECT 4
#define SPEAKER_PIN 11
#define FLEX_1 A1
#define FLEX_2 A2
#define ADXL345_ADDR 0x53
#define REG_POWER_CTL 0x2D
#define REG_DATA_X0 0x32
#define REG_OFSX 0x1E
#define REG_OFSY 0x1F
#define REG_OFSZ 0x20
#define TILT_THRESHOLD 150
#define SAMPLE_COUNT 200
#define PLAYBACK_MS 3100

TMRpcm tmrpcm;

const char *activeClip = nullptr;

void setup() {
  Serial.begin(9600);
  pinMode(FLEX_1, INPUT);
  pinMode(FLEX_2, INPUT);
  Wire.begin();
  writeRegister(REG_POWER_CTL, 8);
  delay(10);

  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("SD card failed");
    return;
  }
  tmrpcm.speakerPin = SPEAKER_PIN;
  tmrpcm.setVolume(7);

  writeRegister(REG_OFSX, 1);
  delay(10);
  writeRegister(REG_OFSY, (byte)-2);
  delay(10);
  writeRegister(REG_OFSZ, (byte)-7);
  delay(10);
}

void loop() {
  int finger1 = averageReading(FLEX_1);
  int finger2 = averageReading(FLEX_2);
  int xOut;
  int yOut;
  int zOut;
  readAxes(xOut, yOut, zOut);

  const char *clip = matchFlex(finger1, finger2);
  if (clip == nullptr) {
    clip = matchTilt(xOut, yOut);
  }

  Serial.print("f1=");
  Serial.print(finger1);
  Serial.print(" f2=");
  Serial.print(finger2);
  Serial.print(" x=");
  Serial.print(xOut);
  Serial.print(" y=");
  Serial.println(yOut);

  if (clip == nullptr) {
    activeClip = nullptr;
    return;
  }
  if ((activeClip != nullptr && strcmp(clip, activeClip) == 0) || tmrpcm.isPlaying()) {
    return;
  }
  activeClip = clip;
  tmrpcm.play(clip);
  delay(PLAYBACK_MS);
}

const char *matchFlex(int val1, int val2) {
  if (val1 > 550 && val2 > 370) return "1.wav";
  if (val1 > 550 && val2 < 370) return "2.wav";
  if (val1 > 460 && val1 < 530 && val2 > 370) return "3.wav";
  if (val1 > 460 && val1 < 530 && val2 < 370) return "4.wav";
  if (val1 < 460 && val2 > 370) return "5.wav";
  if (val1 < 460 && val2 < 370) return "6.wav";
  return nullptr;
}

const char *matchTilt(int xOut, int yOut) {
  if (xOut > TILT_THRESHOLD) return "1.wav";
  if (xOut < -TILT_THRESHOLD) return "2.wav";
  if (yOut > TILT_THRESHOLD) return "3.wav";
  if (yOut < -TILT_THRESHOLD) return "4.wav";
  return nullptr;
}

int averageReading(int flexPin) {
  long sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += analogRead(flexPin);
  }
  return sum / SAMPLE_COUNT;
}

void writeRegister(byte reg, byte value) {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
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
