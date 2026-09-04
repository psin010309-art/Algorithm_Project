#include <Engine/Engine.h>
#include <Level/GameLevel.h>

using namespace Craft;
int main()
{
	//엔진 객체 생성 및 실행
	Craft::Engine engine;
	engine.AddNewLevel<GameLevel>();
	engine.Run();
}