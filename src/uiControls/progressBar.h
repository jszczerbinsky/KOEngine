#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "../types.h"
#include "../ui.h"
#include "extensions.h"

typedef struct
{
  Texture     *texProgress;
  float        minValue;
  float        maxValue;
  unsigned int progressLayer;
  unsigned int labelLayer;
} ProgressBarSettings;

GameObject *SpawnProgressBar(float x, float y, unsigned int w, unsigned int h, Texture *tex,
                             unsigned int layer, unsigned int uiFlags, Font *font,
                             ProgressBarSettings *s);
void        SetProgressBarValue(GameObject *e, float val);

#endif
