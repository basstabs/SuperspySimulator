//Includes
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "Event.h"
#include "Level.h"
#include "Platformer.h"

namespace Platformer
{

	TriggerCondition::TriggerCondition()
	{

	}
	
	TriggerCondition::~TriggerCondition()
	{

	}

	TriggerCondition* TriggerCondition::TriggerConditionFactory(std::ifstream& file, Level* l)
	{

		TriggerCondition* newCondition = NULL;

		std::string type;
		std::getline(file, type);

		if (type == "input")
		{

			std::string line;
			std::getline(file, line);

			std::istringstream input(line);

			int numButtons;
			input >> numButtons;

			std::getline(file, line);
			input.clear();
			input.str(line);

			std::vector<int> inputs;

			for (int i = 0; i < numButtons; i++)
			{

				int in;
				input >> in;
				inputs.push_back(in);

			}

			newCondition = new InputTrigger(inputs);

		}
		else if (type == "defeat")
		{

			std::string line;
			std::getline(file, line);

			newCondition = new DefeatTrigger(line, l->GetPlatformer());

		}
		else if (type == "item")
		{

			std::string line;
			std::getline(file, line);

			std::istringstream input(line);

			int count;

			input >> line;
			input >> count;

			newCondition = new ItemTrigger(line, count);

		}
		else if (type == "and")
		{

			newCondition = new AndTrigger(TriggerCondition::TriggerConditionFactory(file, l), TriggerCondition::TriggerConditionFactory(file, l));

		}
		else
		{

			newCondition = new SingleTrigger();

		}

		return newCondition;

	}

	AndTrigger::AndTrigger(TriggerCondition* p, TriggerCondition* q)
	{

		this->p = p;
		this->q = q;

	}

	AndTrigger::~AndTrigger()
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

	bool AndTrigger::Condition(Player* player)
	{

		return this->p->Condition(player) && this->q->Condition(player);

	}

	void AndTrigger::Render(UI* ui, float transparency, float deltaTime)
	{

	}

	SingleTrigger::SingleTrigger()
	{

		this->fired = false;

	}

	SingleTrigger::~SingleTrigger()
	{

	}

	bool SingleTrigger::Condition(Player* player)
	{

		if (!this->fired)
		{

			this->fired = true;

			return true;

		}

		return false;

	}

	void SingleTrigger::Render(UI* ui, float transparency, float deltaTime)
	{

	}

	InputTrigger::InputTrigger(std::vector<int> i)
	{

		this->indices = i;

	}

	InputTrigger::~InputTrigger()
	{

	}

	bool InputTrigger::Condition(Player* player)
	{

		bool trigger = true;
		for (int i = 0; i < this->indices.size(); i++)
		{

			trigger = trigger && Settings::AccessSettings()->Controls()->Down(this->indices[i]);

		}

		return trigger;

	}

	void InputTrigger::Render(UI* ui, float transparency, float deltaTime)
	{

		ui->RenderInput(this->indices, transparency, deltaTime);

	}

	DefeatTrigger::DefeatTrigger(std::string n, Platformer* p)
	{

		this->name = n;
		this->platformer = p;

	}

	DefeatTrigger::~DefeatTrigger()
	{

	}

	bool DefeatTrigger::Condition(Player* player)
	{

		return !this->platformer->SearchType(this->name);

	}

	void DefeatTrigger::Render(UI* ui, float transparency, float deltaTime)
	{

	}

	ItemTrigger::ItemTrigger(std::string i, int c)
	{

		this->item = i;
		this->count = c;

	}

	ItemTrigger::~ItemTrigger()
	{

	}

	bool ItemTrigger::Condition(Player* player)
	{

		return player->HasItems(this->item, this->count);

	}

	void ItemTrigger::Render(UI* ui, float transparency, float deltaTime)
	{

	}

	Trigger::Trigger(float x, float y, float w, float h) : AABB(x, y, w, h)
	{

	}

	Trigger::~Trigger()
	{

		if (this->condition)
		{

			delete this->condition;
			this->condition = NULL;

		}

	}

	void Trigger::SetCondition(TriggerCondition* cond)
	{

		this->condition = cond;

	}

	void Trigger::Update(Player* player, float deltaTime)
	{

		this->transparency += TRIGGER_TRANSPARENCY_RATE * deltaTime * (this->Intersects(player) ? 1 : -1);
		this->transparency = fmaxf(fminf(TRIGGER_TRANSPARENCY_MAX, this->transparency), 0);

		this->display = (!player->Scripted() && player->Health() > 0);

	}

	void Trigger::Render(UI* ui, float deltaTime)
	{

		if (display)
		{

			this->condition->Render(ui, this->transparency, deltaTime);

		}

	}

