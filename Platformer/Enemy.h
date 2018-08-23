#pragma once

//Includes
#include <vector>
#include <map>
#include <string>
#include <fstream>

//Custom Includes
#include "GameObject.h"
#include "Attack.h"
#include "Weapon.h"

#define CHASER_FLOAT_ANIM 0
#define CHASER_MOVE_ANIM 1
#define CHASER_ATTACK_ANIM 2

#define FSM_IDLE_ANIM 0

namespace Platformer
{

	class Enemy : public GameEntity
	{

	//Methods

	public:

		Enemy();
		Enemy(Level* l, int mH, int d, float x, float y, float w, float h);
		virtual ~Enemy();

		void Die();

		virtual Enemy* Clone(Vector2 location, std::string type) = 0;

		static void Spawn(Vector2 location, std::string type, std::string code, std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static Enemy* EnemyFactory(std::ifstream* file, std::string code, Level* l);

		const char* Type();

	protected:

	//Data

	public:

	protected:

		int reputation;

		std::string type;

	};

	class Emplacement : public Enemy
	{

	//Methods

	public:

		Emplacement();
		Emplacement(Level* l, int mH, int d, float x, float y, float w, float h, float fX, float fY);
		~Emplacement();

		void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		virtual Enemy* Clone(Vector2 location, std::string type);

		void SetWeapon(Weapon* w);

		void SetFiringDirection(float x, float y);

		void Pause();
		void Unpause();

	protected:

	//Data

	public:

	protected:

		Weapon* weapon;

		Vector2 firingDirection = Vector2(1, 0);

	};

	class Patrolman : public Enemy
	{

	//Methods

	public:

		Patrolman();
		Patrolman(Level* l, int mH, int d, float x, float y, float w, float h, float mn, float mx, float s);
		~Patrolman();

		void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		virtual Enemy* Clone(Vector2 location, std::string type);

		void SetVision(AABB v);
		void SetWeapon(Weapon* w);

		void SetMin(float m);
		void SetMax(float m);
		void SetSpeed(float s);

		void SetFiringDirection(float x, float y);

		void Pause();
		void Unpause();

	protected:

		float min, max, speed;

		AABB vision;
		Weapon* weapon;

		float firingX, firingY;

	//Data

	public:

	protected:

	};

	class Chaser : public Enemy
	{

	//Methods

	public:

		Chaser();
		Chaser(Level* l, int mH, int d, float x, float y, float w, float h);
		~Chaser();

		void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		virtual Enemy* Clone(Vector2 location, std::string type);

		void AddToPath(Vector2 p);
		void SetRange(AABB r);
		void SetZone(AABB z);

		void SetOffset(float o);
		void SetSpeed(float s);

		void SetWait(double w);

		void SetWeapon(Weapon* w);

		void Pause();
		void Unpause();

	protected:

	//Data

	public:

	protected:

		std::vector<Vector2> path;
		AABB range;
		AABB zone;

		float yOffset, speed;

		int chasingIndex, pathIndex, pathDirection;

		double leaveTime = -1;
		double waitTime;

		Weapon* weapon;

	};

	class Turret : public Enemy
	{

	//Methods

	public:

		Turret();
		Turret(Level* l, int mH, int d, float x, float y, float w, float h);
		~Turret();

		void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		virtual Enemy* Clone(Vector2 location, std::string type);

		void SetMinAngle(float a);
		void SetMaxAngle(float a);
		void SetAngleOffset(float o);
		void SetRange(float r);

		void SetWeapon(Weapon* w);

		void Pause();
		void Unpause();

	protected:

	//Data

	public:

	protected:

		float minAngle;
		float maxAngle;
		float angleOffset;
		float range;

		Weapon* weapon;

	};

	class Mover : public Enemy
	{

	//Methods

	public:

		Mover();
		Mover(Level* l, int mH, int d, float x, float y, float w, float h);
		~Mover();

		void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		virtual Enemy* Clone(Vector2 location, std::string type);

		void SetType(std::string t);
		void PushParameter(float p);

	protected:

	//Data

	public:

		void UpdateCircle(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		void UpdateConstant(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		void UpdateSine(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

	protected:

		std::string type;
		std::vector<float> parameters;

	};

	struct DecisionNode
	{

		std::string value;

		int left;
		int right;

	};

	class FSM : public Enemy
	{

	//Methods

	public:

		FSM();
		FSM(Level* l, int mH, int d, float x, float y, float w, float h);
		~FSM();

		virtual void Hurt(Attack* a);

		void Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		virtual Enemy* Clone(Vector2 location, std::string type);

		void AddState(std::string s, int anim, bool loop);
		void AddNode(DecisionNode node);
		void PushParameter(std::string s, float p);

		void SetWeapon(Weapon* w);
		void SetInvulnerableEffect(Mix_Chunk* e);
		void SetDecisionDelay(float d);

		void SetHurtBox(AABB box);
		void SetHurtOffset(Vector2 o);

		void Pause();
		void Unpause();

	protected:

		static void Initialize();

		static bool Evaluate(FSM* fsm, DecisionNode* node);

		static void SwitchFSM(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

		static void FSMCharge(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static void FSMTurn(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static void FSMShoot(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static void FSMGoto(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static void FSMIdle(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static void FSMHurt(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static void FSMAmbush(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);
		static void FSMSelfDestruct(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime);

	//Data

	public:

	protected:

		static std::map<std::string, void (*)(FSM*, std::vector<GameEntity*>&, std::vector<Attack*>&, float)> states;

		static bool init;

		std::vector<std::string> scripted;

		std::map<std::string, std::vector<float>> fsm;
		std::map<std::string, std::pair<int, bool>> stateAnimations;
		std::vector<DecisionNode> nodes;
		float decisionDelay;
		double lastSwitch;

		AABB hurtBox;
		Vector2 hurtOffset;

		Mix_Chunk* invulnerableEffect;

		std::vector<Weapon*> weapons;

		std::string choice = "";
		std::string lastChoice = "";

		bool hurt = false;

	};

}
