#include "gamescene.h"
#include "game.h"
#include "ship.h"
#include "ui.h"
#include "ufo.h"
#include "utils.h"
#include "powerup.h"
#include "asteroidhandler.h"
#include "projectilehandler.h"
#include "inputhandler.h"
#include <mylly/core/time.h>
#include <mylly/resources/resources.h>
#include <mylly/audio/audiosystem.h>

// -------------------------------------------------------------------------------------------------

GameScene::GameScene(void) :
	Scene()
{
}

GameScene::~GameScene(void)
{
	delete m_ship;

	if (m_ufo) {
		delete m_ufo;
	}
}

void GameScene::Create(Game *game)
{
	Scene::Create(game);
}

void GameScene::SetupLevel(Game *game)
{
	// Select the level's background.
	SetBackground(game->GetLevel() - 1);

	// Spawn some asteroids.
	m_asteroids->SpawnInitialAsteroids(game, ASTEROID_LARGE, 2 + game->GetLevel());

	// Fade in to start the level.
	FadeCamera(true);

	// Display the in-game HUD.
	game->GetUI()->ToggleHUD(true);
	game->GetUI()->ShowLevelLabel(game->GetLevel());

	// Spawn the player's ship into the scene after a small delay.
	m_playerShipSpawnTime = get_time().time + 1;
}

void GameScene::Update(Game *game)
{
	if (m_playerShipSpawnTime != 0 &&
		get_time().time >= m_playerShipSpawnTime) {

		// Create the player's ship.
		m_ship = new Ship();
		m_ship->Spawn(game);

		m_playerShipSpawnTime = 0;
	}

	// Check whether a UFO should appear in the game.
	if (game->ShouldUFOSpawn() && m_ufo == nullptr) {

		Vec2 spawnPosition, spawnDirection;
		
		Utils::GetRandomSpawnPosition(game->GetBoundsMin(), game->GetBoundsMax(),
		                              spawnPosition, spawnDirection);

		m_ufo = new Ufo();
		m_ufo->Spawn(game);
		m_ufo->SetPosition(spawnPosition);

		game->ResetUFOCounter();
	}

	if (m_ship != nullptr) {

		if (m_ship->IsDestroyed()) {

			// Inform the game handler that the ship was destroyed.
			game->OnShipDestroyed();

			// Spawn an explosion effect in the ship's place.
			SpawnEffect("ship-explosion", m_ship->GetPosition());

			// Shake the camera to amplify the effect.
			game->GetScene()->ShakeCamera(1.0f, 0.5f);

			// Play an explosion sound effect.
			audio_play_sound(res_get_sound("Explosion"), 0);

			// Remove the ship from the game.
			m_ship->Destroy(game);
			m_ship = nullptr;

			// Update UI.
			game->GetUI()->SetShipCount(game->GetShips());

			if (game->GetShips() == 0) {
				game->GetUI()->ShowGameOverLabel();
			}
			else {
				game->GetUI()->ShowRespawnLabel();
			}
		}
		else {
			m_ship->ProcessInput(game);
			m_ship->Update(game);
		}
	}

	// Update UFO if it is in the game.
	if (m_ufo != nullptr) {

		if (m_ufo->IsDestroyed()) {

			// Spawn an explosion effect in the UFO's place.
			SpawnEffect("ship-explosion", m_ufo->GetPosition());

			// Remove the UFO from the game.
			m_ufo->Destroy(game);
			m_ufo = nullptr;

			// Destroying an UFO will give the player 1000 points.
			game->AddScore(1000);
		}
		else {
			m_ufo->Update(game);
		}
	}

	// Update uncollected powerup.
	if (m_powerUp != nullptr) {

		if (m_powerUp->IsDestroyed()) {

			// Inform the game handler that the player collected the powerup.
			game->OnPowerUpCollected();

			m_powerUp->Destroy(game);
			m_powerUp = nullptr;
		}
		else {
			m_powerUp->Update(game);
		}
	}

	m_asteroids->Update(game);
	m_projectiles->Update(game);

	// Godmode button for testing: destroy all asteroids.
	if (game->GetInputHandler()->IsPressingGodmodeButton()) {
		m_asteroids->DestroyAllAsteroids(game);
	}

	// Complete the level when all asteroids and the UFO have been destroyed.
	if (m_asteroids->AllAsteroidsDestroyed() &&
		m_ufo == nullptr &&
		!game->IsLevelCompleted()) {

		game->OnLevelCompleted();
		game->GetUI()->ShowLevelCompletedLabel();
	}

	Scene::Update(game);
}

void GameScene::RespawnShip(Game *game)
{
	if (m_ship != nullptr) {
		return;
	}

	m_ship = new Ship();
	m_ship->Spawn(game);

	game->GetUI()->HideInfoLabels();
}

void GameScene::OnEntityDestroyed(Game *game, Entity *entity)
{
	if (entity->GetType() == ENTITY_ASTEROID ||
		entity->GetType() == ENTITY_UFO) {

		// Spawn a powerup crate if one isn't in the game yet and the player has earned it.
		if (m_powerUp == nullptr &&
			game->HasPlayerEarnedPowerUp()) {

			m_powerUp = new PowerUp();
			m_powerUp->Spawn(game);

			m_powerUp->SetPosition(entity->GetPosition());
		}
	}
}
