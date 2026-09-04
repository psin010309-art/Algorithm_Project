#pragma once

#include <Core/Core.h>
#include <WIndows.h>

namespace Craft
{
	class CRAFT_API Vector2
	{
	public:
		Vector2(int x = 0, int y = 0);
		//{}로 사용 시 컴파일러는 한 번 더 확인을 하게됨 -> 특수한 구문이 있나?
		//default로 할 시 자동성능 최적화 -> 그냥 일반 소멸자로서 작동하게 해줘
		//동적할당 한 객체가 없기에 일반 소멸자를 쓰는게 더 좋다.
		~Vector2() = default;

		//연산자 오버로딩

		//Windows 콘솔 좌표계로 변환하는 연산자 오버로딩
		//콘솔 좌표계는 COORD 사용
		//short 타입
		//X, Y -> 콘솔 가로 세로의 위치.
		//Vector2를 Windows 콘솔 API 함수들에 직접 넘겨서 쓰기 위해서
		operator COORD() const;
		operator COORD();

		//사칙(이항, 왼쪽 자기자신) 연산자 오버로딩.
		//연쇄 대입을 하기 위함
		//대입이 끝난 후 바로 다른 연산에 사용 -> 랜더링에 사용해서 그리기 위함.
		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;
		Vector2 operator*(const Vector2& other) const;
		Vector2 operator/(const Vector2& other) const;

		//대입 연산자 오버로딩
		Vector2& operator=(const Vector2& other);

		//비교 연산자 오버로딩
		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		//자주 사용할 값을 static(전역변수)로 지정
		static Vector2 Zero;
		static Vector2 One;  
		static Vector2 Right;
		static Vector2 Up; 
		static Vector2 oneDown;
		static Vector2 twoDown;

	public:
		//좌표계로 사용하기 위한 변수.
		int x = 0;
		int y = 0;
	};
}


