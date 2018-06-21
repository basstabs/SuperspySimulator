//Includes
#include <sstream>

//Custom Includes
#include "MovingPlatform.h"

namespace Platformer
{

	MovingPlatform::MovingPlatform(int i) : GameObject()
	{


		this->renderIndex = i;

	}

	MovingPlatform::MovingPlatform(int i, float x, float y, float w, float h) : GameObject(x, y, w, h)
	{

		this->renderIndex = i;

	}

	MovingPlatform::~MovingPlatform()
	{

	}

	float MovingPlatform::Speed()
	{

		return this->speed;

	}

	void MovingPlatform::Trigger()
	{

	}

	int MovingPlatform::Index()
	{

		return this->renderIndex;

	}

	ConstantPlatform::ConstantPlatform(int i) : MovingPlatform(i)
	{

		this->speed = 0;

	}

	ConstantPlatform::ConstantPlatform(int i, float x, float y, float w, float h, float s, float maX, float maY, float miX, float miY, int d) : MovingPlatform(i, x, y, w, h)
	{

		this->speed = s;

		this->max = Vector2(maX, maY);
		this->min = Vector2(miX, miY);
		this->direction = d;

		this->unit = this->max - this->min;
		this->unit.Normalize();

	}

	ConstantPlatform::~ConstantPlatform()
	{

	}

	Vector2 ConstantPlatform::Update(float deltaTime)
	{

		Vector2 velocity = this->unit * (this->Speed() * deltaTime);
		velocity = velocity * this->direction;

		Vector2 old(this->X(), this->Y());
		this->Move(velocity);

		float pos, ma, mi;
		pos = this->unit.Dot(Vector2(this->X(), this->Y()));
		ma = this->unit.Dot(this->max);
		mi = this->unit.Dot(this->min);

		if (pos <= mi && this->direction == -1)
		{

			this->SetX(this->min.X());
			this->SetY(this->min.Y());

			this->direction = 1;

			velocity = this->min - old;

		}
		else if (pos >= ma && this->direction == 1)
		{

			this->SetX(this->max.X());
			this->SetY(this->max.Y());

			this->direction = -1;

			velocity = this->max - old;

		}

		this->SetVelocity(velocity);
		return -velocity;

	}

	bool ConstantPlatform::Condition(GameObject* obj, Vector2 velocity, float deltaTime)
	{

		bool intersects = false;
		int move = (obj->Y() >= this->Y() ? -1 : 1);
		obj->Move(Vector2(0, move));

		intersects = this->Intersects(obj);

		obj->Move(Vector2(0, -move));

		return intersects;

	}

	void ConstantPlatform::SetSpeed(float s)
	{

		this->speed = s;

	}

	Elevator::Elevator(int i) : MovingPlatform(i)
	{

		this->speed = 0;
		this->moving = false;

	}

	Elevator::Elevator(int i, float x, float y, float w, float h, float s, float maX, float maY, float miX, float miY, int d) : MovingPlatform(i, x, y, w, h)
	{

		this->speed = s;

		this->max = Vector2(maX, maY);
		this->min = Vector2(miX, miY);
		this->direction = d;

		this->unit = this->max - this->min;
		this->unit.Normalize();

		this->moving = false;

	}

	Elevator::~Elevator()
	{

	}

	Vector2 Elevator::Update(float deltaTime)
	{

		if (this->moving)
		{

			Vector2 velocity = this->unit * (this->Speed() * deltaTime);
			velocity = velocity * this->direction;

			Vector2 old(this->X(), this->Y());
			this->Move(velocity);

			float pos, ma, mi;
			pos = this->unit.Dot(Vector2(this->X(), this->Y()));
			ma = this->unit.Dot(this->max);
			mi = this->unit.Dot(this->min);

			if (pos <= mi && this->direction == -1)
			{

				this->SetX(this->min.X());
				this->SetY(this->min.Y());

				this->direction = 1;

				velocity = this->min - old;

				this->moving = false;

			}
			else if (pos >= ma && this->direction == 1)
			{

				this->SetX(this->max.X());
				this->SetY(this->max.Y());

				this->direction = -1;

				velocity = this->max - old;

				this->moving = false;

			}

			this->SetVelocity(velocity);
			return -velocity;

		}

		return Vector2();

	}

