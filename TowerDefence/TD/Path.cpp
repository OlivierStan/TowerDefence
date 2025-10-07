#include "pch.h"
#include "Path.h"
#include "utils.h"
#include "set"

Path::Path(const Rectf viewport)
	:m_Edge{ },
	m_ViewPort{ viewport }
{
	m_Waypoints = GenerateRandomPath();
}

void Path::Draw() const
{
	utils::SetColor(Color4f(0.f, 0.f, 1.f, 1.f));
	for (int i{ 1 }; i < m_Waypoints.size(); i++)
	{
		utils::DrawLine(m_Waypoints[i - 1], m_Waypoints[i], 5.f);
	}

	utils::SetColor(Color4f(1.f, 0.f, 0.f, 1.f));
	utils::DrawEllipse(m_Waypoints[0], 20.f, 20.f);
}

Vector2f Path::GetNextPosition(const int nextPos) const
{
	return m_Waypoints.at(nextPos);
}

int Path::GetSize() const
{
	return m_Waypoints.size();
}

Vector2f Path::GetRandomPointOnEdge(Edge edge)
{
	return Vector2f(0, (rand() % 4 + 1) * 100 );
}

std::vector<Vector2f> Path::GenerateRandomPath()
{
	constexpr int maxSteps{ 10 };
	constexpr int stepSize{ 100 };

	//select edges
	m_Edge = static_cast<Edge>(rand() % 4);

	//get a random position on the edge
	const Vector2f startPos = GetRandomPointOnEdge(m_Edge);
	const float endX = m_ViewPort.width;

	std::vector<Vector2f> path;

	//std::set<Vector2f> visitedPos;

	std::set<std::pair<int, int>> visitedPos;

	Vector2f currentPos = startPos;

	path.push_back(currentPos);

	currentPos = Vector2f(currentPos.x + stepSize, currentPos.y);
	path.push_back(currentPos);

	visitedPos.insert({ static_cast<int>(currentPos.x / stepSize), static_cast<int>(currentPos.y / stepSize) });

	for (int i{ 0 }; i < maxSteps; i++)
	{
		
		Vector2f nextPos = currentPos;

		//moving closer to end

		if (i % 2 == 0)
		{
			if (rand() % 2 == 0)
			{
				nextPos.y += stepSize;
			}
			else
			{
				nextPos.y -= stepSize;
			}
		}
		else
		{
			nextPos.x += stepSize;
		}

		//clamp 50 from edge

		if (nextPos.y < stepSize)
		{
			nextPos.y = stepSize;
		}
		else if (nextPos.y > m_ViewPort.height - stepSize)
		{
			nextPos.y = m_ViewPort.height - stepSize;
		}
		
		std::pair<int,int> gridPos = std::make_pair( static_cast<int>(nextPos.x / stepSize), static_cast<int>(nextPos.y / stepSize) );

		const Vector2f tempPos{ nextPos };

		while (visitedPos.count(gridPos) != 0)
		{
			nextPos = tempPos;

			if (rand() % 2 == 0)
			{
				if (rand() % 2 == 0)
				{
					nextPos.y += stepSize;
				}
				else
				{
					nextPos.y -= stepSize;
				}
			}
			else
			{
				nextPos.x += stepSize;
			}

			if (nextPos.y < stepSize)
			{
				nextPos.y = stepSize;
			}
			else if (nextPos.y > m_ViewPort.height - stepSize)
			{
				nextPos.y = m_ViewPort.height - stepSize;
			}

			gridPos = std::make_pair(static_cast<int>(nextPos.x / stepSize), static_cast<int>(nextPos.y / stepSize));
		}
		
		visitedPos.insert(gridPos);
		path.push_back(nextPos);
		currentPos = nextPos;


		if (currentPos.x == endX || currentPos.x > endX)
		{
			break;
		}
	}

	return path;
}

Vector2f Path::GetFirstPosition() const
{
	return m_Waypoints.at(0);
}

Path::Edge Path::GetEdge() const
{
	return m_Edge;
}
