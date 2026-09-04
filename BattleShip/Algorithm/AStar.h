#pragma once

#include "Node.h"
#include <vector>
#include <memory>

class AStar : public Craft ::Vector2
{
private:
	// 이동 방향과 해당 방향의 이동 비용.
	struct Direction
	{
		Direction()
		{
		}

		Direction(int x, int y, float cost)
			: x(x), y(y), cost(cost)
		{
		}

		int x = 0;
		int y = 0;
		float cost = 0.0f;
	};

	// 그리드 타일의 종류(열거형).
	// char 타입 -> 문자 타입 -> 숫자.
	// 문자 타입으로 활용 / 1byte 크기를 할당하고 싶을 때 많이 활용.
	enum class TileType : char
	{
		Water = 0,
		Rock = 1,
		Start = 2,
		Goal = 3,
		Visited = 5,
	};

public:
	AStar();
	~AStar();

	// 메시지(객체 지향 프로그래밍 용어) - 공개 메소드(인터페이스).

	// 경로 탐색 요청 함수.
	std::vector<Vector2> FindPath(
		const Vector2& startPosition,
		const Vector2& goalPosition,
		std::vector<std::vector<int>> grid);

	// 탐색한 최종 경로를 출력하는 함수.
	void DisplayGridWithPath(
		std::vector<std::vector<int>>& grid,
		const std::vector<Vector2>& path);

	//Tick마다 이동이 진행되게
	//void Tick(float deltaTime);

private:
	// 메소드(클래스(또는 구조체)가 가지는 함수/ 공개되지 않은 함수).

	// 이전 탐색에 사용한 정보 및 노드를 정리하는 함수.
	void Clear();

	// 노드 생성 전문 함수.
	Node* CreateNode(const Vector2& position, Node* parent = nullptr);

	// 부모 노드를 역추적해서 경로를 구성하는 함수.
	std::vector<Vector2> ConstructPath(Node* destination);

	// 휴리스틱(추정 비용) 계산 함수.
	float CalculateHeuristic(const Vector2& current, const Vector2& goal) const;

	// 그리드 검증 함수.
	bool IsValidGrid(const std::vector<std::vector<int>>& grid) const;

	// 이동하려는 좌표가 유효한지 확인.
	bool IsInRange(int x, int y, const std::vector<std::vector<int>>& grid) const;

	// 옵션: 대각선 이동이 장애물 모서리를 통과하는지 확인.
	bool IsDiagonalBlocked(
		const Vector2& current,
		const Direction& direction,
		const std::vector<std::vector<int>>& grid) const;

	// Open목록 및 Closed 목록 검사 함수.
	Node* FindOpenNode(int x, int y) const;
	bool IsInClosedList(int x, int y) const;

	// 목적지 인지 확인하는 함수.
	bool IsDestination(const Node* node) const;

	// 이전 탐색 표시를 지우고 그리드를 출력하는 함수.
	void ClearVisualization(std::vector<std::vector<int>>& grid) const;


private:
	//메모리 관리를 위해 사용
	std::vector<std::unique_ptr<Node>> allocatedNodes;

	//탐색할 노드 목록/ 탐색을 마친 노드 목록
	std::vector<Node*> openList;
	std::vector<Node*> closedList;

	//시작/목표 노드
	Node* startNode = nullptr;
	Node* goalNode = nullptr;

	float elapsedTime = 0.0f;
	float timeInterval = 0.05f;
};

