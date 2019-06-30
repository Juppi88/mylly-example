#pragma once

#include "entity.h"

// -------------------------------------------------------------------------------------------------

class Projectile : public Entity
{
	friend class ProjectileHandler;

private:
	Projectile(void);
	virtual ~Projectile(void) override;

	void SetOwner(Entity *owner) { m_owner = owner; }

public:
	virtual void Spawn(Game *game) override;
	virtual void Destroy(Game *game) override;
	virtual void Update(Game *game) override;

	Entity *GetOwner(void) const { return m_owner; }
	bool IsOwnedByPlayer(void) const { return (m_owner != nullptr && m_owner->GetType() == ENTITY_SHIP); }
	float GetSpeed(void) const { return (IsOwnedByPlayer() ? PLAYER_SPEED : UFO_SPEED); }

	virtual void OnCollideWith(const Game *game, Entity *other) override;

private:
	static constexpr float PLAYER_SPEED = 25.0f; // Units/Sec
	static constexpr float PLAYER_LIFETIME = 1.0f; // Seconds

	static constexpr float UFO_SPEED = 12.0f; // Units/Sec
	static constexpr float UFO_LIFETIME = 2.0f; // Seconds

	Entity *m_owner = nullptr; // Entity which fired the projectile
	float m_expiresTime = 0; // Time when the projectile should self-destruct

	emitter_t *m_trailEmitter = nullptr; // Projectile trail particle emitter
};
