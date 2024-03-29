#include "utils.h"
#include <time.h>
#include <stdlib.h>
#include <mylly/math/math.h>
#include <mylly/resources/resources.h>

// -------------------------------------------------------------------------------------------------

void Utils::Initialize(void)
{
	// Seed random number generator.
	srand(time(NULL));
}

float Utils::Random(float min, float max)
{
	return min + ((float)rand() / RAND_MAX) * (max - min);
}

int Utils::Random(int min, int max)
{
	return min + (rand() % (max - min));
}

bool Utils::FlipCoin(void)
{
	return ((rand() & 1) == 0);
}

float Utils::RotateTowards(float current, float target, float amount)
{
	float a = target - current;
	float b = target - current + 360;
	float c = target - current - 360;
	float absA = fabsf(a);
	float absB = fabsf(b);
	float absC = fabsf(c);

	// Figure out which one of the a, b, c has the smallest absolute value.
	float min = a;
	float absMin = absA;
	
	if (absB < absMin) {
		min = b;
		absMin = absB;
	}
	if (absC < absMin) {
		min = c;
		absMin = absC;
	}

	// Don't over-rotate.
	if (absMin < amount) {
		amount = absMin;
	}

	// The sign of the smallest value tells the direction we need to rotate in order to reach
	// the target angle.
	if (min < 0) {
		amount *= -1;
	}

	return math_sanitize_angle_deg(current + amount);
}

void Utils::GetRandomSpawnPosition(const Vec2 &boundsMin, const Vec2 &boundsMax,
                                   Vec2 &position, Vec2 &direction)
{
	switch (Utils::Random(0, 4)) {

		case 0: // Left
			position = Vec2(boundsMin.x(), Random(boundsMin.y(), boundsMax.y()));
			direction = vec2(Random(0.0f, 1.0f), Random(-1.0f, 1.0f));
			break;

		case 1: // Right
			position = Vec2(boundsMax.x(), Random(boundsMin.y(), boundsMax.y()));
			direction = Vec2(Random(0.0f, -1.0f), Random(-1.0f, 1.0f));
			break;

		case 2: // Top
			position = Vec2(Random(boundsMin.x(), boundsMax.x()), boundsMax.y());
			direction = Vec2(Random(-1.0f, 1.0f), Random(0.0f, -1.0f));
			break;

		default: // Bottom
			position = Vec2(Random(boundsMin.x(), boundsMax.x()), boundsMin.y());
			direction = Vec2(Random(-1.0f, 1.0f), Random(0.0f, 1.0f));
			break;
	}
}
