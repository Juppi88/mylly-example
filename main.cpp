#include "game.h"
#include <string.h>
#include <mylly/core/mylly.h>

// Game handler instance.
static Game *game;

// This method is called on every frame before rendering the scene.
static void MainLoop(void)
{
	game->Update();
}

static void Cleanup(void)
{
	delete game;
	game = nullptr;
}

int main(int argc, char **argv)
{
	game = new Game();

	mylly_params_t params;
	memset(&params, 0, sizeof(params));

	params.callbacks.on_loop = MainLoop;
	params.callbacks.on_exit = Cleanup;

	strcpy(params.splash.logo_path, "./textures/splash.png");

	// Initialize the engine and enter the main loop.
	if (mylly_initialize(argc, argv, &params)) {

		game->SetupGame();
		mylly_main_loop();
	}

	return 0;
}
