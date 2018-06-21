//Includes
#include <math.h>

//Custom Includes
#include "Vector2.h"

namespace Engine2D
{

	Vector2::Vector2() : Vector2(0, 0)
	{

	}

	Vector2::Vector2(float x, float y)
	{

		this->SetX(x);
		this->SetY(y);

	}

	Vector2::~Vector2()
	{

	}

	float Vector2::X() const
	{

		return this->x;

	}

	float Vector2::Y() const
	{

		return this->y;

	}

	void Vector2::SetX(float x)
	{

		this->x = x;

	}

	void Vector2::SetY(float y)
	{

		this->y = y;

	}

	float Vector2::LengthSquared() const
	{

		return this->X() * this->X() + this->Y() * this->Y();

	}

	float Vector2::Length() const
	{

		return sqrt(this->LengthSquared());

	}

	void Vector2::Rotate(float a)
	{

		float cosine = cosf(a);
		float sine = sinf(a);

		float newX, newY;

		newX = x * cosine - y * sine;
		newY = x * sine + y * cosine;

		this->SetX(newX);
		this->SetY(newY);

	}

	void Vector2::Normalize()
	{

		float l = this->Length();

		if (l == 0)
		{

			return;

		}

		this->x = this->x / l;
		this->y = this->y / l;

	}

	float Vector2::Dot(Vector2 vec)
	{

		return (this->X() * vec.X()) + (this->Y() * vec.Y());

	}

	Vector2& Vector2::operator=(const Vector2& rhs)
	{

		this->SetX(rhs.X());
		this->SetY(rhs.Y());

		return *this;

	}

	Vector2& Vector2::operator+=(const Vector2& rhs)
	{

		this->SetX(this->X() + rhs.X());
		this->SetY(this->Y() + rhs.Y());

		return *this;

	}

	Vector2& Vector2::operator-=(const Vector2& rhs)
	{

		this->SetX(this->X() - rhs.X());
		this->SetY(this->Y() - rhs.Y());

		return *this;

	}

	Vector2& Vector2::operator*=(const float& rhs)
	{

		this->SetX(this->X() * rhs);
		this->SetY(this->Y() * rhs);

		return *this;

	}

	const Vector2 Vector2::operator+(const Vector2 rhs)
	{

		Vector2 sum = *this;
		sum += rhs;

		return sum;

	}

	const Vector2 Vector2::operator-(const Vector2 rhs)
	{

		Vector2 difference = *this;
		difference -= rhs;

		return difference;

	}

	const Vector2 Vector2::operator*(const float rhs)
	{

		Vector2 scaled = *this;
		scaled *= rhs;

		return scaled;

	}

	const Vector2 Vector2::operator-()
	{

		Vector2 opposite;
		opposite -= *this;

		return opposite;

	}

}