#include "ui.h"
#include "utils.h"
#include "game.h"
#include <mylly/core/time.h>
#include <mylly/core/mylly.h>
#include <mylly/mgui/widget.h>
#include <mylly/mgui/widgets/panel.h>
#include <mylly/resources/resources.h>
#include <mylly/scene/sprite.h>

// -------------------------------------------------------------------------------------------------

constexpr colour_t UI::LABEL_COLOUR;
constexpr colour_t UI::HOVERED_LABEL_COLOUR;
constexpr colour_t UI::BACKGROUND_PANEL_COLOUR;
constexpr colour_t UI::BUTTON_COLOUR;
constexpr colour_t UI::BUTTON_HOVER_COLOUR;
constexpr colour_t UI::BUTTON_PRESS_COLOUR;

// -------------------------------------------------------------------------------------------------

UI::UI(void)
{
}

UI::~UI(void)
{
	// Only have to destroy the parent panels as destroy calls are recursive.
	widget_destroy(m_hudPanel);
	widget_destroy(m_mainMenuPanel);
	widget_destroy(m_pauseMenuPanel);
	widget_destroy(m_controlsPanel);
}

void UI::Create(Game *game)
{
	// Create different menus.
	CreateHUD(game);
	CreateMainMenu(game);
	CreatePauseMenu(game);
	CreateControlsMenu(game);

	// Hide HUD and help/level labels until they're acticated.
	ToggleHUD(false);
	ToggleMainMenu(false);
	TogglePauseMenu(false);
	ToggleControlsMenu(false);
}

void UI::Update(void)
{
	// Scroll score.
	if (IsUpdatingScore()) {

		float time = get_time().real_time;

		if (time < m_scoreCounterEnds) {

			float t = 1.0f - (m_scoreCounterEnds - time) / SCORE_COUNTER_DURATION;
			m_currentScore = m_previousScore + (uint32_t)(t * (m_targetScore - m_previousScore));
		}
		else {
			m_currentScore = m_targetScore;
			m_scoreCounterEnds = 0;
		}

		widget_set_text(m_scoreLabel, "%u", m_currentScore);
	}

	// Fade out the level label.
	if (IsFadingLevelLabel()) {

		float time = get_time().real_time;
		colour_t colour = LABEL_COLOUR;

		if (time < m_levelFadeEnds) {

			float t = (m_levelFadeEnds - time) / LEVEL_DURATION;

			if (t < 0.3f) {

				t = t / 0.3f;
				colour.a = (uint8_t)(t * colour.a);

				widget_set_text_colour(m_levelLabel, colour);
				widget_set_text_colour(m_helpLabel, colour);
			}
		}
		else {
			m_levelFadeEnds = 0;

			widget_set_visible(m_levelLabel, false);
			widget_set_visible(m_helpLabel, false);
		}
	}
}

void UI::ToggleHUD(bool isVisible)
{
	widget_set_visible(m_hudPanel, isVisible);

	if (!isVisible) {

		widget_set_visible(m_levelLabel, false);
		widget_set_visible(m_helpLabel, false);
	}
}

void UI::TogglePauseMenu(bool isVisible)
{
	widget_set_visible(m_pauseMenuPanel, isVisible);
}

void UI::ToggleMainMenu(bool isVisible)
{
	widget_set_visible(m_mainMenuPanel, isVisible);
}

void UI::ToggleControlsMenu(bool isVisible)
{
	widget_set_visible(m_controlsPanel, isVisible);
}

void UI::SetScore(uint32_t amount)
{
	m_currentScore = amount;
	m_targetScore = amount;
	m_previousScore = amount;

	widget_set_text(m_scoreLabel, "%u", m_targetScore);
}

void UI::AddScore(uint32_t amount)
{
	m_previousScore = m_currentScore;
	m_targetScore = m_targetScore + amount;
	m_scoreCounterEnds = get_time().real_time + SCORE_COUNTER_DURATION;
}

void UI::SetShipCount(uint32_t ships)
{
	if (ships > LENGTH(m_shipSprites)) {
		ships = LENGTH(m_shipSprites);
	}

	for (uint32_t i = 0; i < LENGTH(m_shipSprites); i++) {
		widget_set_visible(m_shipSprites[i], (i < ships));
	}
}

