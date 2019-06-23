#include "scene.h"

// -------------------------------------------------------------------------------------------------

class MenuScene : public Scene
{
public:
	MenuScene(void);
	~MenuScene(void);

	virtual SceneType GetType(void) const override { return SCENE_MENU; }

	virtual void Create(Game *game) override;
	virtual void SetupLevel(Game *game) override;
	virtual void Update(Game *game) override;

	virtual void OnEntityDestroyed(Game *game, Entity *entity) override;

private:
	void CreateMainMenu(Game *game);
	
	static void OnButtonHovered(widget_t *button, bool hovered);
	static void OnSelectedStartGame(widget_t *button);
	static void OnSelectedExitGame(widget_t *button);

private:
	widget_t *m_menuPanel = nullptr;
};
