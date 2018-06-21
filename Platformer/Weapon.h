#pragma once

//Includes
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
#include <fstream>

//Custom Includes
#include "Vector2.h"
#include "Spritesheet.h"
#include "Attack.h"

using namespace Engine2D;

namespace Platformer
{

	class Weapon
	{

	//Methods
	
	public:

		Weapon(GameEntity* p, float r);
		virtual ~Weapon();

		static Weapon* LoadWeapon(std::ifstream& file, GameEntity* p);

		void Use(std::vector<Attack*>& attacks, Vector2 direction = Vector2(1, 0));

		bool Ready();

		void Pause();
		void Unpause();

		double PauseTime(double time);

		virtual Weapon* Clone(GameEntity* p) = 0;

		virtual void Refresh() = 0;

	protected:

		virtual void Fire(std::vector<Attack*>& attacks, Vector2 direction) = 0;
		virtual void Load(std::ifstream& file) = 0;

		virtual void DataCopy(Weapon* weapon);

	//Data
	
	public:

	protected:

		GameEntity* parent;

		float rateOfUse;
		double lastUsed; 

		double pauseTime;
		bool paused;
		
		Mix_Chunk* useEffect = NULL;
		bool loadedSounds = true;

	};

	class Melee : public Weapon
	{

	//Methods

	public:

		Melee(GameEntity* p, int d, float w, float h, float lX, float lY, float rX, float rY, float r, int i);
		Melee(GameEntity* p, float r, MeleeAttack* m);
		~Melee();

		Weapon* Clone(GameEntity* p);

		void Refresh();

	protected:

		void Fire(std::vector<Attack*>& attacks, Vector2 direction);
		void Load(std::ifstream& file);

	//Data

	public:

	protected:

		MeleeAttack* melee;

	};

	class Projectile : public Weapon
	{
	
	//Methods
	
	public:

		Projectile(GameEntity* p, float lX, float lY, float rX, float rY, float r);
		~Projectile();

		Vector2 Offset();

		Weapon* Clone(GameEntity* p);

		void Refresh();

	protected:

		void Fire(std::vector<Attack*>& attacks, Vector2 direction);
		void Load(std::ifstream& file);

	//Data
	
	public:

	protected:

		Vector2 leftOffset;
		Vector2 rightOffset;

		Bullet* bullet;

	};

	class Multishot : public Weapon
	{

	//Methods

	public:

		Multishot(GameEntity* p, float lX, float lY, float rX, float rY, float r);
		~Multishot();

		Vector2 Offset();

		Weapon* Clone(GameEntity* p);

		void Refresh();

	protected:

		void Fire(std::vector<Attack*>& attacks, Vector2 direction);
		void Load(std::ifstream& file);

	//Data

	public:

	protected:

		Vector2 leftOffset;
		Vector2 rightOffset;

		std::vector<Bullet*> bullets;

	};

	class SpawnGun : public Weapon
	{

	//Methods

	public:

		SpawnGun(GameEntity* p, float lX, float lY, float rX, float rY, float r);
		~SpawnGun();

		Vector2 Offset();

		Weapon* Clone(GameEntity* p);

		void Refresh();

	protected:

		void Fire(std::vector<Attack*>& attacks, Vector2 direction);
		void Load(std::ifstream& file);

	//Data

	public:

	protected:

		Vector2 leftOffset;
		Vector2 rightOffset;

		std::string code;

	};

}
