#pragma once

//Includes
#include <SDL_mixer.h>
#include <vector>
#include <fstream>

//Custom Includes
#include "AABB.h"
#include "Spritesheet.h"

using namespace Engine2D;

#define GRAVITY 1200

#define CODE_DELIMITER '-'

#define BODY_ATTACK_CODE "BA"
#define BODY_ATTACK_CODE_LENGTH 2

namespace Platformer
{

	class Attack;

	class Level;

	class GameObject : public AABB
	{

	//Methods

	public:

		GameObject();
		GameObject(float x, float y, float w, float h);
		virtual ~GameObject();

		void Move(Vector2 m);

		Vector2 Velocity();
		void SetVelocity(Vector2 v);

		Vector2 RawVelocity();
		void SetRawVelocity(Vector2 r);

		virtual bool Active();

		virtual void Pause();
		virtual void Unpause();
		double PauseTime(double time);

		virtual bool ShouldMove();

	protected:

	//Data

	public:

	protected:

		Vector2 velocity;
		Vector2 rawVelocity;

		bool paused;
		double pauseTime;

	};

	class GameEntity : public GameObject
	{

	//Methods

	public:

		GameEntity();
		GameEntity(Level* l, int mH, int d, float x, float y, float w, float h);
		virtual ~GameEntity();

		void LoadEntity(std::ifstream& file, bool close = false);
		void LoadSpritesheet(std::string file, std::string imageFile);

		virtual void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		virtual void Die();

		bool Active();
		void SetActive(bool a);

		double LastHit();
		float HitInvulnerability();

		virtual void Hurt(Attack* a);
		virtual bool IsHurt();

		void PlayAnimation(int index, bool loop);

		virtual void Render(Vector2 world, float deltaTime);

		virtual void Unpause();

		virtual const char* Type();

		void SetLevel(Level* l);
		void SetOffset(Vector2 o);
		void SetDirection(int d);

		int Direction();
		float HealthPercentage();
		int Health();
		int MaxHealth();
		Vector2 Offset();

		void MaximizeHealth();

		Level* GetLevel();

		virtual void RequestEvent(std::string e);

		Spritesheet* Sheet();

		void DataCopy(GameEntity* entity);

	protected:

		virtual void Load(std::ifstream& file);

	//Data

	public:

	protected:

		int health = 0;
		int maxHealth = 0;
		int direction = 1;

		bool active = true;

		double lastHit = 0;
		float hitInvulnerability = 0;

		Level* level;

		Spritesheet sheet;
		Spritesheet koSheet;
		Vector2 lOffset, rOffset;

		Mix_Chunk* hurtEffect;
		bool loadedSounds = true;

	};

}
