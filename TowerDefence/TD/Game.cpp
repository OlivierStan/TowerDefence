#include "pch.h"
#include "Game.h"
#include "utils.h"
#include "Enemy.h"
#include "Path.h"
#include "Tower.h"


Game::Game( const Window& window ) 
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	InitPath();

	InitGame();
}

void Game::Cleanup( )
{
	delete m_pCurrentPath;
	m_pCurrentPath = nullptr;

	for (int i{ 0 }; i < m_pEnemies.size(); i++)
	{
		delete m_pEnemies.at(i);
		m_pEnemies.at(i) = nullptr;
	}

	for (int i{ 0 }; i < m_pTowers.size(); i++)
	{
		delete m_pTowers.at(i);
		m_pTowers.at(i) = nullptr;
	}

	delete m_pFuturePath;
	m_pFuturePath = nullptr;
}

void Game::Update( float elapsedSec )
{
	m_ShowPlacement = false;

	// Check keyboard state
	const Uint8 *pKeyboardStates = SDL_GetKeyboardState( nullptr );

	switch (m_GameState)
	{
	case Game::GameState::pauze:

		if (pKeyboardStates[SDL_SCANCODE_ESCAPE])
		{
			SetState(GameState::mainloop);
		}

		break;
	case Game::GameState::start:

		m_WaveTimer += elapsedSec;

		if (pKeyboardStates[SDL_SCANCODE_SPACE])
		{
			MakeNewWave();

			SetState(GameState::mainloop);

			m_WaveTimer = 0.f;

			m_RoundText = Texture("Round: 1", "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));
		}
		break;
	case Game::GameState::mainloop:

		m_WaveTimer += elapsedSec;

		if (pKeyboardStates[SDL_SCANCODE_ESCAPE])
		{
			SetState(GameState::pauze);
		}

		for (int t{ 0 }; t < m_pTowers.size(); t++)
		{

			for (int e{ 0 }; e < m_pEnemies.size(); e++)
			{

				if (m_pTowers.at(t)->HasEnemy())
				{
					if (!m_pTowers.at(t)->IsFocusEnemyInRange())
					{
						m_pTowers.at(t)->RemoveEnemy();
						m_pTowers.at(t)->ChangeShootState(false);
					}
				}
				else
				{
					if (m_pEnemies.at(e)->IsInRange(m_pTowers.at(t)->GetRange()))
					{
						m_pTowers.at(t)->SetShooting(m_pEnemies.at(e));
					}
				}
			}
			m_pTowers.at(t)->Update(elapsedSec);
		}

		if (m_Lives <= 0)
		{
			SetState(GameState::gameOver); 
			break;
		}

		
		for (int e{ 0 }; e < m_pEnemies.size(); e++)
		{
			if (m_WaveTimer > 3.f)
			{
				m_pEnemies.at(e)->Update(elapsedSec);
			}
			
			if (m_pEnemies.at(e)->IsDead())
			{
				delete m_pEnemies.at(e);
				m_pEnemies.erase(m_pEnemies.begin() + e);
				
				for (int t{ 0 }; t < m_pTowers.size(); t++)
				{
					m_pTowers.at(t)->RemoveEnemy();
				}
			}
			else if (m_pEnemies.at(e)->ReachedEnd())
			{
				m_Lives--;

				std::string text{ "Lives: " + std::to_string(m_Lives) };
				m_LivesText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));

				delete m_pEnemies[e];
				m_pEnemies.erase(m_pEnemies.begin() + e);

				for (int t{ 0 }; t < m_pTowers.size(); t++)
				{
					m_pTowers.at(t)->RemoveEnemy();
				}
			}
		}

		if (m_pEnemies.empty())
		{
			if (pKeyboardStates[SDL_SCANCODE_SPACE])
			{
				m_RoundNr++;

				if (m_RoundNr == 10)
				{
					m_GameState = GameState::gameOver;
				}

				m_WaveTimer = 0.f;

				if (m_RoundNr % 2 == 0)
				{
					m_MaxTowers++;
				}
				else
				{
					m_MaxRemoves++;
				}

				delete m_pCurrentPath;
				m_pCurrentPath = m_pFuturePath;


				m_pFuturePath = new Path(m_Playfield);

				MakeNewWave();

				const std::string text{ "Round: " + std::to_string(m_RoundNr)};
				m_RoundText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));
			}
		}
		break;
	case Game::GameState::gameOver:

		if (pKeyboardStates[SDL_SCANCODE_R])
		{
			m_RoundNr = 1;
			m_MaxTowers = 3;
			m_Lives = 20;
			m_WaveTimer = 0.f;

			delete m_pCurrentPath;
			m_pCurrentPath = nullptr;

			delete m_pFuturePath;
			m_pFuturePath = nullptr;

			InitPath();

			for (int i{ 0 }; i < m_pEnemies.size(); i++)
			{
				delete m_pEnemies.at(i);
			}
			m_pEnemies.clear();

			for (int i{ 0 }; i < m_pTowers.size(); i++)
			{
				delete m_pTowers.at(i);
			}
			m_pTowers.clear();

			SetState(GameState::start);

			ResetText();
		}
	
		break;
	default:
		break;
	}
}

