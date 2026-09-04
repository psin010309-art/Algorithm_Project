#pragma once
#include <Math/Vector2.h>

class Node : public Craft ::Vector2
{
public:
	Node(const Vector2& position, Node* parent = nullptr)
		:position(position), parent(parent)
	{
	}

public:
	//노드의 위치 정보
	Vector2 position;

	//노드의 비용 정보
	float gCost = 0.0f;
	float hCost = 0.0f;
	float fCost = 0.0f;

	//최종 경로를 역추적할 때 사용할 부모 노드
	Node* parent = nullptr;
};