#include "gamedefs.h"

// -------------------------------------------------------------------------------------------------

class WarpEffect
{
public:
	WarpEffect(Ship *playerShip);
	~WarpEffect(void);

	void Setup(Game* game);
	bool Update(Game* game);

private:
	Ship *m_playerShip = nullptr;
	float m_timeElapsed = 0;
	shader_t *m_bloomShader = nullptr; // Reference to bloom post processing shader

	static constexpr float EFFECT_DURATION = 0.25f;
};
