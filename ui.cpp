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

UI::UI(void)
{
}

UI::~UI(void)
{
	// Only have to destroy the parent panels as destroy calls are recursive.
	widget_destroy(m_hudPanel);
}

void UI::Create(Game *game)
{
	m_hudPanel = widget_create(nullptr);

	widget_set_anchors(m_hudPanel,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0
	);

	// Score label
	m_scoreLabel = Utils::CreateLabel(m_hudPanel, "0", false,
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
		widget_set_colour(m_shipSprites[i], Utils::LABEL_COLOUR);

		int x_offset = (i - 1) * 50;

		widget_set_anchors(m_shipSprites[i],
			ANCHOR_MIDDLE, -width / 2 + x_offset,
			ANCHOR_MIDDLE, width / 2 + x_offset,
			ANCHOR_MAX, 0,
			ANCHOR_MAX, height
		);
	}

	m_levelLabel = Utils::CreateLabel(m_hudPanel, "Sector 0", true,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, -250,
		ANCHOR_MAX, -150
	);

	m_helpLabel = Utils::CreateLabel(m_hudPanel, "", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MAX, -150,
		ANCHOR_MAX, -100
	);

	// Create the pause menu.
	CreatePauseMenu(game);

	// Hide HUD and help/level labels until they're acticated.
	ToggleHUD(false);
	TogglePauseMenu(false);
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
		colour_t colour = Utils::LABEL_COLOUR;

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

	widget_set_text_colour(m_levelLabel, Utils::LABEL_COLOUR);
	widget_set_text_colour(m_helpLabel, Utils::LABEL_COLOUR);

	widget_set_text_s(m_levelLabel, levelText);

	if (infoText != nullptr) {
		widget_set_text_s(m_helpLabel, infoText);
	}

	m_levelFadeEnds = 0;
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

	widget_set_colour(m_pauseMenuPanel, col_a(10, 10, 10, 175));

	widget_t *label = Utils::CreateLabel(m_pauseMenuPanel, "PAUSED", false,
		ANCHOR_MIN, 0,
		ANCHOR_MAX, 0,
		ANCHOR_MIN, 20,
		ANCHOR_MIN, 70
	);

	widget_set_text_colour(label, COL_WHITE);

	// Menu buttons
	widget_t *button;

	button = Utils::CreateButton(m_pauseMenuPanel, "Resume", true,
		ANCHOR_MIN, 20,
		ANCHOR_MAX, -20,
		ANCHOR_MIN, 120,
		ANCHOR_MIN, 170
	);

	widget_set_user_context(button, game);
	button_set_clicked_handler(button, OnClickedResume);

	button = Utils::CreateButton(m_pauseMenuPanel, "Main Menu", true,
		ANCHOR_MIN, 20,
		ANCHOR_MAX, -20,
		ANCHOR_MIN, 190,
		ANCHOR_MIN, 240
	);

	widget_set_user_context(button, game);
	button_set_clicked_handler(button, OnClickedMainMenu);

	button = Utils::CreateButton(m_pauseMenuPanel, "Quit", true,
		ANCHOR_MIN, 20,
		ANCHOR_MAX, -20,
		ANCHOR_MIN, 260,
		ANCHOR_MIN, 310
	);

	button_set_clicked_handler(button, OnClickedExit);
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

void UI::OnClickedExit(widget_t *button)
{
	UNUSED(button);
	mylly_exit();
}
