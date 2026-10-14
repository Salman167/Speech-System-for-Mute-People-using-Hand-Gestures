/*
 * Speech system for mute people — flex-sensor glove
 * Board: Arduino Mega 2560
 * Two flex sensors on A1 and A2. Each bend combination plays a WAV
 * from the microSD card through a speaker on pin 11 (TMRpcm).
 *
 * SD module uses SPI. On the Mega the hardware SPI pins are
 * 50 (MISO), 51 (MOSI), 52 (SCK). Chip select is digital pin 4.
 *
 * WAV files must be 8-bit, 8–32 kHz, mono PCM, named 1.wav … 6.wav
 * on the root of a FAT16/FAT32 card.
 */

#include <SD.h>
#include <TMRpcm.h>

#define SD_CHIP_SELECT 4
#define FLEX_1 A1
#define FLEX_2 A2
#define SPEAKER_PIN 11
#define PLAYBACK_MS 3100
#define LONG_PLAYBACK_MS 4000
#define SAMPLE_COUNT 200

TMRpcm tmrpcm;

int val1 = 0;
int val2 = 0;

void setup() {
  pinMode(FLEX_1, INPUT);
  pinMode(FLEX_2, INPUT);
  Serial.begin(9600);

  tmrpcm.speakerPin = SPEAKER_PIN;
  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("SD card failed");
    return;
  }
  tmrpcm.setVolume(7);
}

void loop() {
  val1 = averageReading(FLEX_1);
  val2 = averageReading(FLEX_2);
  Serial.print("value 1 = ");
  Serial.println(val1);
  Serial.print("value 2 = ");
  Serial.println(val2);

  if ((val1 > 550) && (val2 > 370)) {
    playClip("1.wav", PLAYBACK_MS);          // Hello, good morning
  } else if ((val1 > 550) && (val2 < 370)) {
    playClip("2.wav", PLAYBACK_MS);          // Can you please do me a favour
  } else if ((val1 > 460) && (val1 < 530) && (val2 > 370)) {
    playClip("3.wav", PLAYBACK_MS);          // Please help me
  } else if ((val1 > 460) && (val1 < 530) && (val2 < 370)) {
    playClip("4.wav", LONG_PLAYBACK_MS);     // Can you help me cross the road
  } else if ((val1 < 460) && (val2 > 370)) {
    playClip("5.wav", PLAYBACK_MS);          // Can you please convey my message to him
  } else {
    playClip("6.wav", PLAYBACK_MS);          // Thanks a lot, I am so grateful to you
  }
}

int averageReading(int flexPin) {
  long sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    sum += analogRead(flexPin);
  }
  return sum / SAMPLE_COUNT;
}

void playClip(const char *filename, int durationMs) {
  tmrpcm.play(filename);
  delay(durationMs);
}