void UI::ShowLevelLabel(uint32_t level)
{
	char levelText[128];
	snprintf(levelText, sizeof(levelText), "SECTOR %u", level);

	if (level == 1) {
		DisplayInfoLabels(levelText, "Destroy all the asteroids");
	}
	else {
		DisplayInfoLabels(levelText);
	}

	m_levelFadeEnds = get_time().real_time + LEVEL_DURATION;
}

void UI::ShowUnsafeRespawnLabel(void)
{
	DisplayInfoLabels("", "Unsafe to spawn");
	m_levelFadeEnds = get_time().real_time + 2;
}

void UI::ShowLevelCompletedLabel(void)
{
	DisplayInfoLabels("SECTOR CLEAR", "Press ENTER to continue");
}

void UI::ShowRespawnLabel(void)
{
	DisplayInfoLabels("SHIP DESTROYED", "Press ENTER to respawn");
}

void UI::ShowGameOverLabel(void)
{
	DisplayInfoLabels("GAME OVER", "Press ENTER to return to main menu");
}

void UI::HideInfoLabels(void)
{
	widget_set_visible(m_levelLabel, false);
	widget_set_visible(m_helpLabel, false);
}

void UI::DisplayInfoLabels(const char *levelText, const char *infoText)
{
	widget_set_visible(m_levelLabel, true);
	widget_set_visible(m_helpLabel, infoText != nullptr);

	widget_set_text_colour(m_levelLabel, LABEL_COLOUR);
	widget_set_text_colour(m_helpLabel, LABEL_COLOUR);

	widget_set_text_s(m_levelLabel, levelText);

	if (infoText != nullptr) {
		widget_set_text_s(m_helpLabel, infoText);
	}

	m_levelFadeEnds = 0;
}

