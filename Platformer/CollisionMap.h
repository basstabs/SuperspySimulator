#pragma once

//Includes
#include <SDL.h>
#include <string>
#include <map>
#include <vector>

//Custom Includes
#include "GameObject.h"
#include "MovingPlatform.h"

using namespace Engine2D;

namespace Platformer
{

	class CollisionMap
	{

	//Methods

	public:

		CollisionMap();
		~CollisionMap();

		void LoadCollisionMap(std::string fileName);

		void Update(std::vector<GameEntity*>& movable, float deltaTime);

		bool Intersects(GameObject* obj, bool oneway = true);
		bool PlatformCollision(GameObject* obj, bool oneWay);

		void Draw(Vector2 world, float deltaTime);
		void DebugDraw(Vector2 world, float deltaTime);

		void Trigger(int index);
		void Register(std::string code, AABB* box);
		void Unregister(std::string code);

		bool Kill(char c);

	protected:

	//Data

	public:

	protected:

		std::vector<AABB> solid;
		std::vector<AABB> oneway;
		std::vector<MovingPlatform*> moving;

		std::vector<SDL_Texture*> movingSurfaces;

		std::map<std::string, AABB*> registered;

	};

}