#pragma once

//Includes
#include <fstream>

//Custom Includes
#include "GameObject.h"

using namespace Engine2D;

namespace Platformer
{

	class MovingPlatform : public GameObject
	{

	//Methods

	public:

		MovingPlatform(int i);
		MovingPlatform(int i, float x, float y, float w, float h);
		virtual ~MovingPlatform();

		virtual Vector2 Update(float deltaTime) = 0;
		virtual bool Condition(GameObject* obj, Vector2 velocity, float deltaTime) = 0;

		static MovingPlatform* MovingPlatformFactory(std::ifstream* file);

		virtual void Trigger();

		int Index();

	protected:

		virtual float Speed();

	//Data

	public:

	protected:

		float speed = 0;

		int renderIndex;

	};

	class ConstantPlatform : public MovingPlatform
	{

	//Methods

	public:

		ConstantPlatform(int i);
		ConstantPlatform(int i, float x, float y, float w, float h, float s, float maX, float maY, float miX, float miY, int d);
		~ConstantPlatform();

		Vector2 Update(float deltaTime);
		bool Condition(GameObject* obj, Vector2 velocity, float deltaTime);

		void SetSpeed(float s);

	protected:

	//Data

	public:

	protected:

		Vector2 max;
		Vector2 min;

		Vector2 unit;

		int direction = 1;

	};

	class Elevator : public MovingPlatform
	{

	//Methods

	public:

		Elevator(int i);
		Elevator(int i, float x, float y, float w, float h, float s, float maX, float maY, float miX, float miY, int d);
		~Elevator();

		Vector2 Update(float deltaTime);
		bool Condition(GameObject* obj, Vector2 velocity, float deltaTime);

		void SetSpeed(float s);

		void Trigger();

	protected:

	//Data

	public:

	protected:

		bool moving;

		Vector2 max;
		Vector2 min;

		Vector2 unit;

		int direction = 1;

	};

	class RotatingPlatform : public MovingPlatform
	{

	//Methods

	public:

		RotatingPlatform(int i);
		RotatingPlatform(int i, float x, float y, float w, float h, float s, float maX, float maY, float miX, float miY, int d, float startX, float startY, float rPS);
		~RotatingPlatform();

		Vector2 Update(float deltaTime);
		bool Condition(GameObject* obj, Vector2 velocity, float deltaTime);

	protected:

	//Data

	public:

	protected:
		
		ConstantPlatform base;

		Vector2 start;
		float revPerSecond;
		float angle = 0;

	};

	class ConveyorBelt : public MovingPlatform
	{

	//Methods

	public:

		ConveyorBelt(int i);
		ConveyorBelt(int i, float x, float y, float w, float h, float s);
		~ConveyorBelt();

		Vector2 Update(float deltaTime);
		bool Condition(GameObject* obj, Vector2 velocity, float deltaTime);

	protected:

	//Data

	public:

	protected:

	};

}