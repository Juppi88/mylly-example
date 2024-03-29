#include "asteroid.h"
#include "asteroidhandler.h"
#include "game.h"
#include "utils.h"
#include "projectile.h"
#include <mylly/scene/scene.h>
#include <mylly/scene/object.h>
#include <mylly/scene/model.h>
#include <mylly/core/time.h>
#include <mylly/resources/resources.h>
#include <mylly/math/math.h>
#include <mylly/renderer/debug.h>
#include <mylly/audio/audiosystem.h>

// -------------------------------------------------------------------------------------------------

Asteroid::Asteroid(void) :
	FloatingObject(ENTITY_ASTEROID)
{
}

Asteroid::~Asteroid(void)
{
}

void Asteroid::Spawn(Game *game)
{
	if (IsSpawned()) {
		return;
	}

	Entity::Spawn(game);

	// Create an empty parent object for the asteroid which we can rotate around freely.
	SetSceneObject(game->SpawnSceneObject());

	// Create the asteroid object.
	object_t *asteroidObject = game->SpawnSceneObject(GetSceneObject());

	// Load and set an asteroid model.
	model_t *asteroidModel = res_get_model("rock01");
	obj_set_model(asteroidObject, asteroidModel);

	// Randomize the asteroid's initial position.
	quat_t randomRotation = quat_from_euler_deg(
		Utils::Random(0.0f, 360.0f), Utils::Random(0.0f, 360.0f), Utils::Random(0.0f, 360.0f));

	obj_set_local_rotation(asteroidObject, randomRotation);
}

void Asteroid::SetSize(AsteroidSize size)
{
	if (!IsSpawned()) {
		return;
	}

	m_size = size;

	// Set the asteroid's health and model's scale based on the size of the asteroid.
	float scale, mass;

	switch (size) {

		case ASTEROID_LARGE:
			scale = 3.0f;
			mass = 300.0f;
			SetHealth(3);
			break;

		case ASTEROID_MEDIUM:
			scale = 2.0f;
			mass = 150.0f;
			SetHealth(2);
			break;

		default:
			mass = 50.0f;
			scale = 1.0f;
			SetHealth(1);
			break;
	}

	obj_set_local_scale(GetSceneObject(), vec3(scale, scale, scale));
	
	SetBoundingRadius(0.8f * scale);
	SetMass(mass);
	SetMaxSpeed(GetSpeedMultiplier() * MOVEMENT_SPEED_MAX);
}

void Asteroid::SetDirection(const Vec2 &direction)
{
	Vec2 velocity = direction;
	velocity.Normalize();

	// Randomize the speed at which the asteroid is moving.
	float speed = GetSpeedMultiplier() * Utils::Random(MOVEMENT_SPEED_MIN, MOVEMENT_SPEED_MAX);
	
	SetVelocity(direction * speed);
}

void Asteroid::Destroy(Game *game)
{
	if (!IsSpawned()) {
		return;
	}

	// Spawn a cool asteroid breaking effect.
	game->GetScene()->SpawnEffect("asteroid-explosion", GetPosition());

	game->GetScene()->SpawnLightFlash(GetPosition(), col(255, 175, 50), 10, 0.25f);

	audio_play_sound(res_get_sound("SmallExplosion"), 0);

	// Do final cleanup.
	game->GetScene()->GetAsteroidHandler()->RemoveReference(this);
	Entity::Destroy(game);
}

void Asteroid::OnCollideWith(const Game *game, Entity *other)
{
	Entity::OnCollideWith(game, other);

	if (other->GetType() == ENTITY_PROJECTILE &&
		((Projectile *)other)->IsOwnedByPlayer()) { // Ignore hits by UFO projectiles

		// Calculate damage to the asteroid.
		DecreaseHealth();
	}
	else if (other->GetType() == ENTITY_ASTEROID) {
		game->GetScene()->SpawnEffect("asteroid-dust", GetPosition());
	}
}
