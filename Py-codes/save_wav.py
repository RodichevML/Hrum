import wave

INPUT_WAV = "hy_8k_u8.wav"
OUTPUT_H  = "audio_data.h"

with wave.open(INPUT_WAV, "rb") as w:
    n_channels  = w.getnchannels()
    sample_rate = w.getframerate()
    sampwidth   = w.getsampwidth()
    n_frames    = w.getnframes()
    frames      = w.readframes(n_frames)

print("Channels:", n_channels)
print("Rate:", sample_rate)
print("Width bytes:", sampwidth)
print("Frames:", n_frames)
print("Bytes:", len(frames))

# ПРОВЕРКА: должен быть 8‑бит моно
if n_channels != 1 or sampwidth != 1:
    raise RuntimeError("Нужно моно 8‑бит PCM (pcm_u8)! Переконвертируй WAV.")

num_bytes = len(frames)

with open(OUTPUT_H, "w", encoding="utf-8") as f:
    f.write("#pragma once\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write(f"const unsigned int audioSampleRate = {sample_rate};\n")
    f.write(f"const unsigned int audioLength = {num_bytes};\n\n")
    f.write("const uint8_t audioData[] PROGMEM = {\n  ")

    for i, b in enumerate(frames):
        f.write(str(b))
        f.write(",")               # запятая после КАЖДОГО элемента — допустимо
        if (i + 1) % 16 == 0 and i != num_bytes - 1:
            f.write("\n  ")
        else:
            f.write(" ")

    f.write("\n};\n")
