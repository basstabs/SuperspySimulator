#pragma once

//Includes
#include <fstream>
#include <vector>

//Custom Includes
#include "AABB.h"
#include "Player.h"
#include "CollisionMap.h"
#include "UI.h"

using namespace Engine2D;

#define TRIGGER_TRANSPARENCY_MAX 192
#define TRIGGER_TRANSPARENCY_RATE 384

namespace Platformer
{

	struct PauseText
	{

		std::vector<std::pair<Vector2, std::string>> text;
		std::vector<std::pair<Vector2, int>> inputs;

	};


	class TriggerCondition
	{

	//Methods

	public:

		TriggerCondition();
		virtual ~TriggerCondition();

		virtual bool Condition(Player* player) = 0;

		virtual void Render(UI* ui, float transparency, float deltaTime) = 0;

		static TriggerCondition* TriggerConditionFactory(std::ifstream& file, Level* l);

	protected:

	//Data

	public:

	protected:

	};

	class AndTrigger : public TriggerCondition
	{

	//Methods

	public:

		AndTrigger(TriggerCondition* p, TriggerCondition* q);
		~AndTrigger();

		bool Condition(Player* player);

		void Render(UI* ui, float transparency, float deltaTime);

	protected:

	//Data

	public:

	protected:

		TriggerCondition* p;
		TriggerCondition* q;

	};

	class SingleTrigger : public TriggerCondition
	{

	//Methods

	public:

		SingleTrigger();
		~SingleTrigger();

		bool Condition(Player* player);

		void Render(UI* ui, float transparency, float deltaTime);

	protected:

	//Data

	public:

	protected:

		bool fired = false;

	};

	class ItemTrigger : public TriggerCondition
	{

	//Methods

	public:

		ItemTrigger(std::string i, int c);
		~ItemTrigger();

		bool Condition(Player* player);

		void Render(UI* ui, float transparency, float deltaTime);

	protected:

	//Data

	public:

	protected:

		std::string item;
		int count;

	};

	class InputTrigger : public TriggerCondition
	{

	//Methods

	public:

		InputTrigger(std::vector<int> in);
		~InputTrigger();

		bool Condition(Player* player);

		void Render(UI* ui, float transparency, float deltaTime);

	protected:

	//Data

	public:

	protected:

		std::vector<int> indices;

	};

	class DefeatTrigger : public TriggerCondition
	{

	//Methods

	public:

		DefeatTrigger(std::string n, Platformer* p);
		~DefeatTrigger();

		bool Condition(Player* player);

		void Render(UI* ui, float transparency, float deltaTime);

	protected:

	//Data

	public:

	protected:

		std::string name;
		Platformer* platformer;

	};

	class Trigger : public AABB
	{

	//Methods

	public:

		Trigger(float x, float y, float w, float h);
		virtual ~Trigger();

		virtual void Update(Player* player, float deltaTime);

		void SetCondition(TriggerCondition* cond);

		void Render(UI* ui, float deltaTime);

		static Trigger* TriggerFactory(std::ifstream& file, Level* level, std::string n);

	protected:

	//Data

	public:

	protected:

		TriggerCondition* condition;

		float transparency;

		bool display = true;

	};

	class PlatformTrigger : public Trigger
	{

	//Methods

	public:

		PlatformTrigger(CollisionMap* col, int i, float x, float y, float w, float h);
		~PlatformTrigger();

		void Update(Player* player, float deltaTime);

	protected:

	//Data

	public:

	protected:

		CollisionMap* collisionMap;
		int index;
		
	};

	class DoorTrigger : public Trigger
	{

	//Methods

	public:

		DoorTrigger(std::string l, int s, float x, float y, float w, float h);
		~DoorTrigger();

		void Update(Player* player, float deltaTime);

	protected:

	//Data

	public:

	protected:

		std::string level;
		int spawnIndex;

	};

	class ExitTrigger : public Trigger
	{

	//Methods

	public:

		ExitTrigger(float x, float y, float w, float h);
		~ExitTrigger();

		void Update(Player* player, float deltaTime);

	protected:

	//Data

	public:

	protected:

	};

	class SoundTrigger : public Trigger
	{

	//Methods

	public:

		SoundTrigger(std::string e, float x, float y, float w, float h);
		~SoundTrigger();

		void Update(Player* player, float deltaTime);

	protected:

	//Data

	public:

	protected:

		Mix_Chunk* effect;

	};

	class TutorialTrigger : public Trigger
	{

	//Methods

	public:

		TutorialTrigger(PauseText t, float x, float y, float w, float h);
		~TutorialTrigger();

		void Update(Player* player, float deltaTime);

	protected:

	//Data

	public:

	protected:

		PauseText tutorial;

	};

	class TriggerTrigger : public Trigger
	{

	//Methods

	public:

		TriggerTrigger(Trigger* t, std::string n, Level* lvl, bool f, int i, int l, float x, float y, float w, float h);
		~TriggerTrigger();

		void Update(Player* player, float deltaTime);

	protected:

	//Data

	public:

	protected:

		Trigger* trigger;
		Level* level;

		bool foreground;
		int imageIndex;
		int locationIndex;

		std::string name;

	};

	class Breakable : public GameEntity
	{

	//Methods

	public:

		Breakable(std::string c);
		Breakable(Level* l, std::string c, int mH, int d, float x, float y, float w, float h);
		~Breakable();

		void Die();

		const char* Type();

		bool ShouldMove();

	protected:

	//Data

	public:

	protected:

		std::string code = "";

	};


}