void UI::CreateHUD(Game *game)
{
	UNUSED(game);
	
	m_hudPanel = widget_create(nullptr);

	widget_set_anchors(m_hudPanel,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	// Score label
	m_scoreLabel = CreateLabel(m_hudPanel, "0", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 25,
		ANCHOR_MIN, 125
	);

	// Ship icons
	sprite_t *icon = res_get_sprite("shipicons/red");
	int16_t width = (int16_t)icon->size.x;
	int16_t height = (int16_t)icon->size.y;

	for (int i = 0; i < 3; i++) {

		m_shipSprites[i] = panel_create(m_scoreLabel);

		widget_set_sprite(m_shipSprites[i], icon);
		widget_set_colour(m_shipSprites[i], LABEL_COLOUR);

		int x_offset = (i - 1) * 50;

		widget_set_anchors(m_shipSprites[i],
			ANCHOR_MIDDLE, -width / 2 + x_offset,
			ANCHOR_MIDDLE, width / 2 + x_offset,
			ANCHOR_MAX, 0,
			ANCHOR_MAX, height
		);
	}

	m_levelLabel = CreateLabel(m_hudPanel, "Sector 0", true,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, -250,
		ANCHOR_MAX, -150
	);

	m_helpLabel = CreateLabel(m_hudPanel, "", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, -150,
		ANCHOR_MAX, -100
	);
}

void UI::CreateMainMenu(Game *game)
{
	// Create a container widget for the main menu and align it to the centre of the screen.
	m_mainMenuPanel = widget_create(nullptr);
	widget_set_anchors(m_mainMenuPanel,
		ANCHOR_MIDDLE, -500,
		ANCHOR_MIDDLE, +500,
		ANCHOR_MIDDLE, -300,
		ANCHOR_MIDDLE, +300
	);

	// Create the logo.
	widget_t *logo = CreateLabel(m_mainMenuPanel, "ASTEROIDS", true,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MIN, 50
	);

	widget_set_text_font(logo, res_get_font("sofachrome rg it", 0));

	// Create menu buttons.
	widget_t *startGameButton = CreateButton(logo, "START", false,
		ANCHOR_MIDDLE, -250,
		ANCHOR_MIDDLE, 250,
		ANCHOR_MAX, 125,
		ANCHOR_MAX, 175
	);

	button_set_clicked_handler(startGameButton, OnClickedStartGame);
	widget_set_hovered_handler(startGameButton, OnMainMenuButtonHovered);
	widget_set_user_context(startGameButton, game);

	// Create menu buttons.
	widget_t *controlsButton = CreateButton(startGameButton, "CONTROLS", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 50,
		ANCHOR_MAX, 100
	);

	button_set_clicked_handler(controlsButton, OnClickedControls);
	widget_set_hovered_handler(controlsButton, OnMainMenuButtonHovered);
	widget_set_user_context(controlsButton, this);

	widget_t *exitGameButton = CreateButton(controlsButton, "EXIT", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 50,
		ANCHOR_MAX, 100
	);

	button_set_clicked_handler(exitGameButton, OnClickedExitGame);
	widget_set_hovered_handler(exitGameButton, OnMainMenuButtonHovered);
}

void UI::CreatePauseMenu(Game *game)
{
	m_pauseMenuPanel = panel_create(nullptr);

	widget_set_sprite(m_pauseMenuPanel, res_get_sprite("ui-white/textbox_02"));

	widget_set_anchors(m_pauseMenuPanel,
		ANCHOR_MIDDLE, -150,
		ANCHOR_MIDDLE, 150,
		ANCHOR_MIDDLE, -180,
		ANCHOR_MIDDLE, 180
	);

	widget_set_colour(m_pauseMenuPanel, BACKGROUND_PANEL_COLOUR);

	widget_t *label = CreateLabel(m_pauseMenuPanel, "PAUSED", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 20,
		ANCHOR_MIN, 70
	);

	widget_set_text_colour(label, COL_WHITE);

	// Menu buttons
	widget_t *button;

	button = CreateButton(m_pauseMenuPanel, "Resume", true,
		ANCHOR_MIN, 30,
		ANCHOR_MAX, -30,
		ANCHOR_MIN, 120,
		ANCHOR_MIN, 170
	);

	widget_set_user_context(button, game);
	button_set_clicked_handler(button, OnClickedResume);

	button = CreateButton(m_pauseMenuPanel, "Main Menu", true,
		ANCHOR_MIN, 30,
		ANCHOR_MAX, -30,
		ANCHOR_MIN, 190,
		ANCHOR_MIN, 240
	);

	widget_set_user_context(button, game);
	button_set_clicked_handler(button, OnClickedMainMenu);

	button = CreateButton(m_pauseMenuPanel, "Quit", true,
		ANCHOR_MIN, 30,
		ANCHOR_MAX, -30,
		ANCHOR_MIN, 260,
		ANCHOR_MIN, 310
	);

	button_set_clicked_handler(button, OnClickedExitGame);
}

void UI::CreateControlsMenu(Game *game)
{
	UNUSED(game);

	m_controlsPanel = panel_create(nullptr);

	widget_set_sprite(m_controlsPanel, res_get_sprite("ui-white/textbox_02"));

	widget_set_anchors(m_controlsPanel,
		ANCHOR_MIDDLE, -200,
		ANCHOR_MIDDLE, 200,
		ANCHOR_MIDDLE, -200,
		ANCHOR_MIDDLE, 200
	);

	widget_set_colour(m_controlsPanel, BACKGROUND_PANEL_COLOUR);

	widget_t *titleLabel = CreateLabel(m_controlsPanel, "CONTROLS", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 30,
		ANCHOR_MIN, 80
	);

	widget_set_text_colour(titleLabel, COL_WHITE);

	// ----------

	// Control list
	widget_t *header, *control;

	header = CreateLabel(titleLabel, "Throttle", false,
		ANCHOR_MIN, 40,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 40,
		ANCHOR_MAX, 60
	);

	control = CreateLabel(header, "W", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, -40,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	widget_set_text_alignment(header, ALIGNMENT_LEFT);
	widget_set_text_alignment(control, ALIGNMENT_RIGHT);

	// ----------

	header = CreateLabel(header, "Turn", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 20,
		ANCHOR_MAX, 40
	);

	control = CreateLabel(header, "A/D", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, -30,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	widget_set_text_alignment(header, ALIGNMENT_LEFT);
	widget_set_text_alignment(control, ALIGNMENT_RIGHT);

	// ----------

	header = CreateLabel(header, "Fire", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 20,
		ANCHOR_MAX, 40
	);

	control = CreateLabel(header, "Space", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, -30,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	widget_set_text_alignment(header, ALIGNMENT_LEFT);
	widget_set_text_alignment(control, ALIGNMENT_RIGHT);

	// ----------

	header = CreateLabel(header, "Pause", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, 20,
		ANCHOR_MAX, 40
	);

	control = CreateLabel(header, "Esc", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, -30,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	widget_set_text_alignment(header, ALIGNMENT_LEFT);
	widget_set_text_alignment(control, ALIGNMENT_RIGHT);

	// ----------

	// Close button for the control list.
	widget_t *button;

	button = CreateButton(m_controlsPanel, "OK", true,
		ANCHOR_MIDDLE, -75,
		ANCHOR_MIDDLE, 75,
		ANCHOR_MAX, -80,
		ANCHOR_MAX, -30
	);

	widget_set_user_context(button, this);
	button_set_clicked_handler(button, OnClickedCloseControls);
}


widget_t *UI::CreateButton(widget_t *parent, const char *text, bool hasBackground,
                           anchor_type_t left_type, int16_t left_offset,
                           anchor_type_t right_type, int16_t right_offset,
                           anchor_type_t top_type, int16_t top_offset,
                           anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *button = button_create(parent);

	widget_set_text_s(button, text);

	widget_set_anchors(button,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	if (hasBackground) {

		widget_set_sprite(button, res_get_sprite("ui-white/button02"));
		widget_set_text_font(button, res_get_font("Oxanium-Medium", 32));
		widget_set_text_colour(button, COL_WHITE);
		button_set_colours(button, BUTTON_COLOUR, BUTTON_HOVER_COLOUR, BUTTON_PRESS_COLOUR);
	}
	else {

		widget_set_text_font(button, res_get_font("Oxanium-Medium", 48));
		widget_set_text_colour(button, LABEL_COLOUR);
	}

	return button;
}

widget_t *UI::CreateLabel(widget_t *parent, const char *text, bool isLarge,
                          anchor_type_t left_type, int16_t left_offset,
                          anchor_type_t right_type, int16_t right_offset,
                          anchor_type_t top_type, int16_t top_offset,
                          anchor_type_t bottom_type, int16_t bottom_offset)
{
	widget_t *label = label_create(parent);

	if (isLarge) {
		widget_set_text_font(label, res_get_font("Oxanium-ExtraBold", 0));
	}
	else {
		widget_set_text_font(label, res_get_font("Oxanium-Medium", 32));	
	}
	
	widget_set_text_colour(label, LABEL_COLOUR);
	widget_set_text_s(label, text);

	widget_set_anchors(label,
	                   left_type, left_offset, right_type, right_offset,
	                   top_type, top_offset, bottom_type, bottom_offset);

	return label;
}


void UI::OnMainMenuButtonHovered(widget_t *button, bool hovered)
{
	widget_set_text_colour(button, hovered ? HOVERED_LABEL_COLOUR : LABEL_COLOUR);
}

void UI::OnClickedStartGame(widget_t *button)
{
	Game *game = (Game *)button->user_context;

	if (!game->IsLoadingLevel()) {
		game->StartNewGame();
	}
}

void UI::OnClickedControls(widget_t *button)
{
	UI *self = (UI *)button->user_context;

	self->ToggleControlsMenu(true);
	self->ToggleMainMenu(false);
}

void UI::OnClickedCloseControls(widget_t *button)
{
	UI *self = (UI *)button->user_context;

	self->ToggleControlsMenu(false);
	self->ToggleMainMenu(true);
}

void UI::OnClickedExitGame(widget_t *button)
{
	UNUSED(button);

	// This will perform cleanup and exit the game immediately.
	mylly_exit();
}

void UI::OnClickedResume(widget_t *button)
{
	Game *game = (Game *)button->user_context;
	game->TogglePause();
}

void UI::OnClickedMainMenu(widget_t *button)
{
	Game *game = (Game *)button->user_context;
	game->LoadMainMenu();
}
