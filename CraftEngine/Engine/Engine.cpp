#include "Engine.h"
#include <Level/Level.h>
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <Physics/CollisionSystem.h>
#include <Util/Util.h>
#include <SoundSystem/Sound.h>
//#include <chrono> -> 시간 관련된 여러가지 함수 but 언리얼, 유니티에서는 사용안함.
#include <iostream>
#include <Windows.h>
//#include <cstdint> //c라이브러리다
#include <cassert>
//#include <assert.h>도 같다.

namespace Craft
{
	//전역변수 초기화
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		//instance 초기화 
		//1번 싱글톤 중복 생성 검증
		assert(!instance && "instance is not null"); //여기서는 null이여야함.  
		//if(!instance) == if(instance == nullptr) -> !는 not -> boolean값을 뒤집음 -> 주솟값이 0이면 false이고 0이 아니면 True  => 만약 0이라면 False인데 !로 인해 True가 되므로 두개가 같은 것.
		instance = this; //전역 접근 통로 열기

		//엔진 설정 로드.
		LoadEngineSetting(); // 2번 txt 파일 읽기

		//랜덤 시드 설정.
		Util::SetRandomSeed();

		//입력 객체 생성.
		//3번 키보드 입력 관리 객체를 메모리에 할당.
		input = std::make_unique<Input>();

		//랜더러 객체 생성.
		//4번 Renderer생성자 연쇄호출.
		renderer = std::make_unique<Renderer>(
			Vector2 (setting.width, setting.height)
		);

		//콜리전 시스템 객체 생성.
		collisionSystem = std::make_unique<CollisionSystem>();

