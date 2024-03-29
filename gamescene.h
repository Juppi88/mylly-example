#include "scene.h"

// -------------------------------------------------------------------------------------------------

class GameScene : public Scene
{
public:
	GameScene(void);
	~GameScene(void);

	virtual SceneType GetType(void) const override { return SCENE_GAME; }

	virtual void Create(Game *game) override;
	virtual void SetupLevel(Game *game) override;
	virtual void Update(Game *game) override;

	void RespawnShip(Game *game);

	Ship *GetPlayerShip(void) const { return m_ship; }

	virtual void OnEntityDestroyed(Game *game, Entity *entity) override;

private:
	Ship *m_ship = nullptr;
	Ufo *m_ufo = nullptr;
	PowerUp *m_powerUp = nullptr;

	float m_playerShipSpawnTime = 0;
};
