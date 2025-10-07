#pragma once
class Path;


class Enemy
{
public:

	Enemy(const Vector2f& position, Path* path, int hp);

	void Draw() const;

	void Update(float elapsedSec);
	void Move(float elapsedSec);

	float GetDistance() const;

	bool IsInRange(Rectf towerRange) const;

	Vector2f GetPosition() const;

	void IsHit(int damage);

	bool IsDead() const;

	bool ReachedEnd() const;

private:

	Vector2f m_Position;
	Vector2f m_NextPosition;

	int m_Size;
	float m_Speed;

	const int m_MaxLifePoints;
	int m_LifePoints;

	Path* m_Path;
	int m_CurrentWaypoint{ 1 };

	bool m_ReachedEnd;
};

