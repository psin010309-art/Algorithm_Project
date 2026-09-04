#include <Math/Vector2.h>
#include <cassert>

namespace Craft
{
	 Vector2 Vector2::Zero(0, 0);
	 Vector2 Vector2::One(1, 1);
	 Vector2 Vector2::Right(1, 0);
	 Vector2 Vector2::Up(0, -1);
	 Vector2 Vector2::oneDown(0, 1);
	 Vector2 Vector2::twoDown(0, 2);

	Vector2::Vector2(int x, int y)
		:x(x), y(y)
	{
	}
	Vector2::operator COORD() const
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}
	Vector2::operator COORD()
	{
		COORD coord = {};
		coord.X = static_cast<short>(x);
		coord.Y = static_cast<short>(y);

		return coord;
	}
	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}
	Vector2 Vector2::operator*(const Vector2& other) const
	{
		return Vector2(x * other.x, y * other.y);
	}
	Vector2 Vector2::operator/(const Vector2& other) const
	{
		//어서트.
		//예외처리 코드 추가해보기
		//둘 중 하나라도 0이면 0벡터 출력
		assert(other.x != 0 && other.y != 0);
		return Vector2(x / other.x, y / other.y);
	}
	Vector2& Vector2::operator=(const Vector2& other)
	{
		x = other.x;
		y = other.y;

		return *this;
	}
	bool Vector2::operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}
	bool Vector2::operator!=(const Vector2& other) const
	{
		//return (x != other.x) || (y != other.y);
		return !(*this == other);
		//다르면 true, 같으면 false
	}
}