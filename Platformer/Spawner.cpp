//Includes
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "Enemy.h"
#include "Spawner.h"

namespace Platformer
{

	Condition::~Condition()
	{

	}

	void Condition::Pause()
	{

		this->paused = true;
		this->pauseTime = GetTime();

	}

	void Condition::Unpause()
	{

		this->paused = false;

	}

	double Condition::PauseTime(double time)
	{

		return GetTime() - (this->pauseTime - time);

	}

	bool Condition::Active()
	{

		return this->active;

	}

	void Condition::Spawn()
	{

	}

	Condition* Condition::ConditionFactory(std::ifstream* file, std::string id)
	{

		std::string input;
		std::getline(*file, input);

		std::istringstream in(input);

		int numConditions;
		in >> numConditions;

		Condition* newCondition = NULL;

		for (int i = 0; i < numConditions; i++)
		{

			Condition* condition = NULL;

			std::getline(*file, input);

			if (input == TIME_CODE)
			{

				double time;
				std::getline(*file, input);
				in.clear();
				in.str(input);
				in >> time;

				condition = new TimeCondition(time);

			}
			else if (input == COUNT_CODE)
			{

				int max;
				std::getline(*file, input);
				in.clear();
				in.str(input);
				in >> max;

				condition = new CountCondition(max, id);

			}
			else if (input == GLOBAL_COUNT_CODE)
			{

				int max;
				std::getline(*file, input);
				in.clear();
				in.str(input);
				in >> max;

				condition = new GlobalCountCondition(max);

			}

			if (newCondition)
			{

				newCondition = new AndCondition(newCondition, condition);

			}
			else
			{

				newCondition = condition;

			}

		}

		return newCondition;

	}

	AndCondition::AndCondition(Condition* p, Condition* q)
	{

		this->p = p;
		this->q = q;

	}

	AndCondition::~AndCondition()
	{

		if (this->p)
		{

			delete this->p;
			this->p = NULL;

		}

		if (this->q)
		{

			delete this->q;
			this->q = NULL;

		}

	}

	bool AndCondition::Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		return this->p->Pass(entities, attacks, deltaTime) && this->q->Pass(entities, attacks, deltaTime);

	}

	void AndCondition::Pause()
	{

		this->Condition::Pause();

		this->p->Pause();
		this->q->Pause();

	}

	void AndCondition::Unpause()
	{

		this->Condition::Unpause();

		this->p->Unpause();
		this->q->Unpause();

	}

	void AndCondition::Spawn()
	{

		this->p->Spawn();
		this->q->Spawn();

	}

	bool AndCondition::Active()
	{

		return this->active && this->p->Active() && this->q->Active();

	}

	TimeCondition::TimeCondition(double t)
	{

		this->time = t;

		this->lastSpawn = GetTime();

	}

	TimeCondition::~TimeCondition()
	{

	}

	bool TimeCondition::Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		if (GetTime() - this->lastSpawn < this->time)
		{

			this->lastSpawn = GetTime();

			return true;

		}

		return false;

	}

	void TimeCondition::Unpause()
	{

		this->Condition::Unpause();

		this->lastSpawn = this->PauseTime(this->lastSpawn);

	}

	CountCondition::CountCondition(int m, std::string id)
	{

		this->max = m;

		this->id = id;

	}

	CountCondition::~CountCondition()
	{

	}

	bool CountCondition::Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		int numActive = 0;

		for (int i = 0; i < entities.size(); i++)
		{

			if (entities[i]->Type() == this->id)
			{

				numActive++;
				if (numActive >= this->max)
				{

					return false;

				}

			}

		}

		return true;

	}

	GlobalCountCondition::GlobalCountCondition(int m)
	{

		this->max = m;
		this->count = 0;

	}

	GlobalCountCondition::~GlobalCountCondition()
	{

	}

	bool GlobalCountCondition::Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		return this->count < this->max;

	}

	void GlobalCountCondition::Spawn()
	{

		this->count++;

		if (this->count >= this->max)
		{

			this->active = false;

		}

	}

	Spawner::Spawner() : Spawner(0, 0, 0, 0)
	{

	}

	Spawner::Spawner(float x, float y, float w, float h) : GameObject(x, y, w, h)
	{

		this->lastSpawn = GetTime();
		this->currentIndex = 0;
		this->condition = NULL;

	}

	Spawner::~Spawner()
	{

		if (this->condition)
		{

			delete this->condition;
			this->condition = NULL;

		}

	}

	void Spawner::Start(std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		this->lastSpawn = GetTime();

	}

	void Spawner::Update(std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		if (this->condition && this->condition->Pass(entities, attacks, deltaTime))
		{

			this->Spawn(master, entities, attacks, deltaTime);

		}

	}

	void Spawner::Spawn(std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Enemy::Spawn(Vector2(this->X(), this->Y()), this->id, this->codes[this->currentIndex], master, entities, attacks, deltaTime);

		this->currentIndex = (this->currentIndex + 1) % this->codes.size();
		this->lastSpawn = GetTime();

		if (this->condition)
		{

			this->condition->Spawn();

		}

	}

	void Spawner::Pause()
	{

		this->GameObject::Pause();
		this->condition->Pause();

	}

	void Spawner::Unpause()
	{

		this->GameObject::Unpause();
		this->condition->Unpause();

		this->lastSpawn = this->PauseTime(this->lastSpawn);

	}

	void Spawner::SetCodes(std::vector<std::string> codes)
	{

		this->codes = codes;

	}

	void Spawner::SetId(std::string id)
	{

		this->id = id;

	}

	void Spawner::SetCondition(Condition* condition)
	{

		this->condition = condition;

	}

	Spawner* Spawner::SpawnerFactory(std::ifstream* file)
	{

		Spawner* newSpawner = NULL;

		std::string id;
		std::getline(*file, id);

		std::string line;
		std::getline(*file, line);

		std::istringstream input(line);

		float x, y;
		input >> x;
		input >> y;

		newSpawner = new Spawner(x, y, 0, 0);

		int numCodes;

		std::getline(*file, line);
		input.clear();
		input.str(line);

		input >> numCodes;

		std::vector<std::string> codes;

		for (int i = 0; i < numCodes; i++)
		{

			std::getline(*file, line);
			codes.push_back(line);

		}

		newSpawner->SetCodes(codes);

		newSpawner->SetId(id);

		newSpawner->SetCondition(Condition::ConditionFactory(file, id));

		return newSpawner;

	}

}