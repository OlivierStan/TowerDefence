#include "pch.h"
#include "Tower.h"
#include "utils.h"
#include "Enemy.h"

Tower::Tower(const Vector2f position)
	:m_IsShooting{ false }
	,m_Damage{ 5 }
	,m_Size{ 20.f }
	,m_ShootTimer{ 0.f }
	,m_pFocusEnemy{ nullptr }
	,m_BulletTimer{ 0.f }
	,m_IsBulletVisible{ false }
{
	m_Position = Rectf(position.x - m_Size / 2, position.y - m_Size / 2, m_Size, m_Size);
	m_Range = Rectf(position.x - m_Size*3.f, position.y - m_Size*3.f, m_Size * 6, m_Size * 6);
}

Tower::~Tower()
{
	delete m_pFocusEnemy;
	m_pFocusEnemy = nullptr;
}

void Tower::Draw() const
{
	//drawTower
	utils::SetColor(Color4f(0.f, 1.f, 0.f, 1.f));
	utils::FillRect(m_Position);

	//DrawRange
	utils::SetColor(Color4f(1.f, 1.f, 0.f, 1.f));
	utils::DrawRect(m_Range);

	//DrawLaser
	if (m_IsBulletVisible && m_pFocusEnemy != nullptr)
	{
		utils::SetColor(Color4f(1.f, 1.f, 0.f, 1.f));
		utils::DrawLine(Vector2f(m_Position.left + m_Position.width/2, m_Position.bottom + m_Position.height/2), m_pFocusEnemy->GetPosition());
	}
}

Rectf Tower::GetRange() const
{
	return m_Range;
}

Rectf Tower::GetHitBox() const
{
	return m_Position;
}

void Tower::SetShooting(Enemy* enemy)
{
	m_IsShooting = true;
	
	m_pFocusEnemy = enemy;
}

void Tower::RemoveEnemy()
{
	m_pFocusEnemy = nullptr;
}

void Tower::ChangeShootState(const bool isShooting)
{
	m_IsShooting = isShooting;
}

bool Tower::IsFocusEnemyInRange() const
{
	return utils::IsPointInRect(m_pFocusEnemy->GetPosition(), m_Range);
}

void Tower::Update(const float elapsedSec)
{
	if (m_IsShooting && m_pFocusEnemy != nullptr)
	{
		m_ShootTimer += elapsedSec;

		if (m_ShootTimer >= 0.25f)
		{
			m_pFocusEnemy->IsHit(m_Damage);
			m_ShootTimer = 0;

			m_IsBulletVisible = true;
			m_BulletTimer = 0;
		}
		if (m_IsBulletVisible)
		{
			m_BulletTimer += elapsedSec;
		}

		if (m_BulletTimer >= 0.2f)
		{
			m_IsBulletVisible = false;
		}
		
	}
	//m_pBullet->Update(elapsedSec);
}

bool Tower::HasEnemy() const
{
	return m_pFocusEnemy != nullptr;
}
