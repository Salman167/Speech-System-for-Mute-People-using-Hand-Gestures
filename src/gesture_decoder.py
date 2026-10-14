"""Gesture-to-phrase rules from the glove firmware.

The thresholds match firmware/flex_sensor and firmware/accelerometer.
This module does not talk to hardware. It is the same decision table,
so the phrase mapping can be checked without an Arduino.
"""

from __future__ import annotations

FLEX_CLIPS = {
    "1.wav": "Hello, good morning",
    "2.wav": "Can you please do me a favour",
    "3.wav": "Please help me",
    "4.wav": "Can you help me cross the road",
    "5.wav": "Can you please convey my message to him",
    "6.wav": "Thanks a lot, I am so grateful to you",
}

ACCEL_CLIPS = {
    "1.wav": "Hello, good morning",
    "2.wav": "Can you please do me a favour",
    "3.wav": "Please help me",
    "4.wav": "Can you please help me cross the road",
}

TILT_THRESHOLD = 150


def decode_flex(val1: int, val2: int) -> tuple[str, str]:
    """Return (wav filename, spoken phrase) for two averaged flex readings."""
    if val1 > 550 and val2 > 370:
        clip = "1.wav"
    elif val1 > 550 and val2 < 370:
        clip = "2.wav"
    elif 460 < val1 < 530 and val2 > 370:
        clip = "3.wav"
    elif 460 < val1 < 530 and val2 < 370:
        clip = "4.wav"
    elif val1 < 460 and val2 > 370:
        clip = "5.wav"
    else:
        clip = "6.wav"
    return clip, FLEX_CLIPS[clip]


def decode_accelerometer(x_out: float, y_out: float) -> tuple[str, str] | None:
    """Return (wav filename, spoken phrase), or None when the hand is level."""
    if x_out > TILT_THRESHOLD:
        clip = "1.wav"
    elif x_out < -TILT_THRESHOLD:
        clip = "2.wav"
    elif y_out > TILT_THRESHOLD:
        clip = "3.wav"
    elif y_out < -TILT_THRESHOLD:
        clip = "4.wav"
    else:
        return None
    return clip, ACCEL_CLIPS[clip]


def decode_flex_gesture(val1: int, val2: int) -> tuple[str, str] | None:
    """Flex phrases only. Gaps between the bend bands stay silent.

    The original flex sketch plays 6.wav for every unmatched reading, including
    a resting hand. This helper is what the combined glove uses: thanks is the
    open-hand pair (finger 1 below 460 and finger 2 below 370), and the bands
    between thresholds do not speak.
    """
    if val1 > 550 and val2 > 370:
        clip = "1.wav"
    elif val1 > 550 and val2 < 370:
        clip = "2.wav"
    elif 460 < val1 < 530 and val2 > 370:
        clip = "3.wav"
    elif 460 < val1 < 530 and val2 < 370:
        clip = "4.wav"
    elif val1 < 460 and val2 > 370:
        clip = "5.wav"
    elif val1 < 460 and val2 < 370:
        clip = "6.wav"
    else:
        return None
    return clip, FLEX_CLIPS[clip]


def decode_glove(val1: int, val2: int, x_out: float, y_out: float) -> tuple[str, str] | None:
    """Combined glove. A finger gesture wins; otherwise tilt is used."""
    flex = decode_flex_gesture(val1, val2)
    if flex is not None:
        return flex
    return decode_accelerometer(x_out, y_out)
