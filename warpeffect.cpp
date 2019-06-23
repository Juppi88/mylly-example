#include "warpeffect.h"
#include "game.h"
#include "ship.h"
#include "scene.h"
#include <mylly/core/time.h>
#include <mylly/scene/object.h>
#include <mylly/scene/emitter.h>
#include <mylly/renderer/shader.h>
#include <mylly/resources/resources.h>
#include <mylly/math/math.h>
#include <mylly/audio/audiosystem.h>

// -------------------------------------------------------------------------------------------------

WarpEffect::WarpEffect(Ship *playerShip)
{
	m_playerShip = playerShip;
}

WarpEffect::~WarpEffect(void)
{
}

void WarpEffect::Setup(Game* game)
{
	// Spawn an FTL flash effect. The effect is automatically destroyed when it stops.
	game->GetScene()->SpawnEffect("ftl", m_playerShip->GetPosition());

	// Scale the player's ship to a very small size.
	obj_set_local_scale(m_playerShip->GetSceneObject(), vec3(0, 0, 0));


	// Store a reference to the bloom shader so it can be adjusted whenever an effect requiring
	// bloom occurs.
	m_bloomShader = res_get_shader("effect-bloom");

	if (m_bloomShader != nullptr) {

		game->GetScene()->AddCameraEffect(m_bloomShader);

		shader_set_uniform_float(m_bloomShader, "BloomQuality", 2.5f);
		shader_set_uniform_float(m_bloomShader, "BloomFactor", 0.0f);
		shader_set_uniform_int(m_bloomShader, "BloomSamples", 5);
	}

	// Shake the camera a bit.
	game->GetScene()->ShakeCamera(0.3f, 0.4f);

	// Play a hyperspace sound effect.
	audio_play_sound(res_get_sound("Warp"), 0);
}

bool WarpEffect::Update(Game* game)
{
	UNUSED(game);

	m_timeElapsed += get_time().delta_time;

	float t = m_timeElapsed / EFFECT_DURATION;
	t = CLAMP01(t);

	// Gradually scale the ship back to its right size.
	float stretchFactor = t;
	float scale = stretchFactor * 0.8f;
	float scaleX = lerpf(5.0f, 0.8f, stretchFactor);

	obj_set_local_scale(m_playerShip->GetSceneObject(), vec3(scaleX, scale, scale));

	// Also gradually fade off the bloom effect.
	shader_set_uniform_float(m_bloomShader, "BloomFactor", 1 - t);

	bool isFinished = (t >= 1);

	if (isFinished) {

		// Disable the bloom effect before the effect is destroyed.
		game->GetScene()->RemoveCameraEffect(m_bloomShader);
		m_bloomShader = nullptr;
	}

	return isFinished;
}
