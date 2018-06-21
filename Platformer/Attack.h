#pragma once

//Includes
#include <SDL.h>
#include <string>
#include <vector>

//Custom Includes
#include "GameObject.h"

namespace Platformer
{

	class Level;

	class HitComponent
	{

	//Methods

	public:

		HitComponent(Attack* a);

		virtual void Hit(GameEntity* entity) = 0;
		virtual int Damage();

	protected:

	//Data

	public:

	protected:

		Attack* parent;

	};

	class DamageComponent : public HitComponent
	{

	//Methods

	public:

		DamageComponent(Attack* a, int d);

		virtual void Hit(GameEntity* entity);
		virtual int Damage();

	protected:

	//Data

	public:

	protected:

		int damage;

	};

	class BindingComponent : public HitComponent
	{

		//Methods

	public:

		BindingComponent(Attack* a);

		virtual void Hit(GameEntity* entity);

	protected:

	//Data

	public:

	protected:

	};

	class Attack : public GameObject
	{

	//Methods

	public:

		Attack();
		Attack(GameEntity* p, Level* l, int d, float x, float y, float w, float h);

		virtual ~Attack();

		void LoadSpritesheet(std::string fileName, SDL_Texture* surface);

		virtual void Update(std::vector<GameEntity*> entities, float deltaTime);
		virtual bool RemoveCondition();

		virtual void Render(Vector2 world, float deltaTime);

		void CheckForHits(std::vector<GameEntity*> entities, float deltaTime);
		void Hit(GameEntity* entity);

		void Remove();
		virtual bool ShouldRemove(Vector2 world);

		virtual bool Active();

		Spritesheet* Sheet();
		void SetParent(GameEntity* p);
		void SetLevel(Level* l);
		GameEntity* Parent();

		int Damage();

		virtual std::string Type();

	protected:

	//Data

	public:

	protected:

		HitComponent* component;

		bool remove = false;

		GameEntity* parent;
		Level* level;

		Spritesheet sheet;
		Vector2 offset;

	};

	class BodyAttack : public Attack
	{

	//Methods

	public:

		BodyAttack();
		BodyAttack(GameEntity* p, Level* l, int d, float x, float y, float w, float h);
		~BodyAttack();

		void Update(std::vector<GameEntity*> entities, float deltaTime);
		bool RemoveCondition();

		bool Active();

		std::string Type();

	protected:

	//Data

	public:

	protected:

	};

	class MeleeAttack : public Attack
	{

	//Methods

	public:

		MeleeAttack();
		MeleeAttack(GameEntity* p, Level* l, int d, float x, float y, float w, float h, float lOX, float lOY, float rOX, float rOY, int index);
		~MeleeAttack();

		void Update(std::vector<GameEntity*> entities, float deltaTime);
		bool RemoveCondition();
		bool ShouldRemove(Vector2 world);

		bool Active();

		MeleeAttack* Clone(GameEntity* p);

		std::string Type();

	protected:

		Vector2 Offset();

	//Data

	public:

	protected:

		Vector2 lOffset;
		Vector2 rOffset;

		int animIndex;
		int damage;

	};

	class Bullet : public Attack
	{

	//Methods

	public:

		Bullet();
		Bullet(GameEntity* p, Level* l, int hit, int d, float x, float y, float w, float h);
		~Bullet();

		void Update(std::vector<GameEntity*> entities, float deltaTime);
		bool RemoveCondition();
		bool ShouldRemove(Vector2 world);

		void Render(Vector2 world, float deltaTime);

		std::string Type();
		void SetType(std::string t);

		Bullet* Clone(Vector2 pos, int direction);

		bool Active();

	protected:

	//Data

	public:

	protected:

		int hits;

		std::string type;

		float offset;

	};

}