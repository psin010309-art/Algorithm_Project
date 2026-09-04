#pragma once
#include <Actor/Actor.h>
#include <Core/Core.h>
#include <Core/CraftObject.h>
#include <memory> //std:;unique_ptr / std::shared_ptr 사용.
#include <vector> //std::vector 동적 배열.

namespace Craft 
{
	//게임에 배치된 모든 액터를 관리하는 클래스.
	//public std::enable_shared_from_this<Level>
	// : shared_from_this() / weak_from_this<Level>
	// : shared_from_this() - this pointer를 shared_ptr로 변환.
	// : weak_from_this() - this 포인터를 weak__ptr로 변환.
	class CRAFT_API Level : public CraftObject, public std::enable_shared_from_this<Level>
	{
		//커스텀 타입 설정.
		TYPE_DECLARATIONS(Level, CraftObject)

		//friend 선언.
		friend class Engine;

	public:
		Level();
		virtual ~Level(); //레벨이 완전히 생성이 된 후에 초기화 함수가 진행이 되어야함

		//초기화 함수.
		virtual void OnInitialized(); // 게임 플레이 이벤트에 알리는 역할 

		//게임 플레이 이벤트 함수. 
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		//액터 추가 함수(템플릿).
		template<typename T, typename ...Args,
			typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>//T라고 하는 타입의 부모가 액터인지를 확인, T타입이 Actor의 하위이면 True, 아니면 False ->SFINE  False일 경우 함수 삭제
		std::shared_ptr<T> SpawnActor(Args&&...args) // 외우기 ->템플릿에서 가변인자 처리하는 것
		{
			//새로운 액터 생성.
			std::shared_ptr<T> newActor
				= std::make_shared<T>(std::forward<Args>(args)...);

			//추가 요청 목록에 포함.
			addRequestedActorList.emplace_back(newActor);
			
			//오너십 설정. -> Actor가 자신을 소유하는게 누군지 관계를 설정. 
			// Level이 Actor를 소유 /오너십을 타고 자신이 누구의 소유인지를 확인함.
			//SpawnActor시 딱 한 번만 수행
			//TestActor가 게임 도중 자기 자신을 삭제or 자기가 속한 레벨의 다른 액터들을 찾아야 할 때
			//자신이 지금 어떤 레벨에서 움직이는지 알아야하기 때문에 필요.
			//Actor가 부모인 레벨에 접근하기 위한 신분증.
			newActor->SetOwner(weak_from_this()); // Level 입장에서는 자신을 생성한 것이기 때문에 this

			//생성한 액터 반환
			return newActor;
		}

		//액터 검색 함수(템플릿).
		template<typename T, 
			typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>//T라고 하는 타입의 부모가 액터인지를 확인, T타입이 Actor의 하위이면 True, 아니면 False ->SFINE  False일 경우 함수 삭제
		std::shared_ptr<T> FindActor() //특정 몬스터 타입을 검색
		{
			//검색 - 형변환
			for (const auto& actor : actorList)
			{
				//T타입으로 형변환 시도.
				//T타입이 아닌 경우에는 null반환.
				std::shared_ptr<T> targetActor
					= std::dynamic_pointer_cast<T>(actor);
				if (targetActor)
				{
					return targetActor;
				}

			}
			//못찾은 경우 null 반환
			return nullptr;
		}
			

		//Getter
		inline  bool HasInitialized() const { return hasInitialized; }

	protected:
		//이전 프레임에 추가/제거 요청된 액터 처리 함수.
		void ProcessAddAndDestroyActors();

		//액터의 이전 상태 처리 함수
		void SavePreviousActorStates();

	protected:
		//초기화 처리 여부 플래그.
		bool hasInitialized = false;

		//레벨에 배치된 모든 액터.
		std::vector<std::shared_ptr<Actor>> actorList; // 소유권 이전 가능한 포인터
		
		//레벨에 추가 요청된 액터를 저장해두는 목록.
		//현재 프레임을 처리하는 과정에서 액터 추가 요청이 발생하면,
		//해당 액터를 바로 추가하면 기존 액터 처리에 문제가 발생할 수 있어서
		//현재 프레임을 모두 처리한 후에 추가 요청된 액터를 actorList로
		//옮김
		std::vector<std::shared_ptr<Actor>> addRequestedActorList;
	};
}

