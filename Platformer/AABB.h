#pragma once

//Includes

//Custom Includes
#include "Vector2.h"

#define FLOATING_POINT_ERROR 0.0001

namespace Engine2D
{

	class AABB
	{

	//Methods

	public:

		AABB();
		AABB(int x, int y, int w, int h);
		~AABB();

		bool Intersects(AABB* box);
		Vector2 Collides(AABB* box, Vector2 velocity, Vector2 thisVelocity = Vector2(0, 0));

		bool Contains(Vector2 point);

		float X();
		float Y();
		float Width();
		float Height();

		void SetX(float x);
		void SetY(float y);
		void SetWidth(float w);
		void SetHeight(float h);

		Vector2 Center();

	protected:

	//Data

	public:

	protected:

		float x;
		float y;

		float width;
		float height;

	};

}