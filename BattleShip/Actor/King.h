#pragma once
#include <Actor/Actor.h>
#include <vector>


class King : public Craft :: Actor
{
	enum class FireMode
	{
		bomb = 0,
		missile = 1 
	};

	TYPE_DECLARATIONS(King, Actor)

public:
	King();

private:
	//이벤트 함수 오버라이딩
	virtual void Tick(float deltaTime) override;

	//충돌 이벤트 함수 오버라이드
	virtual void OnCollision(const std::shared_ptr<Actor>& other) override;

	//이동 처리 함수
	void Move(float deltaTime);

	//Todo: 공격 함수

private:
	//이동 처리에 필요한 변수
	float xPosition = 0.0f;
	float yPosition = 0.0f;

	//이동 속도 변수
	float moveSpeed = 5.0f;

	//A*로 산출한 경로 담을 배열
	std::vector<Craft::Vector2> path;

	//인덱스
	int pathIndex = 0;
};

