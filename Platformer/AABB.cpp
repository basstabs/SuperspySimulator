//Includes
#include <math.h>

//Custom Includes
#include "AABB.h"

namespace Engine2D
{

	AABB::AABB() : AABB(0, 0, 0, 0)
	{

	}

	AABB::AABB(int x, int y, int w, int h)
	{

		this->SetX(x);
		this->SetY(y);
		this->SetWidth(w);
		this->SetHeight(h);

	}

	AABB::~AABB()
	{

	}

	bool AABB::Intersects(AABB* box)
	{

		if(box->X() >= this->X() + this->Width())
		{

			return false;

		}

		if(box->X() + box->Width() <= this->X())
		{

			return false;

		}

		if(box->Y() >= this->Y() + this->Height())
		{

			return false;

		}

		if(box->Y() + box->Height() <= this->Y())
		{

			return false;

		}

		return true;

	}

	Vector2 AABB::Collides(AABB* box, Vector2 velocity, Vector2 thisVelocity)
	{

		Vector2 correction;

		if (box->X() >= this->X() + this->Width())
		{

			return correction;

		}

		if (box->X() + box->Width() <= this->X())
		{

			return correction;

		}

		if (box->Y() >= this->Y() + this->Height())
		{

			return correction;

		}

		if (box->Y() + box->Height() <= this->Y())
		{

			return correction;

		}

		//Collision, now fix it
		if (velocity.X() >= 0)
		{

			correction.SetX(box->X() - this->X() - this->Width());

		}
		else if (velocity.X() < 0)
		{

			correction.SetX(box->X() + box->Width() - this->X());

		}

		if (velocity.Y() > 0)
		{

			correction.SetY(box->Y() - this->Y() - this->Height());

		}
		else if (velocity.Y() < 0)
		{

			correction.SetY(box->Y() + box->Height() - this->Y());

		}

		if (fabsf(correction.X()) >= fabsf(correction.Y()) && correction.Y() != 0 && !(correction.Y() > 0 && thisVelocity.Y() < 0 && velocity.Y() >= -FLOATING_POINT_ERROR))
		{

			correction.SetX(0);

		}
		else
		{

			correction.SetY(0);

		}

		return correction;

	}

	bool AABB::Contains(Vector2 point)
	{

		if (point.X() >= this->X() + this->Width())
		{

			return false;

		}

		if (point.X() <= this->X())
		{

			return false;

		}

		if (point.Y() >= this->Y() + this->Height())
		{

			return false;

		}

		if (point.Y() <= this->Y())
		{

			return false;

		}

		return true;

	}

	float AABB::X()
	{

		return this->x;

	}

	float AABB::Y()
	{

		return this->y;

	}

	float AABB::Width()
	{

		return this->width;

	}

	float AABB::Height()
	{

		return this->height;

	}

	void AABB::SetX(float x)
	{

		this->x = x;

	}

	void AABB::SetY(float y)
	{

		this->y = y;

	}

	void AABB::SetWidth(float w)
	{

		this->width = w;

	}

	void AABB::SetHeight(float h)
	{

		this->height = h;

	}

	Vector2 AABB::Center()
	{

		return Vector2(this->X() + (this->Width() / 2), this->Y() + (this->Height() / 2));

	}
}