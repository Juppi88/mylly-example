#pragma once

#include "gamedefs.h"
#include "vector.h"

// -------------------------------------------------------------------------------------------------

class Utils
{
public:
	static void Initialize(void);

	static float Random(float min, float max);
	static int Random(int min, int max);
	static bool FlipCoin(void);

	static float RotateTowards(float current, float target, float amount);

	static void GetRandomSpawnPosition(const Vec2 &boundsMin, const Vec2 &boundsMax,
	                                   Vec2 &position, Vec2 &direction);
};