void Game::Draw( ) const
{
	ClearBackground( );

	//tower

	int x{}, y{};
	const Uint32 pressed = SDL_GetMouseState(&x, &y);

	const float  mouseX{ static_cast<float>(x) };
	float  mouseY{ static_cast<float>(y) };

	mouseY = GetViewPort().height - mouseY;

	if (pressed == 1)
	{
		utils::SetColor(Color4f(0.f, 1.f, 0.f, 0.5f));
		utils::FillRect(mouseX - 10.f, mouseY - 10.f, 20.f, 20.f);

		utils::SetColor(Color4f(1.f, 1.f, 0.f, 0.5f));
		utils::DrawRect(mouseX - 60.f, mouseY - 60.f, 120.f, 120.f);
	}

	m_pCurrentPath->Draw();

	const Rectf m_FutureField{ 525.f, 25.f, 150.f, 150.f };

	if (m_GameState == GameState:: start || m_GameState == GameState::mainloop)
	{
		if (!m_pTowers.empty())
		{
			for (int i{ 0 }; i < m_pTowers.size(); i++)
			{
				m_pTowers.at(i)->Draw();
			}
		}
	}

	switch (m_GameState)
	{
	case Game::GameState::pauze:
		m_PauzeText.Draw(Vector2f(100.f, GetViewPort().height / 2));
		m_StartText.Draw(Vector2f(95.f, GetViewPort().height / 2 - 25.f));
		break;
	case Game::GameState::start:
		utils::SetColor(Color4f(1.f, 1.f, 1.f, 1.f));
		break;
	case Game::GameState::mainloop:
		for (int i{ 0 }; i < m_pEnemies.size(); i++)
		{
			m_pEnemies.at(i)->Draw();
		}
		break;
	case Game::GameState::gameOver:
		m_RestartText.Draw(Vector2f(100.f, GetViewPort().height / 2));
		break;
	default:
		break;
	}

	//infofield
	utils::SetColor(Color4f(0.f, 1.f, 1.f, 1.f));
	utils::FillRect(m_InfoField);

	utils::SetColor(Color4f(0.f, 0.f, 0.f, 1.f));
	utils::FillRect(m_FutureField);

	//text
	m_LivesText.Draw(Vector2f(520.f, 350.f));
	m_RoundText.Draw(Vector2f(520.f, 450.f));
	m_TowerText.Draw(Vector2f(520.f, 300.f));
	m_RemovalsText.Draw(Vector2f(520.f, 250.f));

	glPushMatrix();

	glTranslatef(525.f, 25.f, 0.f);

	glScalef(0.3f, 0.3f, 1.f);

	m_pFuturePath->Draw();

	for (int i{ 0 }; i < m_pTowers.size(); i++)
	{
		m_pTowers.at(i)->Draw();
	}


	if (pressed == 1)
	{
		utils::SetColor(Color4f(0.f, 1.f, 0.f, 0.5f));
		utils::FillRect(mouseX - 10.f, mouseY - 10.f, 20.f, 20.f);

		utils::SetColor(Color4f(1.f, 1.f, 0.f, 0.5f));
		utils::DrawRect(mouseX - 60.f, mouseY - 60.f, 120.f, 120.f);
	}

	glPopMatrix();
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	const Vector2f mousePos{ static_cast<float>(e.x), static_cast<float>(e.y) };

	std::string text{};
	//std::cout << "MOUSEBUTTONDOWN event: ";
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		break;
	case SDL_BUTTON_RIGHT:

		if (m_MaxRemoves > 0)
		{
			for (int i{ 0 }; i < m_pTowers.size(); i++)
			{
				if (utils::IsPointInRect(mousePos, m_pTowers[i]->GetHitBox()))
				{
					delete m_pTowers[i];
					m_pTowers.erase(m_pTowers.begin() + i);

					m_MaxRemoves--;
				}
			}
		}
		text = "Remove: " + std::to_string(m_MaxRemoves);
		m_RemovalsText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));

		text = "Towers: " + std::to_string((m_MaxTowers)-m_pTowers.size());
		m_TowerText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));

		break;
	case SDL_BUTTON_MIDDLE:
		break;
	default: ;
	}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	std::string text{};
	//std::cout << "MOUSEBUTTONUP event: ";
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:

		if (m_GameState != GameState::gameOver)
		{
			if (e.x < 500.f)
			{
				if (m_pTowers.size() < m_MaxTowers)
				{
					m_pTowers.push_back(new Tower(Vector2f(e.x, e.y)));
				}
			}
		}

		text = "Towers: " + std::to_string((m_MaxTowers)-m_pTowers.size());
		m_TowerText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));

		break;
	case SDL_BUTTON_RIGHT:
		break;
	case SDL_BUTTON_MIDDLE:

		break;
	default: ;
	}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::InitGame()
{
	SetState(GameState::start);

	m_pTowers.reserve(15);
	//m_pEnemies.reserve(30);
}

void Game::InitPath()
{
	m_pCurrentPath = new Path(m_Playfield);

	m_pFuturePath = new Path(m_Playfield);
}

void Game::SetState(const GameState state)
{
	m_GameState = state;
}

void Game::MakeNewWave()
{
	const int enemyAmount{ 4 + 2*m_RoundNr };

	const Vector2f startPos{ m_pCurrentPath->GetFirstPosition() };
	std::vector<Vector2f> m_FirstWave{};
	
	for (int i{ 1 }; i <= enemyAmount; i++)
	{
		m_pEnemies.push_back(new Enemy(Vector2f(startPos.x + (-50.f * i), startPos.y), m_pCurrentPath, m_RoundNr));
	}
}

void Game::ResetText()
{
	std::string text{ "Round: " + std::to_string(m_RoundNr) };
	m_RoundText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));


	text = "Towers: " + std::to_string((m_MaxTowers)-m_pTowers.size());
	m_TowerText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));

	text = "Remove: 0";
	m_RemovalsText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));


	text = "Lives: " + std::to_string(m_Lives);
	m_LivesText = Texture(text, "Minecraft.ttf", 25.f, Color4f(0.f, 0.f, 0.f, 1.f));
}
