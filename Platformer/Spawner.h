#pragma once

//Includes
#include <vector>
#include <fstream>

//Custom Includes
#include "GameObject.h"
#include "Attack.h"
#include "Enemy.h"

#define TIME_CODE "T"
#define COUNT_CODE "C"
#define GLOBAL_COUNT_CODE "GC"

namespace Platformer
{

	class Condition
	{

	//Methods

	public:

		virtual ~Condition();

		virtual bool Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime) = 0;

		static Condition* ConditionFactory(std::ifstream* file, std::string id);

		virtual void Spawn();

		virtual void Pause();
		virtual void Unpause();
		double PauseTime(double time);

		virtual bool Active();

	protected:

	//Data

	public:

	protected:

		bool active = true;

		bool paused;
		double pauseTime;

	};

	class AndCondition : public Condition
	{

	//Methods

	public:

		AndCondition(Condition* p, Condition* q);
		~AndCondition();

		bool Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		void Spawn();

		void Pause();
		void Unpause();

		bool Active();

	protected:

	//Data

	public:

	protected:

		Condition* p;
		Condition* q;

	};

	class TimeCondition : public Condition
	{

	//Methods

	public:

		TimeCondition(double t);
		~TimeCondition();

		bool Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		void Unpause();

	protected:

	//Data

	public:

	protected:

		double time;
		double lastSpawn;

	};

	class CountCondition : public Condition
	{

	//Methods

	public:

		CountCondition(int m, std::string id);
		~CountCondition();

		bool Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

	protected:

	//Data

	public:

	protected:

		std::string id;

		int max;

	};

	class GlobalCountCondition : public Condition
	{

	//Methods

	public:

		GlobalCountCondition(int m);
		~GlobalCountCondition();

		bool Pass(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		void Spawn();

	protected:

	//Data

	public:

	protected:

		int max;

		int count;

	};

	class Spawner : public GameObject
	{

	//Methods

	public:

		Spawner();
		Spawner(float x, float y, float w, float h);
		virtual ~Spawner();

		void Start(std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		void Update(std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		
		void Spawn(std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		void SetCodes(std::vector<std::string> codes);
		void SetId(std::string id);
		void SetCondition(Condition* condition);

		void Pause();
		void Unpause();

		static Spawner* SpawnerFactory(std::ifstream* file);

	protected:

	//Data

	public:

	protected:

		double lastSpawn;

		std::vector<std::string> codes;
		std::string id;

		int currentIndex;

		Condition* condition;

	};

}