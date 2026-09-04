#include "AStar.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#define NOMINMAX
#include <Windows.h>

using namespace Craft;
AStar::AStar()
{
}

AStar::~AStar()
{
}

//void AStar::Tick(float deltaTime)
//{
//	elapsedTime += deltaTime;
//	elapsedTime = elapsedTime >= timeInterval ? timeInterval : elapsedTime;
//}

std::vector<Vector2> AStar::FindPath(const Vector2& startPosition, const Vector2& goalPosition, std::vector<std::vector<int>> grid)
{
	//이전 탐색 결과 초기화
	Clear();

	if (!IsValidGrid(grid))
	{
		//유효하지 않으면 빈 배열 반환
		return { };
	}

	//시작 위치/ 목표 위치가 grid 기준에서 문제 없는 위치 값인지 확인.
	if (!IsInRange(startPosition.x, startPosition.y, grid)
		|| !IsInRange(goalPosition.x, goalPosition.y, grid))
	{
		return { };
	}

	//이전 탐색 과정의 시각화 제거(기존에 방문 처리한 값)
	ClearVisualization(grid);

	startNode = CreateNode(startPosition);
	goalNode = CreateNode(goalPosition);

	//시작 노드 비용 계산 및 openList에 추가해 탐색 시작.
	startNode->gCost = 0.0f;
	startNode->hCost = CalculateHeuristic(startPosition, goalPosition);
	startNode->fCost = startNode->gCost + startNode->hCost;

	openList.emplace_back(startNode);

	const float diagonalCost = 1.41421f;
	const std::vector<Direction> directions =
	{
		{0, -1, 1.0f}, {0, 1, 1.0f},
		{-1, 0, 1.0f}, {1, 0, 1.0f},
		{-1, -1, diagonalCost },
		{ 1, -1, diagonalCost },
		{ -1, 1, diagonalCost },
		{  1, 1, diagonalCost }
	};

	//openList가 빌 때까지 탐색 반복
	while (!openList.empty())
	{
		//openList에서 fCost가 가장 작은 노드를 선택
		//이진힙을 사용하면 최적화 가능.
		Node* currentNode = openList[0];
		for (Node* node : openList)
		{
			//더 작은 비용의 노드 검색
			if (node->fCost < currentNode->fCost
				|| (node->fCost == currentNode->fCost
					&& node->hCost < currentNode->hCost))
			{
				currentNode = node;
			}
		}

		//목표 노드인지 확인.
		if (IsDestination(currentNode))
		{
			//이동 경로 제작 후 반환
			return ConstructPath(currentNode);
		}

		//현재 노드 openList에서 제거
		//방문 처리 위해
		auto iterator = std::find(openList.begin(), openList.end(), currentNode);

		//검색에 성공?
		if (iterator != openList.end())
		{
			openList.erase(iterator);
		}

		closedList.emplace_back(currentNode);

		//현재 위치를 기준으로 주변(8방향) 이웃노드 탐색
		for (const Direction& direction : directions)
		{
			//현재 노드 기준으로 인접한 노드의 좌표계산
			//새로운 좌표(위치) = 현재 위치 + 이동 방향.
			int newX = currentNode->position.x + direction.x;
			int newY = currentNode->position.y + direction.y;

			//예외 처리
			if (!IsInRange(newX, newY, grid))
			{
				continue;
			}

			//새로운 위치가 장애물인지 확인.
			if (grid[newY][newX] == (int)TileType::Rock)
			{
				continue;
			}

			if (IsDiagonalBlocked(currentNode->position, direction, grid))
			{
				continue;
			}

			if (IsInClosedList(newX, newY))
			{
				continue;
			}

			//현재 노드를 거쳐서 새로운 위치로 가는데 드는 비용 계산.
			float newGCost = currentNode->gCost + direction.cost;

			//이미 openList에 있는데 비용면에서 더 나은지 확인
			Node* openNode = FindOpenNode(newX, newY);

			if (openNode)
			{
				//비용 비교
				if (newGCost < openNode->gCost)
				{
					openNode->gCost = newGCost;
					openNode->fCost = openNode->gCost + openNode->hCost;
					openNode->parent = currentNode;
				}

				continue;
			}

			//이웃 노드 생성 및 openList에 추가.
			Node* neighborNode = CreateNode(Vector2(newX, newY), currentNode);

			//새로운 노드의 비용 계산
			neighborNode->gCost = newGCost;
			neighborNode->hCost = CalculateHeuristic(
				neighborNode->position, goalNode->position);
			neighborNode->fCost = neighborNode->gCost + neighborNode->hCost;

			//새로운 노드를 openList에 추가.
			openList.emplace_back(neighborNode);

			//옵션: 시각화
			if (grid[newY][newX] == (int)TileType::Water)
			{
				grid[newY][newX] = (int)TileType::Visited;
			}


			/*float delay = 0.0f;
			Tick(delay);*/
		}
	}
	return std::vector<Vector2>();
}

void AStar::DisplayGridWithPath(std::vector<std::vector<int>>& grid, const std::vector<Vector2>& path)
{
	//기존에 시각화를 위해 사용한 값 복구
	ClearVisualization(grid);

	static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int green = FOREGROUND_GREEN;

	//이동 경로 그리기
	for (const Vector2& position : path)
	{
		//경로 위치의 타일 값 읽기.
		int value = grid[position.y][position.x];

		//시작/목표 위치는 경로 표시에서 건너뛰기.
		if (value == (int)TileType::Start
			|| value == (int)TileType::Goal)
		{
			continue;
		}

		COORD cursorPosition;
		cursorPosition.X = static_cast<short>(position.x * 2);
		cursorPosition.Y = static_cast<short>(position.y);

		//커서 이동.
		SetConsoleCursorPosition(handle, cursorPosition);

		//텍스트 색 지정
		SetConsoleTextAttribute(handle, green);

		//글자 출력
		std::cout << "* ";
		
		//Tick마다 움직임
		/*float delay = 0.0f;
		Tick(delay);*/
	}
}

