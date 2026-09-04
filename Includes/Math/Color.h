#pragma once

#include <Core/Core.h>
#include <Windows.h>

namespace Craft
{
	//색상을 열거형으로 정의.
	enum class CRAFT_API Color : WORD
	{
		//FOREGROUND - 글자 색상
		//BACKGROUND - 배경 색상
		//bit | = +
		//intnesity 는 밝기
		Red = FOREGROUND_RED,
		Green = FOREGROUND_GREEN,
		Blue = FOREGROUND_BLUE,
		Yellow = Red | Green,
		Cyan = Green | Blue,
		Purple = Red | Blue,
		White = Red | Green,
		BrightWhite = White | FOREGROUND_INTENSITY
	};
}