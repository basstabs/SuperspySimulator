#pragma once

//Includes
#include <SDL_mixer.h>
#include <vector>

//Custom Includes
#include "GameObject.h"
#include "Attack.h"
#include "ScriptedAction.h"
#include "Weapon.h"
#include "Items.h"
#include "SaveData.h"

#define INVULNERABILITY_DURATION 250

#define PLAYER_SPRITE_WIDTH 54
#define PLAYER_SPRITE_HEIGHT 54

#define PLAYER_MAX_JUMP_LENGTH 10
#define PLAYER_COYOTE_TIME 6
#define PLAYER_ONEWAY_TIME 50

#define PLAYER_HURT_SPEED 200

#define PLAYER_ANIM_STAND 0
#define PLAYER_ANIM_RUN 2
#define PLAYER_ANIM_JUMP 4
#define PLAYER_ANIM_FALL 6
#define PLAYER_ANIM_HURT 8

#define PLAYER_ANIM_CAPTURE 9
#define PLAYER_ANIM_HOP_STAND 10
#define PLAYER_ANIM_HOP_JUMP 11
#define PLAYER_ANIM_HOP_FALL 12

#define BIND_DURATION 5000
#define BIND_INCREASE 1000

namespace Platformer
{

	class Platformer;

	class Player : public GameEntity
	{

	friend class HopAction;

	//Methods

	public:

		Player(Platformer* p);
		Player(Platformer* p, Level* l, int mH, int d, float x, float y, float w, float h);
		~Player();

		void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		void CanJump();

		void Shoot(std::vector<Attack*>& attacks);
		void Die();

		void Pause();
		void Unpause();

		const char* Type();

		bool OneWay();

		void Hurt(Attack* a);
		bool IsHurt();
		bool Scripted();

		int Respawn();

		void RequestEvent(std::string e);
		void AddItem(Item* i);
		std::vector<Item*> Inventory();

		bool HasItems(std::string type, int count);

		friend void SaveData::AugmentPlayer(Player* p);

	protected:

		void Load(std::ifstream& file);

		bool ShouldMove();

	//Data

	public:

	protected:

		bool jump = false;
		bool jumpKeyUp = true;

		bool oneWay = true;
		double oneWayTime;

		float movementSpeed = 200;
		float jumpSpeed = -250;
		float jumpAcceleration = -2400;
		int jumpTime;

		ScriptedAction* scripted;

		Weapon* weapon;

		Mix_Chunk* jumpEffect;
		
		Platformer* parent;

		std::vector<Item*> inventory;

		bool crushed = false;

		int timesBound = 0;

	};

}