	Trigger* Trigger::TriggerFactory(std::ifstream& file, Level* level, std::string n)
	{

		Trigger* newTrigger = NULL;

		std::string line;
		std::getline(file, line);

		std::istringstream input(line);

		float x, y, w, h;
		input >> x;
		input >> y;
		input >> w;
		input >> h;

		std::string type;
		std::getline(file, type);

		if (type == "platform")
		{

			int index;

			std::getline(file, line);
			input.clear();
			input.str(line);

			input >> index;

			newTrigger = new PlatformTrigger(level->Collision(), index, x, y, w, h);

		}
		else if (type == "door")
		{

			int spawn;
			std::string level;

			std::getline(file, level);
			std::getline(file, line);

			input.clear();
			input.str(line);

			input >> spawn;

			newTrigger = new DoorTrigger(level, spawn, x, y, w, h);

		}
		else if (type == "sound")
		{

			std::getline(file, line);
			
			newTrigger = new SoundTrigger(line, x, y, w, h);

		}
		else if (type == "exit")
		{

			newTrigger = new ExitTrigger(x, y, w, h);

		}
		else if (type == "trigger")
		{

			Trigger* trigger = TriggerFactory(file, level, n);

			std::getline(file, line);
			input.clear();
			input.str(line);

			bool fore;
			int imageIndex, locationIndex;

			input >> fore;
			input >> imageIndex;
			input >> locationIndex;

			newTrigger = new TriggerTrigger(trigger, n, level, fore, imageIndex, locationIndex, x, y, w, h);

		}
		else
		{

			newTrigger = new Trigger(x, y, w, h);

		}

		newTrigger->SetCondition(TriggerCondition::TriggerConditionFactory(file, level));

		return newTrigger;

	}

	PlatformTrigger::PlatformTrigger(CollisionMap* col, int i, float x, float y, float w, float h) : Trigger(x, y, w, h)
	{

		this->collisionMap = col;
		this->index = i;

	}

	PlatformTrigger::~PlatformTrigger()
	{
		
	}

	void PlatformTrigger::Update(Player* player, float deltaTime)
	{

		this->Trigger::Update(player, deltaTime);

		if (this->Intersects(player) && this->condition->Condition(player))
		{

			this->collisionMap->Trigger(this->index);

		}

	}

	DoorTrigger::DoorTrigger(std::string l, int s, float x, float y, float w, float h) : Trigger(x, y, w, h)
	{

		this->level = l;
		this->spawnIndex = s;

	}

	DoorTrigger::~DoorTrigger()
	{

	}

	void DoorTrigger::Update(Player* player, float deltaTime)
	{

		this->Trigger::Update(player, deltaTime);

		if (this->Intersects(player) && this->condition->Condition(player) && !player->Scripted())
		{

			player->RequestEvent("Door " + this->level + " " + std::to_string(this->spawnIndex));

		}

	}

	ExitTrigger::ExitTrigger(float x, float y, float w, float h) : Trigger(x, y, w, h)
	{

	}

	ExitTrigger::~ExitTrigger()
	{

	}

	void ExitTrigger::Update(Player* player, float deltaTime)
	{

		this->Trigger::Update(player, deltaTime);

		if (this->Intersects(player) && this->condition->Condition(player))
		{

			player->RequestEvent("Exit");

		}

	}

	SoundTrigger::SoundTrigger(std::string e, float x, float y, float w, float h) : Trigger(x, y, w, h)
	{

		this->effect = LoadSound(Settings::AccessSettings()->SoundVolume(), e);

	}

	SoundTrigger::~SoundTrigger()
	{

		if (this->effect)
		{

			Mix_FreeChunk(this->effect);
			this->effect = NULL;

		}

	}

	void SoundTrigger::Update(Player* player, float deltaTime)
	{

		this->Trigger::Update(player, deltaTime);

		if (this->condition->Condition(player))
		{

			PlaySound(this->effect, this, false);

		}

	}

	TriggerTrigger::TriggerTrigger(Trigger* t, std::string n, Level* lvl, bool f, int i, int l, float x, float y, float w, float h) : Trigger(x, y, w, h)
	{

		this->trigger = t;
		this->level = lvl;

		this->foreground = f;
		this->imageIndex = i;
		this->locationIndex = l;

		this->name = n;

	}

	TriggerTrigger::~TriggerTrigger()
	{

		if (this->trigger)
		{

			delete this->trigger;
			this->trigger = NULL;

		}

	}

	void TriggerTrigger::Update(Player* player, float deltaTime)
	{

		this->Trigger::Update(player, deltaTime);

		if (this->Intersects(player) && this->condition->Condition(player) && !player->Scripted())
		{

			if (this->trigger)
			{

				this->level->AddTrigger(this->trigger);
				this->level->RemoveGround(this->foreground, this->imageIndex, this->locationIndex);

				UI::AccessUI()->PlayItemEffect();

				this->trigger = NULL;

				this->level->Block(this->name);

			}

		}

	}

	Breakable::Breakable(std::string c) : GameEntity()
	{

		this->code = c;

	}

	Breakable::Breakable(Level* l, std::string c, int mH, int d, float x, float y, float w, float h) : GameEntity(l, mH, d, x, y, w, h)
	{

		this->code = c;

	}

	Breakable::~Breakable()
	{

	}

	void Breakable::Die()
	{

		this->level->Collision()->Unregister(this->code);
		this->GameEntity::Die();

	}

	const char* Breakable::Type()
	{

		return this->code.c_str();

	}

	bool Breakable::ShouldMove()
	{

		return false;

	}

}