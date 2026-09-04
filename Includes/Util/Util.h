#pragma once

#include <random>
#include <Core/Core.h> // CRAFT_API 매크로

namespace Util
{

	CRAFT_API std::mt19937& GetRandomEngine();
	CRAFT_API void SetRandomSeed();
	CRAFT_API int RandomRange(int min, int max);
	CRAFT_API float RandomRange(float min, float max);
}