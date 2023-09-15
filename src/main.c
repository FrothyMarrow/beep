#include <SDL.h>
#include <stdint.h>

typedef struct {
  SDL_AudioSpec want;
  SDL_AudioSpec have;
  SDL_AudioDeviceID dev;
} SdlAudio;

typedef struct {
  uint32_t sampleFrequency;
  uint32_t sampleSize;
  uint32_t audioFrequency;
  uint32_t audioAmplitude;
} Config;

void squareWave(void *userdata, Uint8 *stream, int len) {
  Config *config = (Config *)userdata;

  // Track the current sample index
  static uint32_t sampleIndex = 0;

  uint32_t sampleCount = len / sizeof(int16_t);
  uint32_t samplesPerHalfCycle =
      (config->sampleFrequency / config->audioFrequency) / 2;

  // Flip the sample value between positive and negative
  // depending on crest or trough of the wave
  for (uint32_t i = 0; i < sampleCount; i++, sampleIndex++) {
    // Current sample buffer
    int16_t *sampleBuffer = (int16_t *)stream + i;

    if (sampleIndex / samplesPerHalfCycle % 2) {
      *sampleBuffer = config->audioAmplitude;
    } else {
      *sampleBuffer = -config->audioAmplitude;
    }
  }
}

int main(void) {
  // Initialize SDL
  SDL_Init(SDL_INIT_AUDIO);

  SdlAudio audio;
  Config config = {.sampleFrequency = 44100, // Standard CD quality
                   .sampleSize = 2048,       // 2048 samples per callback
                   .audioFrequency = 440,    // A4 frequency
                   .audioAmplitude = 3000};  // Volume

  SDL_zero(audio.want);

  audio.want.freq = config.sampleFrequency; // Samples per second
  audio.want.format = AUDIO_S16;            // Signed 16-bit samples
  audio.want.channels = 1;                  // Mono
  audio.want.samples = config.sampleSize;   // Samples per callback
  audio.want.userdata = &config;            // Passed to callback
  audio.want.callback = squareWave;         // Callback function

  // Open default audio device
  audio.dev = SDL_OpenAudioDevice(NULL, 0, &audio.want, &audio.have, 0);

  // Failed to open an audio device
  if (audio.dev == 0) {
    SDL_Log("Failed to open audio device: %s", SDL_GetError());
    return 1;
  }

  // AudioSpec format mismatch
  if (audio.want.format != audio.have.format) {
    SDL_Log("Failed to get the desired AudioSpec");
    return 1;
  }

  // Unpause the audio device
  SDL_PauseAudioDevice(audio.dev, 0);

  SDL_Delay(1000);

  // Cleanup
  SDL_CloseAudioDevice(audio.dev);
  SDL_Quit();

  return 0;
}
