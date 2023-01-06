#include "progressBar.h"

#include "../types.h"

struct ProgressBarInstance
{
  GameObject *progress;
  GameObject *bg;
  float       value;
  float       minValue;
  float       maxValue;
};

static void freeProgressBarInstance(void *pbHookPtr, int killingAll)
{
  struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *)pbHookPtr;

  if (!killingAll)
  {
    KillGameObject(pbHook->bg);
    KillGameObject(pbHook->progress);
  }
}

void SetProgressBarValue(GameObject *e, float val)
{
  struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *)e->extension;

  if (val < pbHook->minValue)
    val = pbHook->minValue;
  else if (val > pbHook->maxValue)
    val = pbHook->maxValue;

  pbHook->value = val;

  pbHook->progress->width           = e->width * (val - pbHook->minValue) / pbHook->maxValue;
  pbHook->progress->localPosition.x = pbHook->progress->width / 2 - (float)e->width / 2;
};

GameObject *SpawnProgressBar(float x, float y, unsigned int w, unsigned int h, Texture *tex,
                             unsigned int layer, unsigned int uiFlags, Font *font,
                             ProgressBarSettings *s)

{
  GameObject *obj = SpawnUiObject(x, y, w, h, NULL, s->labelLayer, uiFlags, font);

  obj->freeExtension = &freeProgressBarInstance;
  obj->extension     = malloc(sizeof(struct ProgressBarInstance));
  obj->extensionType = EXT_UICONTROLS_PROGRESSBAR;

  struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *)obj->extension;
  pbHook->minValue                   = s->minValue;
  pbHook->maxValue                   = s->maxValue;
  pbHook->value                      = 0;

  pbHook->bg         = SpawnUiObject(0, 0, w, h, tex, layer, 0, NULL);
  pbHook->bg->parent = obj;

  pbHook->progress         = SpawnUiObject(0, 0, 0, h, s->texProgress, s->progressLayer, 0, NULL);
  pbHook->progress->parent = obj;

  return obj;
}
