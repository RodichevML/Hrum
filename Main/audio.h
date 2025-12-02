#pragma once
#include <Arduino.h>
#include "audio_data_loud.h"   // audioData[], audioLength, audioSampleRate

// Параметры PWM
const int AUDIO_PIN    = 21;
const int PWM_FREQ     = 40000;
const int PWM_RES_BITS = 8;

// Новый тип таймера в ESP32 core 3.x [attached_file:1][web:77]
hw_timer_t* audioTimer = nullptr;
volatile unsigned int sampleIndex = 0;

// ISR таймера
void IRAM_ATTR onAudioTimer(void* arg) {
  if (sampleIndex >= audioLength) {
    // Отключаем alarm (disable = true) [attached_file:1]
    timerAlarm(audioTimer, 0, false, true);
    sampleIndex = 0;
    return;
  }
  uint8_t sample = audioData[sampleIndex++];
  // Новый LEDC API: pin как идентификатор [attached_file:1]
  ledcWrite(AUDIO_PIN, sample);
}

void audioSetup() {
  // Новый LEDC API 3.x: ledcAttach(pin, freq, bits) [attached_file:1]
  ledcAttach(AUDIO_PIN, PWM_FREQ, PWM_RES_BITS);

  // Новый timerBegin(freq_Hz) [attached_file:1][web:77]
  audioTimer = timerBegin(1000000);  // 1 МГц, тик = 1 мкс

  // Новый timerAttachInterruptArg [attached_file:1]
  timerAttachInterruptArg(audioTimer, &onAudioTimer, nullptr);

  uint32_t periodTicks = 1000000UL / audioSampleRate;

  // Новый timerAlarm(timer, periodTicks, auto_reload, enable) [attached_file:1]
  // Настраиваем период и авто‑перезапуск, но пока disable (enable = false)
  timerAlarm(audioTimer, periodTicks, true, false);
}

void audioPlayOnce() {
  sampleIndex = 0;
  uint32_t periodTicks = 1000000UL / audioSampleRate;
  // Включаем alarm (enable = true), period и auto_reload не меняем
  timerAlarm(audioTimer, periodTicks, true, true);
}
