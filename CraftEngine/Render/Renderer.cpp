#include <Render/Renderer.h>
#include <Render/ScreenBuffer.h>
#include <cassert>
#include <iostream>
#include <Windows.h>

//실제 사용은 include 해줘야함. 포인터 형식은 전방선언
namespace Craft
{
	// ----------------Frame-------------------//
	Renderer::Frame::Frame(int bufferCount)
	{
		//직접 관리할 2차원 배열
		charInfoArray = std::make_unique<CHAR_INFO[]>(bufferCount);
		sortingOrderArray = std::make_unique<int[]>(bufferCount);
	}
	Renderer::Frame::~Frame()
	{

	}
    
	//프레임 초기화 함수
	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		//이중 루프를 순회하면서 값 초기화.
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0;x < width; ++x)
			{
				//1차원 배열을 2차원 배열로 사용할 때
				//필요한 인덱스 좌표 변환
				const int index = (y * width) + x;

				//글자 항목 초기화.
				CHAR_INFO &info = charInfoArray[index];

				//빈문자 설정 - 기존의 설정된 값 지우기.
				info.Char.AsciiChar = ' ';

				// 색상 표기 안함.
				info.Attributes = 0;

				//그리기 순서 배열
				sortingOrderArray[index] = -1;
			}
		}
	}
	// ----------------Frame-------------------//
	//static 변수 초기화
	Renderer* Renderer::instance = nullptr;
	
	Renderer::Renderer(const Vector2& screenSize)
		:screenSize(screenSize)
	{
		//어써트
		assert(!instance && "instance should be null here");
		instance = this;

		//프레임 객체 생성.
		const int bufferCount = screenSize.x * screenSize.y;
		frame = std::make_unique<Frame>(bufferCount);

		//생성 후 프레임 지우기
		frame->Clear(screenSize);

		//이중 버퍼 구현을 위한 콘솔 버퍼 생성 및 초기화.
		screenBufferArray[0] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[0]->Clear();

		screenBufferArray[1] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[1]->Clear();

		//화면에 0번 콘솔 버퍼 활성화.
		SetConsoleActiveScreenBuffer(screenBufferArray[0]->GetBuffer());

		////콘솔 커서 안보이게 설정.
		//CONSOLE_CURSOR_INFO info;
		//GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

		////보이기 옵션을 false로 설정.
		//info.bVisible = FALSE;
		//SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	}
	Renderer::~Renderer()
	{
		instance = nullptr;

		//콘솔 창 원래대로 복구.
		SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));


        ////콘솔 커서 다시 보이게 설정(복구) 표준 핸들관리이기 때문에 지움. 직접 만들어서 관리
		//CONSOLE_CURSOR_INFO info;
		//GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

		////보이기 옵션을 true로 설정
		//info.bVisible = TRUE;
		//SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	}
	//제출
	void Renderer::Submit(
		const std::string& image, const Vector2& position, Color color, int sortingOrder)
	{
		//랜더 명령 생성 및 값 설정,
		//그리기 데이터를 한 곳에 모으면 최적화가 가능
		//한 번에 합치기, 필요없는 데이터 호출x
		RenderCommand command;
		command.image = image;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		//랜더 큐에 명령 추가
		//배열을 순회하면서 명령을 꺼내어 그리기처리.
		renderQueue.emplace_back(command);
	}
	void Renderer::Draw()
	{
		//화면(이미지/프레임) 지우기
		Clear();

		//그리기(프레임)
		DrawRenderQueue();

		//화면(이미지/프레임) 표시.
		Present();
	}
	Renderer& Renderer::Get()
	{
		// 어써트
		assert(instance && "insatance should not be null hrere");
		return *instance;
	}
	void Renderer::Clear()
	{
		//프레임 값 초기화.
		frame->Clear(screenSize);

		//콘솔 버퍼 초기화.
		GetCurrentBuffer()->Clear();
	}
	void Renderer::DrawRenderQueue()
	{
		//랜더 큐 순회하며 그리기 명령 실행
		for (const RenderCommand& command : renderQueue)
		{
			//그릴 문자값이 없으면 건너뛰기 ->예외처리
			//비어있으면 건너뛴다.
			if (command.image.empty())
			{
				continue;
			}

			//y 위치가 화면을 벗어나면 건너뛰기
			//y는 한 줄 이라는 전제에 만듦
			//y는 height 값 설정을 해줘야 함.
			if (command.position.y < 0
				|| command.position.y >= screenSize.y)
			{
				continue;
			}

			//그리려는 문자열 길이 값.
			const int length = static_cast<int>(command.image.length());

			//글자의 시작 위치.
			const int startX = command.position.x;

			//글자의 끝 위치.
			//시작 위치 + 길이
			//배열의 인덱스를 생각하면됨.
			const int endX = startX + length - 1;

			//x위치가 화면을 벗어나는지 확인.
			if (endX < 0 || startX >= screenSize.x)
			{
				continue;
			}
			
			//실제 그릴 글자의 위치 구하기.
			//시작 위치나 끝 위치가 화면에 벗어나게 걸린 상황에
			//들어온 것들은 구현해야함.
			//삼항 연산자.
			//x가 보다 작으면 ~ 크면 ~
			const int visibleStart = startX < 0 ? 0 : startX;
			const int visibleEnd
				= endX >= screenSize.x ? screenSize.x - 1 : endX;

			//문자열을 루프 순회하면서 글자를 2차원 배열에 하나씩 기록.
			for (int x = visibleStart; x <= visibleEnd; ++x)
			{
				//문자열에서 글자값을 가져올 때 사용할 인덱스.
				//command 이미지에 접근할 때 사용
				const int sourceIndex = x - startX;

				//글자 2차원 배열의 인덱스 구하기
				//1차원 배열을 2차원 배열 처럼 사용할 때: (y * width) + x
				const int index = (command.position.y * screenSize.x) + x;

				//정렬 순서를 비교해서 그릴지 말지를 판정.
				//이미 그려진 값이 우선순위가 높으면 건너뛰기.
				//같거나 새로 그리려는 값이 우선순위가 높으면 덮어쓰기.
				if (frame->sortingOrderArray[index] > command.sortingOrder)
				{
					continue;
				}

				//2차원 배열에 글자, 속성 설정.
				frame->charInfoArray[index].Char.AsciiChar
					= command.image[sourceIndex];

				frame->charInfoArray[index].Attributes
					= static_cast<DWORD>(command.color);

				//그리기 우선순위 값도 설정,
				//같은 위치일 경우 sortingOrder를 통해 우선순위 확인.
				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}

		//앞에서 설정한 2차원 배열을 콘솔에 그리기.
		GetCurrentBuffer()->Draw(frame->charInfoArray.get());

		//랜더큐 비우기.
		renderQueue.clear();

		//콘솔 색상 초기화.
		SetConsoleTextAttribute(
			GetCurrentBuffer()->GetBuffer(),
			static_cast<DWORD>(Color::White)
		);
	}
	void Renderer::Present()
	{
		//현재 순번의 콘솔 버퍼를 활성화.
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

		//인덱스 업데이트(갱신)
		//배열을 순차적으로 바꾸기 위함 0->1->0->1...
		//마법의 공식 -> One Minus..
		currentBufferIndex = 1 - currentBufferIndex;
	}
	const ScreenBuffer* const Renderer::GetCurrentBuffer() const
	{
		//스마트 포인터에서 원시 포인터로 다룰 시 .get을 써서 가능
		//의도치 않게 바깥에서 포인터를 소멸시킬 수 있음.
		//유니크 포인터 참조로 반환도 가능.
		return screenBufferArray[currentBufferIndex].get();
	}
}