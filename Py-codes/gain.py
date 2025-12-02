import wave
import math

INPUT_WAV = "hy_8k_u8.wav"   # тот же файл, что ты подавал в конвертер
OUTPUT_H  = "audio_data_loud.h"
GAIN      = 5            # во сколько раз увеличить амплитуду

with wave.open(INPUT_WAV, "rb") as w:
    n_channels  = w.getnchannels()
    sample_rate = w.getframerate()
    sampwidth   = w.getsampwidth()
    n_frames    = w.getnframes()
    frames      = w.readframes(n_frames)

print("Channels:", n_channels, "Rate:", sample_rate, "Width bytes:", sampwidth)

if n_channels != 1 or sampwidth != 1:
    raise RuntimeError("Нужен моно 8‑бит PCM (pcm_u8)!")

in_bytes = frames
out_bytes = bytearray(len(in_bytes))

for i, b in enumerate(in_bytes):
    x = b - 128               # -128..127
    y = int(x * GAIN)         # усилить
    if y > 127:
        y = 127
    if y < -128:
        y = -128
    out_bytes[i] = y + 128    # обратно в 0..255

with open(OUTPUT_H, "w", encoding="utf-8") as f:
    f.write("#pragma once\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write(f"const unsigned int audioSampleRate = {sample_rate};\n")
    f.write(f"const unsigned int audioLength = {len(out_bytes)};\n\n")
    f.write("const uint8_t audioData[] PROGMEM = {\n  ")

    for i, b in enumerate(out_bytes):
        f.write(str(b) + ",")
        if (i + 1) % 16 == 0 and i != len(out_bytes) - 1:
            f.write("\n  ")
        else:
            f.write(" ")

    f.write("\n};\n")