		//사운드 객체 생성.
		sound = std::make_unique<Sound>();
	}
	Engine::~Engine()
	{
		instance = nullptr;
	}
	void Engine::Run()
	{
		//고해상도 타이머 사용.

		//밀리세컨드 - 1/1000 초 -> 해상도: 1000.
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		//현재 시간 읽기.
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);

		//프레임 계산을 위한 변수
		int64_t current = counter.QuadPart;
		int64_t previous = current;

		//고정 프레임으로 만들기 위한 값.
		float oneFrameTime = 1.0f / setting.framerate;

		//엔진 루프.
		while (true)
		{
			//종료 조건 처리.
			if (isQuit)
			{
				break;
			}
			//프레임 처리.

			//입력 처리.
			ProcessInput();

			//프레임 시간 계산.
			// 1. 현재 시간 읽기.
			QueryPerformanceCounter(&counter);
			current = counter.QuadPart;

			//2.(현재 시간 - 이전 시간) / 시간 단위(해상도).
			// -> 초단위로 변환.
			//예) 밀리세컨드(1/1000초).  200밀리세컨드 -> 200/1000 -> 0.2초
			float deltaTime
				= static_cast<float>(current - previous)
				/ static_cast<float>(frequency.QuadPart);

			//고정 프레임 처리.
			//프레임 사이에 걸린 시간이 목표 시간보다 더 많이 지났으면
			//프레임 처리.
			if (deltaTime >= oneFrameTime)
			{
				//게임 이벤트 함수 호출.
				OnInitialized();

				//게임 이벤트의 초기화 함수(1번만 호출).
				BeginPlay();

				//게임 업데이트 함수.
				Tick(deltaTime);

				//충돌 처리.
				ProcessCollision();

				//화면 그리기.
				Draw();

				//이 위까지 호출이 완료되면 프레임 처리 완료됨.

				//레벨 전환 처리.
				if (nextLevel) // = (!nullptr)
				{
					//기존 레벨 정리. null이 아닐 시
					if (mainLevel)
					{
						mainLevel.reset();
					}

					//추가 요청된 레벨을 메인 레벨로 설정,
					mainLevel = nextLevel;

					//포인터 정리.
					nextLevel.reset();
				}

				//추가/제거 요청된 액터 정리.
				if (mainLevel)
				{
					mainLevel-> ProcessAddAndDestroyActors();
					
					//액터의 이전 상태 저장 처리.
					mainLevel->SavePreviousActorStates();
				}

				//입력 상태 저장.
				SavePreviousInputStates();

				//현재 시간을 이전 시간으로 저장.
				previous = current;
			}
		}



		//종료 처리 함수 호출.
		Shutdown();

	}
	void Engine::Quit()
	{
		//엔진 종료 플래그 설정.
		isQuit = true;
	}

	void Engine::PlayOneShot(const std::string& filename)
	{
		if (!sound)
		{
			return;
		}

		//사운드 시스템 함수 호출.
		sound-> PlayOneShot(std::string("../Assets/Sound/") + filename);
	}

	void Engine::PlayBackgroundMusic(const std::string& filename)
	{
		if (!sound)
		{
			return;
		}

		//사운드 시스템 함수 호출.
		sound->PlayBackgroundMusic(std::string("../Assets/Sound/") + filename);
	}

	void Engine::StopBackgroundMusic()
	{
		if (!sound)
		{
			return;
		}

		//사운드 시스템 함수 호출.
		sound->StopBackgroundMusic();
	}
	Engine& Engine::Get()
	{   
		//검증 - 어서트(어써트/assert)
		//assert는 무조건(필수로) 통화해야하는 조건이 있을 때 사용
		//디버그 모드에서만 동작.
		assert(instance && "instance is null"); // 출력도 가능
		return *instance;
	}
	void Engine::ProcessInput()
	{
		assert(input && "input should not be null here");
		if (!input)
		{
			//input이 null이면 반환
			return;
		}

		input->ProcessInput();
	}
	void Engine::OnInitialized()
	{
		//레벨 초기화 처리.
		//예외 처리.
		if (!mainLevel || mainLevel->HasInitialized())
		{
			return;
		}

		//초기화 이벤트 호출.
		mainLevel->OnInitialized();
	}
	void Engine::BeginPlay()
	{
		if (!mainLevel) // 메인 레벨이 null이라면
		{
			return; //종료
		}
		//레벨에 이벤트 전달
		mainLevel->BeginPlay();
	}
	void Engine::Tick(float deltaTime)
	{
		if (!mainLevel)
		{
			return;
		}
		mainLevel->Tick(deltaTime);
	}
	void Engine::Draw()
	{
		if(!mainLevel)
		{
			return;
		}
		mainLevel->Draw();

		//랜더러에 Draw이벤트 호출
		if (!renderer)
		{
			return;
		}
		renderer->Draw();
	}
	void Engine::ProcessCollision()
	{
		//예외처리
		if (!mainLevel || !collisionSystem)
		{
			return;
		}
		//충돌 처리
		//의존성 주입(dependency Injection)
		collisionSystem->ProcessCollision(mainLevel->actorList);
	}
	void Engine::SavePreviousInputStates()
	{
		assert(input && "input should not be null here");
		if (!input)
		{
			return;
		}

		input->SavePreviousStates();
	}
	void Engine::Shutdown()
	{
	}
	void Engine::LoadEngineSetting()
	{
		//파일 열기.(개행 문자 처리를 쉽게 텍스트 모드로 열기
		FILE* file = nullptr;
		fopen_s(&file, "../Config/Setting.txt", "rt"); //2번으로 텍스트 파일 읽기. width, height, framerate 변수 채우기

		//예외처리
		if (!file)
		{
			std::cout << "Failed to open engine setting file.\n";
			//디버그 모드에서 강제 종단 시키는 기능.
			__debugbreak();
			return;
		}
		//데이터 읽어오기.
		const int bufferSize = 2048;
		char buffer[bufferSize] = {};

		//읽기
		size_t readSize 
			= fread(buffer, sizeof(char), bufferSize, file);

		//값 저장을 위해 서식 해석(파싱-Parsing).
		//문자열 자르기(Split).
		char* context = nullptr;
		char* token = nullptr;

		//파일에서 읽은 전체 문자열을 개행(\n) 기준으로 처음 자르기.
		//개행 기준으로 자른 왼쪽은 토큰 나머지는 컨택스트로 들어가며 이걸 반복해야함.
		token = strtok_s(buffer, "\n", &context);

		//반복해서 자르기 토큰이 null일 떄까지
		while (token)
		{
			//공백 전까지 읽은 문자열을 저장할 변수.
			char key[15] = {};

			//포맷을 지정한 문자열 읽기.
			//공백 문자를 만나면 그 전까지 읽어서 저장.
			sscanf_s(token, "%s", key, 15);

			//키 값을 비교해서 값 설정.
			if (strcmp(key, "framerate") == 0)
			{
				sscanf_s(token, "framerate = %f", &setting.framerate);
			}

			else if (strcmp(key, "width") == 0)
			{
				sscanf_s(token, "width = %d", &setting.width);
			}
			else if (strcmp(key, "height") == 0)
			{
				sscanf_s(token, "height = %d", &setting.height);
			}
			
			//나머지 문자열 자르기(개행 문자 기준)
			token = strtok_s(nullptr, "\n", &context);
		}

		//파일 닫기
		fclose(file);
		file = nullptr;

	}
}