#pragma once
#include <Actor/Actor.h>
#include <vector>

class MiniShip : public Craft :: Actor 
{
	TYPE_DECLARATIONS(MiniShip, Actor)

public:
	MiniShip();

private:
	virtual void Tick(float deltaTime) override;

	//충돌 이벤트 함수 오버라이드
	virtual void OnCollision(const std::shared_ptr<Actor>& other) override;

	//이동 처리 함수
	void Move(float deltaTime);

private:
	float xPosition = 0.0f;
	float yPosition = 0.0f;

	float moveSpeed = 13.0f;

	std::vector<Craft::Vector2> path;
	int pathIndex = 0;
};

