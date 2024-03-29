#include "projectile.h"
#include "projectilehandler.h"
#include "game.h"
#include <mylly/scene/object.h>
#include <mylly/scene/light.h>
#include <mylly/scene/emitter.h>
#include <mylly/resources/resources.h>
#include <mylly/core/time.h>
#include <mylly/math/math.h>

// -------------------------------------------------------------------------------------------------

Projectile::Projectile(void) :
	Entity(ENTITY_PROJECTILE)
{
	SetDrawDepth(-5);
	SetCollidable(false);
	SetBoundingRadius(0.3f);
}

Projectile::~Projectile(void)
{
}

void Projectile::Spawn(Game *game)
{
	if (IsSpawned()) {
		return;
	}

	Entity::Spawn(game);

	// Load the bullet sprite.
	const char *spriteName = (IsOwnedByPlayer() ? "gloweffect/4" : "gloweffect-purple/4");
	sprite_t *bulletSprite = res_get_sprite(spriteName);

	if (bulletSprite == nullptr) {
		return;
	}

	// Spawn an object into the scene and add the sprite to it.
	SetSceneObject(game->SpawnSceneObject());
	obj_set_sprite(GetSceneObject(), bulletSprite);

	// Rotate the sprite towards the camera.
	obj_set_local_rotation(GetSceneObject(), quat_from_euler_deg(90, 0, 0));
	obj_set_local_scale(GetSceneObject(), vec3(0.15f, 0.15f, 0.15f));

	// Add a light component to the projectile so it lights the asteroids it hits.
	light_t *light = obj_add_light(GetSceneObject());

	light_set_type(light, LIGHT_POINT);
	light_set_range(light, 10.0f);
	light_set_intensity(light, 3);

	// Attach a particle emitter to the projectile for a trail effect.
	if (IsOwnedByPlayer()) {

		m_trailEmitter = game->GetScene()->SpawnEffect("projectile-trail", GetPosition());
		light_set_colour(light, col(100, 150, 200));
	}
	else {

		m_trailEmitter = game->GetScene()->SpawnEffect("projectile2-trail", GetPosition());
		light_set_colour(light, col(200, 100, 150));
	}

	// Projectiles are automatically destroyed after a while if they don't hit anything.
	m_expiresTime = get_time().time + (IsOwnedByPlayer() ? PLAYER_LIFETIME : UFO_LIFETIME);
}

void Projectile::Destroy(Game *game)
{
	if (!IsSpawned()) {
		return;
	}

	game->GetScene()->GetProjectileHandler()->RemoveReference(this);

	// Stop trail particle emitter. The effect is automatically destroyed when it no longer has
	// active particles.
	emitter_stop(m_trailEmitter);

	// Do final cleanup.
	Entity::Destroy(game);
}

void Projectile::Update(Game *game)
{
	if (!IsSpawned()) {
		return;
	}

	// Move the projectile.
	Vec2 movement = GetVelocity() * get_time().delta_time;
	SetPosition(GetPosition() + movement);

	Entity::Update(game);

	// Destroy the projectile if it has hit something (an asteroid) or has been flying for a while.
	if (IsDestroyed() ||
		get_time().time >= m_expiresTime) {

		Destroy(game);
	}

	// Update trail emitter position in the scene.
	if (m_trailEmitter != nullptr) {
		obj_set_position(m_trailEmitter->parent, GetScenePosition().vec());
	}
}

void Projectile::OnCollideWith(const Game *game, Entity *other)
{
	Entity::OnCollideWith(game, other);

	// Self destruct when hitting a non-projectile target that is not the owher of this projectile.
	if (other != m_owner &&
		other->GetType() != ENTITY_PROJECTILE) {

		Kill();

		// Spawn a hit effect.
		game->GetScene()->SpawnEffect(
			IsOwnedByPlayer() ? "projectile-hit" : "projectile2-hit", GetPosition()
		);
	}
}
