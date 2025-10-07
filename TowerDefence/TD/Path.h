#pragma once
#include "vector"

class Path
{
public:

	enum class Edge {
		Top,
		Bottom,
		Left,
		Right
	};

	explicit Path(const Rectf viewport);
	
	void Draw() const;
	
	Vector2f GetNextPosition(int nextPos) const;

	int GetSize() const;

	Vector2f GetRandomPointOnEdge(Edge edge);
	
	std::vector<Vector2f> GenerateRandomPath();

	Vector2f GetFirstPosition() const;

	Edge GetEdge() const;

private:

	Edge m_Edge;

	Rectf m_ViewPort;

	std::vector<Vector2f> m_Waypoints;
};

