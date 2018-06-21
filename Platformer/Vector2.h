#pragma once

//Includes

//Custom Includes

namespace Engine2D
{

	class Vector2
	{

	//Methods

	public:

		Vector2();
		Vector2(float x, float y);
		~Vector2();

		float X() const;
		float Y() const;
		void SetX(float x);
		void SetY(float y);

		float LengthSquared() const;
		float Length() const;

		void Rotate(float a);
		void Normalize();

		float Dot(Vector2 vec);

		Vector2& operator=(const Vector2& rhs);
		Vector2& operator+=(const Vector2& rhs);
		Vector2& operator-=(const Vector2& rhs);
		Vector2& operator *=(const float& rhs);

		const Vector2 operator+(const Vector2 rhs);
		const Vector2 operator-(const Vector2 rhs);
		const Vector2 operator*(const float rhs);

		const Vector2 operator-();

	protected:

	//Data

	public:

	protected:

		float x;
		float y;

	};

}