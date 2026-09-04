#pragma once

#include <Math/Vector2.h>
#include <Windows.h>

namespace Craft
{
	//이중 버퍼링 구현을 위한 화면 버퍼 클래스.
	//콘솔 핸들 관리.
	class ScreenBuffer
	{
	public:
		//Vector2라는 다른 타입의 객체를 받아 객체 초기화.
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		//콘솔 초기화 - 화면 지우기
		void Clear() const;

		//전달된 글자 값 그리는 함수.
		void Draw(const CHAR_INFO* const charInfo) const;

		//Getter
		inline HANDLE GetBuffer() const { return buffer; }

	private:
		//HANDLE:특정 값을 가리키는 포인터
		//화면 버퍼 핸들.
		HANDLE buffer = nullptr;

		//화면 크기.
		Vector2 size;
	};
}