void AStar::Clear()
{
	openList.clear();
	closedList.clear();

	startNode = nullptr;
	goalNode = nullptr;
}

Node* AStar::CreateNode(const Vector2& position, Node* parent)
{
	//노드를 생성하고 allocateNodes에 추가.
	auto newNode = std::make_unique<Node>(position, parent);
	//외부 탐색
	Node* outNode = newNode.get();

	//이동 처리
	allocatedNodes.emplace_back(std::move(newNode));
	
	return outNode;
}

std::vector<Vector2> AStar::ConstructPath(Node* destination)
{
	//목표 노드로부터 부모 노드를 따라 경로 역추적.
	std::vector<Vector2> path;
	Node* current = destination;

	while (current)
	{
		//현재 노드는 경로 배열에 추가
		path.emplace_back(current->position);

		//부모 노드로 이동해서 경로 역추적
		current = current->parent;
	}

	//루프가 종료되면 path에는 반대 방향의 경로 정보가 저장.
	//따라서 다시 역방향으로 뒤집기.
	std::reverse(path.begin(), path.end());
	return path;
}

float AStar::CalculateHeuristic(const Vector2& current, const Vector2& goal) const
{
	//옥타일 (8방향) 비용 계산법.
	//대각선 이동 허용시 주의사항.
	// 최대한 대각선으로 이동하고 남은 거리는 직선으로
	//-> 대각선 형태의 장애물을 뚫고 가지 못하게 막기.

	//현재 위치와 목표 위치 사이의 차이 계산.
	//std::abs ->절댓값 함수
	int diffX = std::abs(current.x - goal.x);
	int diffY = std::abs(current.y - goal.y);

	//대각선 거리와 남은 직선 거리 분리.
	//대각선 이동시 x,y모두 1칸 씩 감소하며 0이 먼저되는 값
	int diagonalDistance = (std::min)(diffX, diffY);
	//대각선 이동 후 0이 아닌 남은 칸
	int straightDistance = (std::max)(diffX, diffY) - diagonalDistance;

	//대각선 비용
	const float diagonalCost = 1.41421f;
	const float straightCost = 1.0f;

	return diagonalDistance * diagonalCost + straightDistance * straightCost;
}

bool AStar::IsValidGrid(const std::vector<std::vector<int>>& grid) const
{
	//그리드가 비어있다면 유효하지 않음.
	if (grid.empty())
	{
		return false;
	}

	size_t width = grid[0].size();
	for (const std::vector<int>& row : grid)
	{
		//앞에서 구한 행의 길이와 다른 행이 나타나면 유효하지 않음
		if (row.size() != width)
		{
			return false;
		}
	}

	//검사를 통과하면 유효함.
	return true;
}

bool AStar::IsInRange(int x, int y, const std::vector<std::vector<int>>& grid) const
{
	//grid의 가로 크기는 같다고 가정.
	return x >= 0 && x < static_cast<int>(grid[0].size())
		&& y >= 0 && y < static_cast<int>(grid.size());
}

bool AStar::IsDiagonalBlocked(const Vector2& current, const Direction& direction, const std::vector<std::vector<int>>& grid) const
{
	//이동하려는 방향에 장애물이 있는지 확인.
	//대각선 성분만 판단.
	//대각선 성분이 아니라면 판단할 필요 없음.
	//대각선 방향의 x,y 성분은 모두 0이 아니기 때문.
	if (direction.x == 0 || direction.y == 0)
	{
		return false;
	}


	//대각선으로 이동하려는 새로운 위치의 x성분과 y성분을 분해.
	int sideX = current.x + direction.x;
	int sideY = current.y + direction.y;

	if (!IsInRange(sideX, current.x, grid)
		|| !IsInRange(sideY, current.y, grid))
	{
		return { };
	}

	//대각선 이동 성분 위치 중 하나라도 장애물(벽)이 있으면 이동 불가
	return grid[current.y][sideX] == (int)TileType::Rock
		|| grid[sideY][current.x] == (int)TileType::Rock;
}

Node* AStar::FindOpenNode(int x, int y) const
{
	//같은 좌표의 노드를 OpenList에서 찾기.
	for (Node* node : openList)
	{
		//좌표비교
		if (node->position == Vector2(x, y))
		{
			return node;
		}
	}
	return nullptr;
}

bool AStar::IsInClosedList(int x, int y) const
{
	//같은 좌표가 ClosedList에 있는지 확인.
	for (Node* node : closedList)
	{
		//좌표 비교
		if (node->position == Vector2(x, y))
		{
			return true;
		}
	}
	//아직 방문하지 않은 유효한 대상
	return false;
}

bool AStar::IsDestination(const Node* node) const
{
	//두 노드 모두 null이 아니고 두 노드의 위치가 같은지 비교.
	return (node != nullptr) && (goalNode != nullptr)
		&& node->position == goalNode->position;
}

void AStar::ClearVisualization(std::vector<std::vector<int>>& grid) const
{
	//탐색 후보 표시해둔 것을 다시 원상 복구
	//탐색 후보로 표시해뒀다는 건 원래 이동 가능한 노드
	for (std::vector<int>& row : grid)
	{
		for (int& value : row)
		{
			if (value == (int)TileType::Visited)
			{
				//숫자로 0
				value = (int)TileType::Water;
			}
		}
	}
}


