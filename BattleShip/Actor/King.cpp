#include "King.h"
#include <Math/Vector2.h>
#include <Engine/Engine.h>
#include <Input/Input.h>
#include <Level/GameLevel.h>
#include <Algorithm/AStar.h>

#include <cmath>

using namespace Craft;
King::King()
	:super("S",Vector2::Zero, Color::Green )
{
	//생성 위치 설정
	int x = (Engine::Get().GetWidth() -5);
	int y = (Engine::Get().GetHeight() / 2) - (height/2);
	SetPosition(Vector2(x, y));

	//x와 y 위치 저장.
	xPosition = static_cast<float>(x);
	yPosition = static_cast<float>(y);

	sortingOrder = 4;
}

void King::Tick(float deltaTime)
{
	//상위 계층의 Tick 호출
	super::Tick(deltaTime);

	//ESC 종료
	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		QuitGame();
	}

	//마우스 우클릭 이동
	if (Input::Get().GetKeyDown(VK_RBUTTON))
	{
		const Vector2& mousePos = Input::Get().GetMousePosition();

		//다운캐스팅
		std::shared_ptr<GameLevel> gameLevel = Cast<GameLevel>(GetOwner());
		if (!gameLevel) return;

		//다음 경로 요청
		this-> path = gameLevel->FindPath(Vector2(static_cast<int>(std::round(xPosition)), static_cast<int>(std::round(yPosition))), mousePos);
		pathIndex = 0;
		
	}

	Move(deltaTime);
}

void King::OnCollision(const std::shared_ptr<Actor>& other)
{
}

void King::Move(float deltaTime)
{
	if (path.size() == 0 || pathIndex >= path.size())
	{
		return;
	}

	//남은 거리
	float length = static_cast<float>(sqrt(pow(path[pathIndex].x - xPosition, 2) + pow(path[pathIndex].y - yPosition, 2)));

	//임계값보다 작을 시 멈춤.
	if (length < 0.1f)
	{
		//노드 중심에 정확히 정렬
		xPosition = static_cast<float>(path[pathIndex].x);
		yPosition = static_cast<float>(path[pathIndex].y);

		//인덱스 하나 증가
		pathIndex++;

		if (pathIndex >= path.size())
		{
			return;
		}
		length = static_cast<float>(sqrt(pow(path[pathIndex].x - xPosition, 2) + pow(path[pathIndex].y - yPosition, 2)));
	}

	//대각선 움직임 속도 정규화
	float NormalizedXPos = (path[pathIndex].x - xPosition) / length;
	float NormalizedYPos = (path[pathIndex].y - yPosition) / length;

	//x위치 업데이트
	//이동 거리 = 현재 위치 + (빠르기 * 시간)
	xPosition += moveSpeed * deltaTime * NormalizedXPos;
	//y위치 업데이트
	yPosition += moveSpeed * deltaTime * NormalizedYPos;


	//화면 왼쪽 벗어나지 않게
	if (xPosition < 0)
	{
		xPosition = 0.0f;
	}

	//화면 오른쪽 벗어나지 않게
	//너비 고정.
	if (xPosition + width >= Engine::Get().GetWidth())
	{
		xPosition = static_cast<float>(Engine::Get().GetWidth() - width);
	}

	//화면 위쪽 벗어나지 않게
	if (yPosition < 0)
	{
		yPosition = 0.0f;
	}

	//화면 아래쪽 벗어나지 않게
	if (yPosition >= Engine::Get().GetHeight())
	{
		yPosition = static_cast<float>(Engine::Get().GetHeight() - 1);
	}

	//위치 업데이트
	Vector2 newPosition(static_cast<int>(std::round(xPosition)),static_cast<int>(std::round(yPosition)));
	SetPosition(newPosition);
}


