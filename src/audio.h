#ifndef KO_SOUND_H
#define KO_SOUND_H

#include "types.h"

struct PlayMusicSettings
{
  unsigned int fadeInTime;
  double startPos;
};

SoundEffect  *LoadSoundEffect (const char *path, int channel);
void          FreeSoundEffect (SoundEffect *effect);
void          SoundPlay       (SoundEffect *effect);
void          HaltChannel     (int channel);

Music *LoadMusic  (const char *path);
void FreeMusic    (Music *music);
void MusicPlay    (Music *music, const struct PlayMusicSettings *s);
void MusicPause   ();
void MusicResume  ();

#endif
