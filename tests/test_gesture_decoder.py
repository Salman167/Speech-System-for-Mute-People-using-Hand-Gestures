import unittest

from src.cli import main
from src.gesture_decoder import (
    decode_accelerometer,
    decode_flex,
    decode_flex_gesture,
    decode_glove,
)


class FlexDecoderTests(unittest.TestCase):
    def test_both_strongly_bent(self):
        self.assertEqual(decode_flex(600, 400), ("1.wav", "Hello, good morning"))

    def test_first_bent_second_open(self):
        self.assertEqual(
            decode_flex(600, 300),
            ("2.wav", "Can you please do me a favour"),
        )

    def test_mid_bend_with_second_bent(self):
        self.assertEqual(decode_flex(500, 400), ("3.wav", "Please help me"))

    def test_mid_bend_with_second_open(self):
        self.assertEqual(
            decode_flex(500, 300),
            ("4.wav", "Can you help me cross the road"),
        )

    def test_open_first_finger(self):
        self.assertEqual(
            decode_flex(400, 400),
            ("5.wav", "Can you please convey my message to him"),
        )

    def test_remaining_combination(self):
        self.assertEqual(
            decode_flex(400, 300),
            ("6.wav", "Thanks a lot, I am so grateful to you"),
        )

    def test_boundaries_are_exclusive(self):
        # 550 and 460/530 are not inside the strict comparisons used on the glove.
        self.assertEqual(decode_flex(550, 370)[0], "6.wav")
        self.assertEqual(decode_flex(460, 400)[0], "6.wav")
        self.assertEqual(decode_flex(530, 400)[0], "6.wav")


class AccelerometerDecoderTests(unittest.TestCase):
    def test_forward(self):
        self.assertEqual(
            decode_accelerometer(200, 0),
            ("1.wav", "Hello, good morning"),
        )

    def test_backward(self):
        self.assertEqual(
            decode_accelerometer(-200, 0),
            ("2.wav", "Can you please do me a favour"),
        )

    def test_right(self):
        self.assertEqual(decode_accelerometer(0, 200), ("3.wav", "Please help me"))

    def test_left(self):
        self.assertEqual(
            decode_accelerometer(0, -200),
            ("4.wav", "Can you please help me cross the road"),
        )

    def test_level_hand_is_silent(self):
        self.assertIsNone(decode_accelerometer(150, -150))

    def test_x_wins_over_y(self):
        self.assertEqual(decode_accelerometer(200, 200)[0], "1.wav")


class GloveDecoderTests(unittest.TestCase):
    def test_open_hand_is_thanks(self):
        self.assertEqual(
            decode_flex_gesture(400, 300),
            ("6.wav", "Thanks a lot, I am so grateful to you"),
        )

    def test_gap_between_bands_is_silent(self):
        self.assertIsNone(decode_flex_gesture(540, 400))

    def test_finger_gesture_wins_over_tilt(self):
        self.assertEqual(decode_glove(600, 400, -200, 0)[0], "1.wav")

    def test_tilt_is_used_when_fingers_are_between_bands(self):
        self.assertEqual(
            decode_glove(540, 400, 0, -200),
            ("4.wav", "Can you please help me cross the road"),
        )

    def test_rest_is_silent(self):
        self.assertIsNone(decode_glove(540, 370, 0, 0))


class CliTests(unittest.TestCase):
    def test_flex_command(self):
        self.assertEqual(main(["flex", "600", "400"]), 0)

    def test_tilt_rest_is_silent(self):
        self.assertEqual(main(["tilt", "0", "0"]), 0)

    def test_bad_args(self):
        self.assertEqual(main(["flex"]), 2)


if __name__ == "__main__":
    unittest.main()
