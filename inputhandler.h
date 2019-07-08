#pragma once

#include "gamedefs.h"

// -------------------------------------------------------------------------------------------------

enum {
	BUTTON_FORWARD,
	BUTTON_BACKWARD,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_FIRE,
	BUTTON_CONFIRM,
	BUTTON_GODMODE,

	NUM_CONTROLS
};

// -------------------------------------------------------------------------------------------------

class InputHandler
{
public:
	InputHandler(Game *game);
	~InputHandler(void);

	float GetSteering(void) const;
	float GetAcceleration(void) const;

	bool IsFiring(void) const;
	bool IsPressingConfirm(void) const;
	bool IsPressingGodmodeButton(void) const;

private:
	static bool TogglePause(uint32_t key, bool pressed, void *context);
	static bool ShowEditor(uint32_t key, bool pressed, void *context);
};
