#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "../types.h"
#include "../ui.h"
#include "extensions.h"

struct ProgressBarSettings
{
	Texture *texProgress;
	float minValue;
	float maxValue;
	unsigned int progressLayer;
	unsigned int labelLayer;
};

Entity *CreateProgressBar(const struct EntitySpawnSettings *ess, const struct UISpawnSettings *uis, const struct ProgressBarSettings *pbs);
void SetProgressBarValue(Entity *e, float val);

#endif
