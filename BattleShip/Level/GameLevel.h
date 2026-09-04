#pragma once
#include <Level/Level.h>
#include <Actor/Actor.h>
#include <Algorithm/AStar.h>
#include <vector>

class GameLevel : public Craft :: Level
{
	TYPE_DECLARATIONS(GameLevel, Level)


public:
	GameLevel();

	//경로 찾는 함수
	std::vector<Craft::Vector2> FindPath(const Craft::Vector2& startPosition, const Craft::Vector2& goalPosition);

private:
	//초기화 이벤트 함수 오버라이드
	virtual void OnInitialized() override;

	virtual void Tick(float deltaTime) override;

	virtual void Draw() override;

	std::string StringMap(const std::vector<int>& row);

private:
	//그리드 맵
	std::vector<std::vector<int>> grid;
	//A*객체 생성
	AStar aStar;
};

