# Speech System for Mute People using Hand Gestures

Wearable sign-to-speech glove. Hand position is read with flex sensors or an ADXL345 accelerometer, matched to a stored phrase, and played as a WAV file through a speaker. Built on an Arduino Mega 2560 for the Signals and Systems course (EC430), JSS Science and Technology University, 2020–2021.

## What is in this repo

| Path | What it is |
| --- | --- |
| `firmware/flex_sensor/flex_sensor.ino` | Original two-finger flex glove. Six bend combinations, six phrases. |
| `firmware/accelerometer/accelerometer.ino` | Original ADXL345 tilt. Four directions, four phrases. |
| `firmware/glove/glove.ino` | Both sensors together. A resting hand stays silent, and a phrase is not repeated until the hand relaxes. |
| `firmware/calibrate/calibrate.ino` | Prints finger and tilt readings on Serial so thresholds can be tuned. No speaker. |
| `src/gesture_decoder.py` | The same decision rules, without hardware. |
| `src/cli.py` | Prints the phrase for a flex, tilt, or combined reading. |
| `tests/test_gesture_decoder.py` | Checks each gesture maps to the phrase in the report. |
| `Accelerometer code.txt`, `Flex sensor code.txt` | Original sketches as uploaded. |
| `Final report_compressed.pdf` | Project report. |
| `Output.txt` | Demo video links. |

## Phrases

Flex sensor (`A1` = finger 1, `A2` = finger 2). Readings are the average of 200 samples.

| Finger 1 | Finger 2 | File | Phrase |
| --- | --- | --- | --- |
| > 550 | > 370 | `1.wav` | Hello, good morning |
| > 550 | < 370 | `2.wav` | Can you please do me a favour |
| 461–529 | > 370 | `3.wav` | Please help me |
| 461–529 | < 370 | `4.wav` | Can you help me cross the road |
| < 460 | > 370 | `5.wav` | Can you please convey my message to him |
| anything else | | `6.wav` | Thanks a lot, I am so grateful to you |

Accelerometer. Raw counts, threshold 150. A level hand stays silent. X is checked before Y.

| Tilt | File | Phrase |
| --- | --- | --- |
| +X (forward) | `1.wav` | Hello, good morning |
| −X (backward) | `2.wav` | Can you please do me a favour |
| +Y (right) | `3.wav` | Please help me |
| −Y (left) | `4.wav` | Can you please help me cross the road |

## Wiring (Arduino Mega 2560)

| Part | Connection |
| --- | --- |
| Flex sensor 1 | `A1`, with a series resistor as a voltage divider |
| Flex sensor 2 | `A2`, same divider |
| ADXL345 | I2C `0x53` — SDA `20`, SCL `21` |
| MicroSD module | SPI: MISO `50`, MOSI `51`, SCK `52`, CS `4` |
| Speaker | Digital pin `11` (TMRpcm PWM) |

Audio files are 8-bit mono PCM WAV, 8–32 kHz, on the root of a FAT16/FAT32 card. Libraries: [SD](https://www.arduino.cc/reference/en/libraries/sd/), [TMRpcm](https://github.com/TMRh20/TMRpcm).

Open the sketch folder in the Arduino IDE, select **Arduino Mega or Mega 2560**, and upload.

The original flex sketch plays `6.wav` for every reading that is not one of the first five gestures, including a hand at rest. `firmware/glove` keeps that sixth phrase for an open hand (finger 1 below 460 and finger 2 below 370) and stays quiet in the gaps between bands. Finger gestures are checked before tilt. The same clip is not played again until the sensors return to rest.

## Calibrate

Upload `firmware/calibrate`, then open the Serial Monitor at 9600 baud. Each line is `f1,f2,x,y,z`. Hold a gesture, write down the numbers, and adjust the thresholds in the sketch if your sensors sit in a different range.

## Check a reading

```bash
python -m unittest tests.test_gesture_decoder
python -m src.cli flex 600 400
python -m src.cli tilt 0 -200
python -m src.cli glove 540 400 0 -200
```

`silent` means the combined glove would not speak for that reading.

## Demos

1. Flex sensor — https://youtu.be/f8KMneHpYm8
2. Accelerometer — https://youtu.be/35Ss9eQTRpA
