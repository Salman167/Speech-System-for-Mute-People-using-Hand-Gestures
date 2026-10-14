"""Print the phrase a glove reading would speak.

Examples:
    python -m src.cli flex 600 400
    python -m src.cli tilt -200 0
    python -m src.cli glove 400 300 0 0
"""

from __future__ import annotations

import sys

from src.gesture_decoder import decode_accelerometer, decode_flex, decode_glove


def main(argv: list[str] | None = None) -> int:
    args = list(sys.argv[1:] if argv is None else argv)
    usage = (
        "usage:\n"
        "  python -m src.cli flex <finger1> <finger2>\n"
        "  python -m src.cli tilt <x> <y>\n"
        "  python -m src.cli glove <finger1> <finger2> <x> <y>"
    )
    if not args or args[0] in {"-h", "--help"}:
        print(usage)
        return 0 if args else 2

    mode = args[0]
    try:
        numbers = [float(value) for value in args[1:]]
    except ValueError:
        print(usage)
        return 2

    if mode == "flex" and len(numbers) == 2:
        clip, phrase = decode_flex(int(numbers[0]), int(numbers[1]))
        result = (clip, phrase)
    elif mode == "tilt" and len(numbers) == 2:
        result = decode_accelerometer(numbers[0], numbers[1])
    elif mode == "glove" and len(numbers) == 4:
        result = decode_glove(int(numbers[0]), int(numbers[1]), numbers[2], numbers[3])
    else:
        print(usage)
        return 2

    if result is None:
        print("silent")
        return 0
    clip, phrase = result
    print(f"{clip}: {phrase}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
