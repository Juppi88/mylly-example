#pragma once

#include "gamedefs.h"
#include <mylly/mgui/widget.h>

// -------------------------------------------------------------------------------------------------

class UI
{
public:
	UI(void);
	~UI(void);

	void Create(Game *game);
	void Update(void);

	void ToggleHUD(bool isVisible);
	void ToggleMainMenu(bool isVisible);
	void TogglePauseMenu(bool isVisible);
	void ToggleControlsMenu(bool isVisible);

	void SetScore(uint32_t amount);
	void AddScore(uint32_t amount);

	void SetShipCount(uint32_t ships);

	void ShowLevelLabel(uint32_t level);
	void ShowUnsafeRespawnLabel(void);
	void ShowLevelCompletedLabel(void);
	void ShowRespawnLabel(void);
	void ShowGameOverLabel(void);

	void HideInfoLabels(void);

private:
	void CreateHUD(Game *game);
	void CreateMainMenu(Game *game);
	void CreatePauseMenu(Game *game);
	void CreateControlsMenu(Game *game);

	bool IsUpdatingScore(void) const { return (m_scoreCounterEnds != 0); }
	bool IsFadingLevelLabel(void) const { return (m_levelFadeEnds != 0); }

	void DisplayInfoLabels(const char *levelText, const char *infoText = nullptr);

	// UI helpers for quickly creating UI widgets.
	static widget_t *CreateButton(widget_t *parent, const char *text, bool hasBackground,
	                              anchor_type_t left_type, int16_t left_offset,
	                              anchor_type_t right_type, int16_t right_offset,
	                              anchor_type_t top_type, int16_t top_offset,
	                              anchor_type_t bottom_type, int16_t bottom_offset);

	static widget_t *CreateLabel(widget_t *parent, const char *text, bool isLarge,
	                             anchor_type_t left_type, int16_t left_offset,
	                             anchor_type_t right_type, int16_t right_offset,
	                             anchor_type_t top_type, int16_t top_offset,
	                             anchor_type_t bottom_type, int16_t bottom_offset);

	static void OnMainMenuButtonHovered(widget_t *button, bool hovered);
	static void OnClickedStartGame(widget_t *button);
	static void OnClickedControls(widget_t *button);
	static void OnClickedCloseControls(widget_t *button);
	static void OnClickedExitGame(widget_t *button);
	static void OnClickedResume(widget_t *button);
	static void OnClickedMainMenu(widget_t *button);
	
private:
	static constexpr colour_t LABEL_COLOUR = col_a(255, 255, 255, 120);
	static constexpr colour_t HOVERED_LABEL_COLOUR = col_a(255, 255, 255, 255);
	static constexpr colour_t BACKGROUND_PANEL_COLOUR = col_a(10, 10, 10, 175);
	static constexpr colour_t BUTTON_COLOUR = col(35, 35, 35);
	static constexpr colour_t BUTTON_HOVER_COLOUR = col(50, 50, 50);
	static constexpr colour_t BUTTON_PRESS_COLOUR = col(20, 20, 20);

	static constexpr float SCORE_COUNTER_DURATION = 1.0f;
	static constexpr float LEVEL_DURATION = 5.0f;

	widget_t *m_hudPanel = nullptr;
	widget_t *m_mainMenuPanel = nullptr;
	widget_t *m_pauseMenuPanel = nullptr;
	widget_t *m_controlsPanel = nullptr;

	widget_t *m_scoreLabel = nullptr;
	widget_t *m_levelLabel = nullptr;
	widget_t *m_helpLabel = nullptr;

	widget_t *m_shipSprites[3] = { nullptr, nullptr, nullptr };

	float m_scoreCounterEnds = 0;
	uint32_t m_currentScore = 0;
	uint32_t m_previousScore = 0;
	uint32_t m_targetScore = 0;

	float m_levelFadeEnds = 0;
};
