#include "menuscene.h"
#include "game.h"
#include "ui.h"
#include "utils.h"
#include "asteroidhandler.h"
#include <mylly/core/mylly.h>
#include <mylly/mgui/widgets/button.h>
#include <mylly/resources/resources.h>
#include <mylly/io/input.h>

// -------------------------------------------------------------------------------------------------

MenuScene::MenuScene(void) :
	Scene()
{
}

MenuScene::~MenuScene(void)
{
	input_toggle_cursor(false);
}

void MenuScene::Create(Game *game)
{
	Scene::Create(game);

	// Disable some parts of the UI when displaying the main menu.
	game->GetUI()->ToggleHUD(false);
	game->GetUI()->ToggleMainMenu(true);
}

void MenuScene::SetupLevel(Game *game)
{
	// Select a background for the menu.
	SetBackground(4);

	// Spawn some asteroids.
	m_asteroids->SpawnInitialAsteroids(game, ASTEROID_LARGE, 3);
	m_asteroids->SpawnInitialAsteroids(game, ASTEROID_MEDIUM, 5);

	// Show cursor in the menu.
	input_toggle_cursor(true);
}

void MenuScene::Update(Game *game)
{
	m_asteroids->Update(game);

	Scene::Update(game);
}

void MenuScene::OnEntityDestroyed(Game *game, Entity *entity)
{
	UNUSED(game);
	UNUSED(entity);
}
