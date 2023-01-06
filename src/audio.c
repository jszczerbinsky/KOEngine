#include "audio.h"

#include <SDL2/SDL_mixer.h>

#include "log.h"

Music *lastMusic  = NULL;
int    musicEnded = 0;

static void musicFinishedCb() { musicEnded = 1; }

void initSound() { Mix_HookMusicFinished(musicFinishedCb); }

SoundEffect *LoadSoundEffect(const char *path, int channel)
{
  SoundEffect *effect = malloc(sizeof(SoundEffect));
  effect->channel     = channel;
  effect->chunk       = Mix_LoadWAV(path);
  if (!effect->chunk)
  {
    Log("ERROR: can't load audio file (%s): %s", path, Mix_GetError());
  }
  return effect;
}

void FreeSoundEffect(SoundEffect *effect)
{
  Mix_FreeChunk(effect->chunk);
  free(effect);
}

void SoundPlay(SoundEffect *effect)
{
  if (effect) Mix_PlayChannel(effect->channel, effect->chunk, 0);
}

void HaltChannel(int channel) { Mix_HaltChannel(channel); }

Music *LoadMusic(const char *path)
{
  Music *music = Mix_LoadMUS(path);
  if (!music)
  {
    Log("ERROR: can't load music file (%s): %s", path, Mix_GetError());
  }
  return music;
}

void FreeMusic(Music *music) { Mix_FreeMusic(music); }

void MusicPlay(Music *music, const MusicSettings *s)
{
  if (!music) return;

  lastMusic = music;
  if (Mix_FadeInMusicPos(music, 1, s->fadeInTime, s->startPos) < 0)
    Log("WARNING, can't play music: %s", Mix_GetError());
}

void MusicPause() { Mix_PauseMusic(); }

void MusicResume() { Mix_ResumeMusic(); }
