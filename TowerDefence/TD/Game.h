#pragma once
#include "BaseGame.h"
#include "vector"
#include "list"
#include "Texture.h"

class Path;
class Enemy;
class Tower;

class Game : public BaseGame
{
public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update(float elapsedSec) override;
	void Draw() const override;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e) override;
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e) override;
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e) override;
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e) override;
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e) override;

private:

	enum class GameState {
		pauze,
		start,
		mainloop,
		gameOver
	};

	GameState m_GameState{ GameState::pauze };


	// FUNCTIONS
	void Initialize();
	void Cleanup();
	void ClearBackground() const;

	void InitGame();

	void InitPath();

	void SetState(GameState state);

	void MakeNewWave();

	void ResetText();

	//MEMBERS

	Rectf m_Playfield{ 0.f, 0.f, 500.f, GetViewPort().height };

	Rectf m_InfoField{ 500.f, 0.f, 200.f, GetViewPort().height};
	
	Rectf m_FutureField{ 525.f, 25.f, 150.f, 150.f};

	int m_Lives{ 20 };

	Texture m_LivesText{ "Lives: 20", "Minecraft.ttf", 25, Color4f(0.f, 0.f, 0.f, 1.f) };
	Texture m_RoundText{ "Press \'SPACE\' to start round", "Minecraft.ttf", 12, Color4f(0.f, 0.f, 0.f, 1.f) };
	Texture m_TowerText{ "Towers: 3", "Minecraft.ttf", 25, Color4f(0.f, 0.f, 0.f, 1.f) };
	Texture m_RemovalsText{ "Remove: 1", "Minecraft.ttf", 25, Color4f(0.f, 0.f, 0.f, 1.f) };

	Texture m_PauzeText{ "PAUSE", "Minecraft.ttf", 100, Color4f(1.f, 1.f, 1.f, 1.f) };
	Texture m_StartText{ "Press 'ESC' to start again", "Minecraft.ttf", 25, Color4f(1.f, 1.f, 1.f, 1.f) };
	Texture m_RestartText{ "Press \'r\' to restart", "Minecraft.ttf", 25, Color4f(1.f, 1.f, 1.f, 1.f) };


	bool m_ShowPlacement{ false };

	int m_RoundNr{ 1 };
	int m_MaxTowers{ 3 };
	int m_MaxRemoves{ 1 };

	float m_WaveTimer{ 0.f };

	// ------level 1 positions------

	Vector2f m_FirstStartPos{ GetViewPort().width + 50, 325 };

	//path
	Path* m_pCurrentPath;
	Path* m_pFuturePath;

	// tower
	std::vector<Tower*> m_pTowers;

	//enemy
	std::vector<Enemy*> m_pEnemies;

	float m_Speed{ 150.f };
};