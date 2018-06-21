#pragma once

//Includes
#include <vector>

//Custom Includes
#include "Attack.h"
#include "UI.h"

#define ESCAPE_START_TIME 2000
#define ESCAPE_INTERVAL 1000
#define ESCAPE_TRANSPARENCY 192
#define ESCAPE_MAX_DIFFICULTY 10.0
#define ESCAPE_DURATION 1500
#define ESCAPE_BASE_DIVISION 2

namespace Platformer
{

	class Player;

	class ScriptedAction
	{

	//Methods

	public:

		ScriptedAction();
		ScriptedAction(float d);
		virtual ~ScriptedAction();

		void Start();

		virtual void Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime) = 0;
		virtual void Render();

		virtual bool Active();

		virtual void HandleAnimation(float deltaTime);

		virtual std::string Type() = 0;

		void Pause();
		void Unpause();

	protected:

	//Data

	public:

	protected:

		double startTime;
		float duration;

		bool paused = false;
		double pauseTime;

	};

	class PlayerHurtAction : public ScriptedAction
	{

	//Methods

	public:

		PlayerHurtAction();
		PlayerHurtAction(int direction, float s, float d);
		~PlayerHurtAction();

		std::string Type();

		void Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime);

	protected:

	//Data

	public:

	protected:

		int direction;
		float speed;

	};

	class FallingKOAction : public ScriptedAction
	{

	//Methods

	public:

		FallingKOAction();
		FallingKOAction(float d, bool fall);
		~FallingKOAction();

		std::string Type();

		void Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime);

		bool Active();

	public:

	protected:

	//Data

	public:

	protected:

		bool falling = false;
		bool play = false;

	};

	class HopAction : public ScriptedAction
	{

	//Methods

	public:

		HopAction(Player* p);
		HopAction(Player* p, float d);
		~HopAction();

		std::string Type();

		void Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime);

		void HandleAnimation(float deltaTime);

	protected:

	//Data

	public:

	protected:

		bool jumpKeyUp = true;

		Player* parent;
		bool captured = false;

	};

	class EscapeMinigame : public ScriptedAction
	{

	//Methods

	public:

		EscapeMinigame(int n, Player* p);
		EscapeMinigame(int n, Player* p, float d);
		~EscapeMinigame();

		std::string Type();

		void Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime);

		void Render();

		bool Active();

		double EscapeDuration();

		static void Reset();

	protected:

		void PickButton();

	//Data

	public:

	protected:

		static int hard;
		int difficulty;

		Player* parent;

		int button = -1;

		float success;

		double lastButton = 0;

		SDL_Texture* base;
		SDL_Texture* filler;
		SDL_Texture* hair;

	};

}