	bool Elevator::Condition(GameObject* obj, Vector2 velocity, float deltaTime)
	{

		bool intersects = false;
		int move = (obj->Y() >= this->Y() ? -1 : 1);
		obj->Move(Vector2(0, move));

		intersects = this->Intersects(obj);

		obj->Move(Vector2(0, -move));

		return intersects;

	}

	void Elevator::SetSpeed(float s)
	{

		this->speed = s;

	}

	void Elevator::Trigger()
	{

		this->moving = true;

	}

	RotatingPlatform::RotatingPlatform(int i) : MovingPlatform(i), base(i)
	{

		this->base = ConstantPlatform(i);

	}

	RotatingPlatform::RotatingPlatform(int i, float x, float y, float w, float h, float s, float maX, float maY, float miX, float miY, int d, float startX, float startY, float rPS) : MovingPlatform(i), base(i)
	{

		this->SetWidth(w);
		this->SetHeight(h);

		this->start = Vector2(startX, startY);

		this->SetX(x + this->start.X() - (w / 2));
		this->SetY(y + this->start.Y() - (h / 2));

		this->base = ConstantPlatform(i, x, y, 0, 0, s, maX, maY, miX, miY, d);

		this->angle = 0;
		this->revPerSecond = rPS;

	}

	RotatingPlatform::~RotatingPlatform()
	{

	}

	Vector2 RotatingPlatform::Update(float deltaTime)
	{

		Vector2 old(this->X(), this->Y());

		this->base.Update(deltaTime);

		this->angle += this->revPerSecond * deltaTime;
		Vector2 offset = this->start;
		offset.Rotate(this->angle);

		this->SetX(this->base.X() + offset.X() - (this->Width() / 2));
		this->SetY(this->base.Y() + offset.Y() - (this->Height() / 2));

		return old - Vector2(this->X(), this->Y());

	}

	bool RotatingPlatform::Condition(GameObject* obj, Vector2 velocity, float deltaTime)
	{

		bool intersects = false;
		int move = (obj->Y() >= this->Y() ? -1 : 1);
		obj->Move(Vector2(0, move));

		intersects = this->Intersects(obj);

		obj->Move(Vector2(0, -move));

		return intersects;


	}

	MovingPlatform* MovingPlatform::MovingPlatformFactory(std::ifstream* file)
	{

		MovingPlatform* newPlatform = NULL;

		std::string type;
		std::getline(*file, type);

		std::string line;

		std::getline(*file, line);
		std::istringstream input(line);

		int index;
		input >> index;

		if (type == "C" || type == "E" || type == "R")
		{

			float x, y, w, h, s, maX, maY, miX, miY;
			int d = 1;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;
			input >> w;
			input >> h;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> s;
			input >> maX;
			input >> maY;
			input >> miX;
			input >> miY;

			if (type == "C")
			{

				newPlatform = new ConstantPlatform(index, x, y, w, h, s, maX, maY, miX, miY, d);

			}
			else if (type == "R")
			{

				float startX, startY, rPS;

				std::getline(*file, line);
				input.clear();
				input.str(line);

				input >> startX;
				input >> startY;
				input >> rPS;

				newPlatform = new RotatingPlatform(index, x, y, w, h, s, maX, maY, miX, miY, d, startX, startY, 2 * M_PI * rPS);

			}
			else
			{

				newPlatform = new Elevator(index, x, y, w, h, s, maX, maY, miX, miY, d);

			}

		}
		else if (type == "B")
		{

			float x, y, w, h, s;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;
			input >> w;
			input >> h;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> s;

			newPlatform = new ConveyorBelt(index, x, y, w, h, s);
		}

		return newPlatform;

	}

	ConveyorBelt::ConveyorBelt(int i) : MovingPlatform(i)
	{

	}

	ConveyorBelt::ConveyorBelt(int i, float x, float y, float w, float h, float s) : MovingPlatform(i, x, y, w, h)
	{

		this->speed = s;
	}

	ConveyorBelt::~ConveyorBelt()
	{

	}

	Vector2 ConveyorBelt::Update(float deltaTime)
	{

		return Vector2(this->speed * deltaTime, 0);

	}

	bool ConveyorBelt::Condition(GameObject* obj, Vector2 velocity, float deltaTime)
	{

		bool intersects = false;
		int move = (obj->Y() >= this->Y() ? -1 : 1);
		obj->Move(Vector2(0, move));

		intersects = this->Intersects(obj);

		obj->Move(Vector2(0, -move));

		return intersects;

	}

}
