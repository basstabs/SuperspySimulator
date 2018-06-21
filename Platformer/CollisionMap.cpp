//Includes
#include <fstream>
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "CollisionMap.h"

namespace Platformer
{

	CollisionMap::CollisionMap()
	{

	}

	CollisionMap::~CollisionMap()
	{

		for (int i = 0; i < this->moving.size(); i++)
		{

			delete this->moving[i];
			this->moving[i] = NULL;

		}

		for (int i = 0; i < this->movingSurfaces.size(); i++)
		{

			SDL_DestroyTexture(this->movingSurfaces[i]);
			this->movingSurfaces[i] = NULL;

		}

		this->movingSurfaces.clear();

	}

	void CollisionMap::LoadCollisionMap(std::string fileName)
	{

		std::ifstream file(fileName);
		std::string line;

		std::getline(file, line);
		std::istringstream input(line);

		int numSolid, numOneWay, numMoving;

		input >> numSolid;
		input >> numOneWay;
		input >> numMoving;

		for (int i = 0; i < numSolid; i++)
		{

			std::getline(file, line);

			input.clear();
			input.str(line);

			float x, y, w, h;
			input >> x;
			input >> y;
			input >> w;
			input >> h;

			this->solid.push_back(AABB(x, y, w, h));

		}

		for (int i = 0; i < numOneWay; i++)
		{

			std::getline(file, line);

			input.clear();
			input.str(line);

			float x, y, w, h;
			input >> x;
			input >> y;
			input >> w;
			input >> h;

			this->oneway.push_back(AABB(x, y, w, h));

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numImages;
		input >> numImages;

		for (int i = 0; i < numImages; i++)
		{

			std::getline(file, line);

			SDL_Texture* texture = LoadTexture(line);
			this->movingSurfaces.push_back(texture);

		}

		for (int i = 0; i < numMoving; i++)
		{

			this->moving.push_back(MovingPlatform::MovingPlatformFactory(&file));

		}

		file.close();

	}

	void CollisionMap::Update(std::vector<GameEntity*>& movable, float deltaTime)
	{

		Vector2 correction;

		for (int i = 0; i < this->moving.size(); i++)
		{

			Vector2 velocity = this->moving[i]->Update(deltaTime);
			this->moving[i]->Move(velocity);

			for (int j = 0; j < movable.size(); j++)
			{

				if (this->moving[i]->Condition(movable[j], velocity, deltaTime))
				{

					movable[j]->Move(-velocity);
					movable[j]->SetVelocity(-velocity);

				}
				else
				{

					this->moving[i]->Move(-velocity);

					correction = movable[j]->Collides(this->moving[i], velocity);
					movable[j]->Move(correction);

					this->moving[i]->Move(velocity);

				}

			}

			this->moving[i]->Move(-velocity);

		}

	}

	bool CollisionMap::Intersects(GameObject* obj, bool oneway)
	{

		for (int i = 0; i < this->solid.size(); i++)
		{

			if (this->solid[i].Intersects(obj))
			{

				return true;

			}

		}

		std::map<std::string, AABB*>::iterator it;
		for (it = this->registered.begin(); it != this->registered.end(); it++)
		{

			if (it->second->Intersects(obj))
			{

				return true;

			}

		}

		if (oneway)
		{

			for (int i = 0; i < this->oneway.size(); i++)
			{

				Vector2 correction = obj->Collides(&this->oneway[i], obj->Velocity());

				if (correction.Y() < 0 && obj->Y() - obj->Velocity().Y() + obj->Height() <= this->oneway[i].Y())
				{

					return true;

				}

			}

		}

		for (int i = 0; i < this->moving.size(); i++)
		{

			if (this->moving[i]->Intersects(obj))
			{

				return true;

			}

		}

		return false;

	}

	bool CollisionMap::PlatformCollision(GameObject* obj, bool oneWay)
	{

		Vector2 correction;
		bool topCollision = false;

		std::map<std::string, AABB*>::iterator it;
		for (it = this->registered.begin(); it != this->registered.end(); it++)
		{

			correction = obj->Collides(it->second, obj->Velocity());

			obj->Move(correction);
			obj->SetVelocity(obj->Velocity() + correction);

			if (correction.Y() != 0)
			{

				Vector2 raw = obj->RawVelocity();
				raw.SetY(0);
				obj->SetRawVelocity(raw);

			}

			if (correction.Y() < 0)
			{

				topCollision = true;

			}

		}

		for (int i = 0; i < this->solid.size(); i++)
		{

			correction = obj->Collides(&this->solid[i], obj->Velocity());

			obj->Move(correction);
			obj->SetVelocity(obj->Velocity() + correction);
			
			if (correction.Y() != 0)
			{

				Vector2 raw = obj->RawVelocity();
				raw.SetY(0);
				obj->SetRawVelocity(raw);

			}

			if (correction.Y() < 0)
			{

				topCollision = true;

			}

		}

		if (oneWay)
		{

			for (int i = 0; i < this->oneway.size(); i++)
			{

				correction = obj->Collides(&this->oneway[i], obj->Velocity());

				if (correction.Y() < 0 && obj->Y() - obj->Velocity().Y() + obj->Height() <= this->oneway[i].Y())
				{

					obj->SetY(obj->Y() + correction.Y());
					obj->SetVelocity(Vector2(0, obj->Velocity().Y() + correction.Y()));
					obj->SetRawVelocity(Vector2(obj->RawVelocity().X(), 0));

					topCollision = true;

				}

			}

		}

		for (int i = 0; i < this->moving.size(); i++)
		{

			correction = obj->Collides(this->moving[i], obj->Velocity(), this->moving[i]->Velocity());

			obj->Move(correction);
			obj->SetVelocity(obj->Velocity() + correction);

			if (correction.Y() != 0)
			{

				Vector2 raw = obj->RawVelocity();
				raw.SetY(0);
				obj->SetRawVelocity(raw);

			}

			if (correction.Y() < 0)
			{

				topCollision = true;

			}

		}

		return topCollision;

	}

	void CollisionMap::Draw(Vector2 world, float deltaTime)
	{

		for (int i = 0; i < this->moving.size(); i++)
		{

			if (this->moving[i]->Index() >= 0)
			{

				SDL_Texture* tex = this->movingSurfaces[this->moving[i]->Index()];

				SDL_Rect src;
				SDL_Rect dest;

				src.x = 0;
				src.y = 0;
				SDL_QueryTexture(tex, NULL, NULL, &src.w, &src.h);

				dest.x = this->moving[i]->X() - world.X();
				dest.y = this->moving[i]->Y() - world.Y();
				dest.w = src.w;
				dest.h = src.h;

				SDL_RenderCopyEx(renderer, tex, &src, &dest, 0, NULL, SDL_FLIP_NONE);

			}

		}

	}

	void CollisionMap::DebugDraw(Vector2 world, float deltaTime)
	{

		for (int i = 0; i < this->solid.size(); i++)
		{

			FillAABB(&this->solid[i], world, 128, 0, 0, 128);

		}

		for (int i = 0; i < this->oneway.size(); i++)
		{

			FillAABB(&this->oneway[i], world, 0, 128, 0, 128);

		}

		for (int i = 0; i < this->moving.size(); i++)
		{

			FillAABB(this->moving[i], world, 0, 0, 128, 128);

		}

		std::map<std::string, AABB*>::iterator it;
		for (it = this->registered.begin(); it != this->registered.end(); it++)
		{

			FillAABB(it->second, world, 0, 128, 128, 128);

		}

	}

	void CollisionMap::Trigger(int index)
	{

		if (index >= 0 && index < this->moving.size())
		{

			this->moving[index]->Trigger();

		}

	}

	void CollisionMap::Register(std::string code, AABB* box)
	{

		this->registered[code] = box;

	}

	void CollisionMap::Unregister(std::string code)
	{

		this->registered.erase(code);

	}

	bool CollisionMap::Kill(char c)
	{

		if (c == 'P')
		{

			return true;

		}

		if (c == 'E')
		{

			return true;

		}

		return false;

	}
}