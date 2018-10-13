//Includes
#include <sstream>
#include <math.h>

//Custom Includes
#include "Helpers.h"
#include "Enemy.h"
#include "Settings.h"
#include "SaveData.h"
#include "Level.h"

namespace Platformer
{

	float AdjustAngle(float a)
	{

		if (a < -M_PI)
		{

			a += 2 * M_PI;

		}

		if (a > M_PI)
		{

			a -= 2 * M_PI;

		}

		return a;

	}

	bool LessEqual(float l, float r)
	{

		return l <= r;

	}

	bool GreaterEqual(float l, float r)
	{

		return l >= r;

	}

	bool Equal(float l, float r)
	{

		return l == r;

	}

	bool NotEqual(float l, float r)
	{

		return l != r;

	}

	std::map<std::string, bool (*)(float, float)> compareFunctions = std::map<std::string, bool (*)(float, float)>();

	std::map<std::string, void (*)(FSM*, std::vector<GameEntity*>&, std::vector<Attack*>&, float)> FSM::states = std::map<std::string, void (*)(FSM*, std::vector<GameEntity*>&, std::vector<Attack*>&, float)>();
	bool FSM::init = false;

	Enemy::Enemy() : GameEntity()
	{

	}

	Enemy::Enemy(Level* l, int mH, int d, float x, float y, float w, float h) : GameEntity(l, mH, d, x, y, w, h)
	{

	}

	Enemy::~Enemy()
	{

	}

	void Enemy::Die()
	{

		this->active = false;

		this->level->EnemyDied(this->Center());

		SaveData::AccessSaveData()->AddAtRisk(this->reputation);

	}

	void Enemy::Spawn(Vector2 location, std::string type, std::string code, std::map<std::string, Enemy*> master, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Enemy* enemy = master[code]->Clone(location, type);
		enemy->type = type;
		entities.push_back(enemy);

		enemy->reputation = master[code]->reputation;

		enemy->lastHit = GetTime() - enemy->HitInvulnerability();

		std::string::size_type body = code.find(BODY_ATTACK_CODE);
		if(body != std::string::npos)
		{

			std::istringstream damageStream(code.substr(body + BODY_ATTACK_CODE_LENGTH, code.find_first_of(CODE_DELIMITER, body) - (body + BODY_ATTACK_CODE_LENGTH)));
			int damage;
			damageStream >> damage;

			Attack* bodyAttack = new BodyAttack(enemy, enemy->level, damage, enemy->X(), enemy->Y(), enemy->Width(), enemy->Height());
			attacks.push_back(bodyAttack);

		}

	}

	const char* Enemy::Type()
	{

		return this->type.c_str();

	}

	Enemy* Enemy::EnemyFactory(std::ifstream* file, std::string code, Level* l)
	{

		Enemy* newEnemy = NULL;

		std::string line;

		std::stringstream input;

		float x, y, w, h;

		std::string::size_type location = code.find_first_of(CODE_DELIMITER);
		std::string category = code.substr(location + 1, (code.find_first_of(CODE_DELIMITER, location + 1) - location - 1));

		if (category == "T")
		{

			Turret* turret = new Turret();
			turret->SetLevel(l);
			turret->LoadEntity(*file);

			float minA, maxA, offset, r;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> minA;
			input >> maxA;
			input >> offset;
			input >> r;

			turret->SetMinAngle((M_PI * minA) / 180.0);
			turret->SetMaxAngle((M_PI * maxA) / 180.0);
			turret->SetAngleOffset((M_PI * offset) / 180.0);
			turret->SetRange(r);

			Weapon* weapon = Weapon::LoadWeapon(*file, turret);
			turret->SetWeapon(weapon);

			newEnemy = turret;

		}
		else if (category == "Emp")
		{

			Emplacement* emp = new Emplacement();
			emp->SetLevel(l);
			emp->LoadEntity(*file);

			float fX, fY;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> fX;
			input >> fY;

			emp->SetFiringDirection(fX, fY);

			Weapon* weapon = Weapon::LoadWeapon(*file, emp);
			emp->SetWeapon(weapon);

			newEnemy = emp;

		}
		else if (category == "M")
		{

			Mover* mover = new Mover();
			mover->SetLevel(l);
			mover->LoadEntity(*file);

			std::string type;
			int numParameters;

			std::getline(*file, type);
			mover->SetType(type);

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> numParameters;

			for (int i = 0; i < numParameters; i++)
			{

				float plus = 0;

				std::getline(*file, line);
				input.clear();
				input.str(line);

				float p;
				input >> p;

				mover->PushParameter(p + plus);

			}

			newEnemy = mover;

		}
		else if (category == "P")
		{

			Patrolman* patrol = new Patrolman();
			patrol->SetLevel(l);
			patrol->LoadEntity(*file);

			float min, max, speed;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> min;
			input >> max;
			input >> speed;

			patrol->SetMin(min);
			patrol->SetMax(max);
			patrol->SetSpeed(speed);

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;
			input >> w;
			input >> h;

			patrol->SetVision(AABB(patrol->X() + x, patrol->Y() + y, w, h));

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;

			patrol->SetFiringDirection(x, y);

			Weapon* weapon = Weapon::LoadWeapon(*file, patrol);
			patrol->SetWeapon(weapon);

			newEnemy = patrol;

		}
		else if (category == "C")
		{

			Chaser* chaser = new Chaser();
			chaser->SetLevel(l);
			chaser->LoadEntity(*file);

			int numPath;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> numPath;

			for (int i = 0; i < numPath; i++)
			{

				std::getline(*file, line);
				input.clear();
				input.str(line);

				input >> x;
				input >> y;

				chaser->AddToPath(Vector2(chaser->X() + x, chaser->Y() + y));

			}

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;
			input >> w;
			input >> h;

			chaser->SetRange(AABB(x, y, w, h));

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;
			input >> w;
			input >> h;

			chaser->SetZone(AABB(x, y, w, h));

			std::getline(*file, line);
			input.clear();
			input.str(line);

			float o, s;
			double w;

			input >> o;
			input >> s;
			input >> w;

			chaser->SetOffset(o);
			chaser->SetSpeed(s);
			chaser->SetWait(w);

			Weapon* weapon = Weapon::LoadWeapon(*file, chaser);
			chaser->SetWeapon(weapon);

			newEnemy = chaser;

		}
		else if (category == "F")
		{

			FSM* fsm = new FSM();
			fsm->SetLevel(l);
			fsm->LoadEntity(*file);

			int numStates;

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> numStates;

			for (int i = 0; i < numStates; i++)
			{

				std::string state;
				std::getline(*file, state);

				std::getline(*file, line);
				input.clear();
				input.str(line);

				int anim;
				bool loop;

				input >> anim;
				input >> loop;

				fsm->AddState(state, anim, loop);

				std::getline(*file, line);
				input.clear();
				input.str(line);

				int numParams;
				input >> numParams;

				for (int j = 0; j < numParams; j++)
				{

					std::getline(*file, line);
					input.clear();
					input.str(line);

					float p;
					input >> p;

					fsm->PushParameter(state, p);

				}

			}

			std::getline(*file, line);
			input.clear();
			input.str(line);

			int numNodes;
			input >> numNodes;

			for (int i = 0; i < numNodes; i++)
			{

				DecisionNode node;
				std::getline(*file, node.value);

				std::getline(*file, line);
				input.clear();
				input.str(line);

				input >> node.left;
				input >> node.right;

				fsm->AddNode(node);

			}

			std::getline(*file, line);
			input.clear();
			input.str(line);

			float delay;
			input >> delay;

			fsm->SetDecisionDelay(delay);

			std::getline(*file, line);
			if (line.size() > 0)
			{

				fsm->SetInvulnerableEffect(LoadSound(Settings::AccessSettings()->SoundVolume(), line));

			}

			std::getline(*file, line);
			input.clear();
			input.str(line);

			int numWeapons;
			input >> numWeapons;

			for (int i = 0; i < numWeapons; i++)
			{

				fsm->SetWeapon(Weapon::LoadWeapon(*file, fsm));

			}

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;
			input >> w;
			input >> h;

			fsm->SetHurtBox(AABB(x, y, w, h));

			std::getline(*file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;

			fsm->SetHurtOffset(Vector2(x, y));

			newEnemy = fsm;

		}

		std::getline(*file, line);
		input.clear();
		input.str(line);

		input >> newEnemy->reputation;

		return newEnemy;

	}

	Emplacement::Emplacement()
	{

	}

	Emplacement::Emplacement(Level* l, int mH, int d, float x, float y, float w, float h, float fX, float fY) : Enemy(l, mH, d, x, y, w, h)
	{

		this->weapon = NULL;

		this->firingDirection = Vector2(fX, fY);

	}

	Emplacement::~Emplacement()
	{

		if (this->weapon)
		{

			delete this->weapon;
			this->weapon = NULL;

		}

	}

	void Emplacement::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		this->weapon->Use(attacks, this->firingDirection);

		this->GameEntity::Update(entities, attacks, deltaTime);

	}

	Enemy* Emplacement::Clone(Vector2 location, std::string type)
	{

		Emplacement* emp = new Emplacement(this->level, this->maxHealth, this->direction, location.X(), location.Y(), this->Width(), this->Height(), this->firingDirection.X(), this->firingDirection.Y());
		this->DataCopy(emp);

		emp->weapon = this->weapon->Clone(emp);

		emp->sheet.PlayAnimation(0, true);

		return emp;

	}

	void Emplacement::SetWeapon(Weapon* w)
	{

		this->weapon = w;

	}

	void Emplacement::SetFiringDirection(float x, float y)
	{

		this->firingDirection = Vector2(x, y);

	}

	void Emplacement::Pause()
	{

		this->GameObject::Pause();

		this->weapon->Pause();

	}

	void Emplacement::Unpause()
	{

		this->GameEntity::Pause();

		this->weapon->Unpause();

	}

	Patrolman::Patrolman()
	{

	}

	Patrolman::Patrolman(Level* l, int mH, int d, float x, float y, float w, float h, float mn, float mx, float s) : Enemy(l, mH, d, x, y, w, h)
	{

		this->min = mn;
		this->max = mx;
		this->speed = s;

	}

	Patrolman::~Patrolman()
	{

		if (this->weapon)
		{

			delete this->weapon;
			this->weapon = NULL;

		}

	}

	void Patrolman::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Vector2 old(this->X(), this->Y());

		this->SetX(this->X() + this->speed * this->direction * deltaTime);

		if (this->X() <= this->min)
		{

			this->SetX(min);
			this->direction = 1;

		}
		else if (this->X() >= this->max)
		{

			this->SetX(max);
			this->direction = -1;

		}

		this->velocity = Vector2(this->X(), this->Y()) - old;

		if (this->weapon->Ready())
		{

			for (int i = 0; i < entities.size(); i++)
			{

				if (entities[i]->Type()[0] != this->Type()[0])
				{

					if (entities[i]->Intersects(&this->vision))
					{

						if ((entities[i]->X() > this->X() && this->direction == 1) || (entities[i]->X() < this->X() && this->direction == -1))
						{

							this->weapon->Use(attacks, Vector2(this->direction * this->firingX, this->firingY));

						}

					}

				}

			}

		}

		this->GameEntity::Update(entities, attacks, deltaTime);

		if (!this->weapon->Ready())
		{

			this->sheet.ShiftAnimation(1, true);

		}
		else
		{

			this->sheet.ShiftAnimation(0, true);

		}

	}

	void Patrolman::SetFiringDirection(float x, float y)
	{

		this->firingX = x;
		this->firingY = y;

	}

	Enemy* Patrolman::Clone(Vector2 location, std::string type)
	{

		Patrolman* patrol = new Patrolman(this->level, this->maxHealth, this->direction, location.X(), location.Y(), this->Width(), this->Height(), this->min + location.X(), this->max + location.X(), this->speed);
		this->DataCopy(patrol);

		patrol->vision = this->vision;
		patrol->vision.SetX(patrol->vision.X() + location.X());
		patrol->vision.SetY(patrol->vision.Y() + location.Y());
		
		patrol->weapon = this->weapon->Clone(patrol);

		patrol->sheet.PlayAnimation(0, true);

		patrol->firingX = this->firingX;
		patrol->firingY = this->firingY;

		return patrol;

	}

	void Patrolman::SetVision(AABB v)
	{

		this->vision = v;

	}

	void Patrolman::SetWeapon(Weapon* w)
	{

		this->weapon = w;

	}

	void Patrolman::SetMin(float m)
	{

		this->min = m;

	}

	void Patrolman::SetMax(float m)
	{

		this->max = m;

	}

	void Patrolman::SetSpeed(float s)
	{

		this->speed = s;

	}

	void Patrolman::Pause()
	{

		this->GameObject::Pause();

		this->weapon->Pause();

	}

	void Patrolman::Unpause()
	{

		this->GameEntity::Pause();

		this->weapon->Unpause();

	}

	Turret::Turret()
	{

	}

	Turret::Turret(Level* l, int mH, int d, float x, float y, float w, float h) : Enemy(l, mH, d, x, y, w, h)
	{

		this->weapon = NULL;

	}

	Turret::~Turret()
	{

		if (this->weapon)
		{

			delete this->weapon;
			this->weapon = NULL;

		}

	}

	void Turret::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		if (this->weapon->Ready())
		{

			for (int i = 0; i < entities.size(); i++)
			{

				if (entities[i]->Type()[0] != this->Type()[0])
				{

					Vector2 point = entities[i]->Center();
					Vector2 firingPoint = this->Center();

					if ((firingPoint - point).LengthSquared() < this->range * this->range)
					{

						float angle = atan2f((firingPoint - point).Y(), (firingPoint - point).X());

						if (AdjustAngle(this->angleOffset + angle) >= minAngle && AdjustAngle(this->angleOffset + angle) <= maxAngle)
						{

							Vector2 direction(-1, 0); //Unit vector starts from -1 to account for the different coordinate systems
							direction.Rotate(angle);

							this->weapon->Use(attacks, direction);

						}

					}

				}

			}

		}

		this->GameEntity::Update(entities, attacks, deltaTime);

	}

	void Turret::SetMinAngle(float a)
	{

		this->minAngle = a;

	}

	void Turret::SetMaxAngle(float a)
	{

		this->maxAngle = a;

	}

	void Turret::SetAngleOffset(float o)
	{

		this->angleOffset = o;

	}

	void Turret::SetRange(float r)
	{

		this->range = r;

	}

	void Turret::SetWeapon(Weapon* w)
	{

		this->weapon = w;

	}

	Enemy* Turret::Clone(Vector2 location, std::string type)
	{

		Turret* turret = new Turret(this->level, this->maxHealth, this->direction, location.X(), location.Y(), this->Width(), this->Height());
		this->DataCopy(turret);

		turret->minAngle = this->minAngle;
		turret->maxAngle = this->maxAngle;
		turret->angleOffset = this->angleOffset;
		turret->range = this->range;

		turret->weapon = this->weapon->Clone(turret);

		turret->sheet.PlayAnimation(0, true);

		return turret;

	}

	void Turret::Pause()
	{

		this->GameObject::Pause();

		this->weapon->Pause();

	}

	void Turret::Unpause()
	{

		this->GameEntity::Pause();

		this->weapon->Unpause();

	}

	Mover::Mover()
	{

	}

	Mover::Mover(Level* l, int mH, int d, float x, float y, float w, float h) : Enemy(l, mH, d, x, y, w, h)
	{

	}

	Mover::~Mover()
	{

	}

	void Mover::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		if(this->type == "Circle")
		{

			this->UpdateCircle(entities, attacks, deltaTime);

		}
		else if(this->type == "Constant")
		{

			this->UpdateConstant(entities, attacks, deltaTime);

		}
		else if(this->type == "Sine")
		{

			this->UpdateSine(entities, attacks, deltaTime);

		}

		this->GameEntity::Update(entities, attacks, deltaTime);

	}

	void Mover::UpdateCircle(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Vector2 old(this->X(), this->Y());

		this->parameters[2] += 2 * M_PI * this->parameters[3] * deltaTime;
		Vector2 offset(this->parameters[4], this->parameters[5]);
		offset.Rotate(this->parameters[2]);

		this->SetX(this->parameters[0] + offset.X() - (this->Width() / 2));
		this->SetY(this->parameters[1] + offset.Y() - (this->Height() / 2));

		this->SetVelocity(old - Vector2(this->X(), this->Y()));

	}

	void Mover::UpdateConstant(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Vector2 unit = Vector2(this->parameters[3] - this->parameters[1], this->parameters[4] - this->parameters[2]);
		unit.Normalize();

		Vector2 velocity = unit * (this->parameters[0] * deltaTime);
		velocity = velocity * this->direction;

		Vector2 old(this->X(), this->Y());
		this->Move(velocity);

		float pos, ma, mi;
		pos = unit.Dot(Vector2(this->X(), this->Y()));
		ma = unit.Dot(Vector2(this->parameters[3], this->parameters[4]));
		mi = unit.Dot(Vector2(this->parameters[1], this->parameters[2]));

		if (pos <= mi && this->direction == -1)
		{

			this->SetX(this->parameters[1]);
			this->SetY(this->parameters[2]);
			this->direction = 1;

			velocity = Vector2(this->parameters[1], this->parameters[2]) - old;

		}
		else if (pos >= ma && this->direction == 1)
		{

			this->SetX(this->parameters[3]);
			this->SetY(this->parameters[4]);

			this->direction = -1;

			velocity = Vector2(this->parameters[3], this->parameters[4]) - old;

		}

		this->SetVelocity(velocity);
	
	}

	void Mover::UpdateSine(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Vector2 old(this->X(), this->Y());

		this->SetX(this->X() + this->parameters[0] * this->direction * deltaTime);

		float min = this->parameters[2];
		float max = this->parameters[3];

		if (this->X() <= min)
		{

			this->SetX(min);
			this->direction = 1;

		}
		else if (this->X() >= max)
		{

			this->SetX(max);
			this->direction = -1;

		}

		this->SetY(this->parameters[1] + this->parameters[5] * sinf((2 * M_PI  * (this->X() - min) * this->parameters[4] * this->direction) / (max - min)));

		this->velocity = Vector2(this->X(), this->Y()) - old;

	}

	Enemy* Mover::Clone(Vector2 location, std::string type)
	{

		Mover* mover = new Mover(this->level, this->maxHealth, this->direction, location.X(), location.Y(), this->Width(), this->Height());
		this->DataCopy(mover);

		mover->type = this->type;
		mover->parameters = this->parameters;

		if (this->type == "Circle")
		{

			mover->parameters[0] += location.X();
			mover->parameters[1] += location.Y();

		}
		else if (this->type == "Constant")
		{

			mover->parameters[1] += location.X();
			mover->parameters[2] += location.Y();
			mover->parameters[3] += location.X();
			mover->parameters[4] += location.Y();

		}
		else if (this->type == "Sine")
		{

			mover->parameters[1] += location.Y();
			mover->parameters[2] += location.X();
			mover->parameters[3] += location.X();

		}

		mover->sheet.PlayAnimation(0, true);

		return mover;

	}

	void Mover::SetType(std::string t)
	{

		this->type = t;

	}

	void Mover::PushParameter(float p)
	{

		this->parameters.push_back(p);

	}

	Chaser::Chaser()
	{

	}

	Chaser::Chaser(Level* l, int mH, int d, float x, float y, float w, float h) : Enemy(l, mH, d, x, y, w, h)
	{

	}

	Chaser::~Chaser()
	{

		if (this->weapon)
		{

			delete this->weapon;
			this->weapon = NULL;

		}

	}

	void Chaser::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		this->SetVelocity(Vector2());

		if (this->chasingIndex < 0)
		{

			if (this->path.size() > 1)
			{

				Vector2 unit = (this->pathDirection == 1 ? this->path[this->pathIndex + 1] - Vector2(this->X(), this->Y()) : Vector2(this->X(), this->Y()) - this->path[this->pathIndex]);
				unit.Normalize();

				Vector2 velocity = unit * (this->speed * deltaTime);
				velocity = velocity * this->pathDirection;

				Vector2 old(this->X(), this->Y());
				this->Move(velocity);

				float pos, ma, mi;
				pos = unit.Dot(Vector2(this->X(), this->Y()));
				ma = unit.Dot(this->path[this->pathIndex + 1]);
				mi = unit.Dot(this->path[this->pathIndex]);

				if (pos <= mi && this->pathDirection == -1)
				{

					this->SetX(this->path[this->pathIndex].X());
					this->SetY(this->path[this->pathIndex].Y());

					velocity = Vector2(this->X(), this->Y()) - old;

					this->pathIndex += this->pathDirection;

				}
				else if (pos >= ma && this->pathDirection == 1)
				{

					this->SetX(this->path[this->pathIndex + 1].X());
					this->SetY(this->path[this->pathIndex + 1].Y());

					velocity = Vector2(this->X(), this->Y()) - old;

					this->pathIndex += this->pathDirection;

				}

				velocity = Vector2(this->X(), this->Y()) - old;

				this->SetVelocity(velocity);

			}

			if (this->pathIndex < 0)
			{

				this->pathIndex = 0;
				this->pathDirection = 1;

			}
			else if (this->pathIndex > this->path.size() - 2 && this->path.size() > 1)
			{

				this->pathIndex = this->path.size() - 2;
				this->pathDirection = -1;

			}

			for (int i = 0; i < entities.size(); i++)
			{

				if (entities[i]->Intersects(&this->zone) && entities[i]->Type()[0] != this->Type()[0])
				{

					this->chasingIndex = i;

					break;

				}

			}

		}
		else
		{

			if (this->chasingIndex < entities.size())
			{

				GameEntity* entity = entities[this->chasingIndex];

				if (this->range.Intersects(entity))
				{

					this->sheet.PlayAnimation(CHASER_ATTACK_ANIM, false);
					
					if (this->weapon)
					{

						this->weapon->Use(attacks, Vector2(this->direction, 0));

					}

				}

				if (this->Intersects(&this->zone) && entity->Intersects(&this->zone))
				{

					Vector2 unit = Vector2(entity->Center().X(), entity->Center().Y()) - Vector2(this->Center().X(), this->Center().Y());
					unit.Normalize();
					unit = unit * (this->speed * deltaTime);

					this->Move(unit);
					this->SetVelocity(unit);

				}
				else
				{

					if (this->leaveTime < 0)
					{

						this->leaveTime = GetTime();

					}

					if (GetTime() - this->leaveTime >= this->waitTime)
					{

						this->chasingIndex = -1;

						this->leaveTime = -1;

					}

				}

			}

		}

		if (this->Velocity().X() < 0)
		{

			this->direction = -1;

		}
		else if(this->Velocity().X() > 0)
		{

			this->direction = 1;

		}

		this->range.SetX(this->X() + (this->Direction() == -1 ? -this->range.Width() : 0));
		this->range.SetY(this->Y() + this->yOffset);

		this->GameEntity::Update(entities, attacks, deltaTime);

		if (this->weapon && this->weapon->Ready())
		{

			if (this->Velocity().LengthSquared() == 0)
			{

				this->sheet.PlayAnimation(CHASER_FLOAT_ANIM, true);

			}
			else
			{

				this->sheet.PlayAnimation(CHASER_MOVE_ANIM, true);

			}

		}
		else
		{

			if (!this->sheet.Playing())
			{

				this->sheet.PlayAnimation(CHASER_FLOAT_ANIM, true);

			}

		}

	}

	Enemy* Chaser::Clone(Vector2 location, std::string type)
	{

		Chaser* chaser = new Chaser(this->level, this->maxHealth, this->direction, location.X(), location.Y(), this->Width(), this->Height());
		this->DataCopy(chaser);

		chaser->path = this->path;

		for (int i = 0; i < chaser->path.size(); i++)
		{

			chaser->path[i].SetX(location.X() + chaser->path[i].X());
			chaser->path[i].SetY(location.Y() + chaser->path[i].Y());

		}

		chaser->range = this->range;
		chaser->zone = this->zone;

		chaser->zone.SetX(location.X() + chaser->zone.X());
		chaser->zone.SetY(location.Y() + chaser->zone.Y());

		chaser->yOffset = this->yOffset;
		chaser->speed = this->speed;

		chaser->chasingIndex = -1;
		chaser->pathIndex = 0;
		chaser->pathDirection = 1;

		chaser->leaveTime = -1;
		chaser->waitTime = this->waitTime;

		if (this->weapon)
		{

			chaser->weapon = this->weapon->Clone(chaser);

		}
		else
		{

			chaser->weapon = NULL;

		}

		return chaser;

	}

	void Chaser::AddToPath(Vector2 p)
	{

		this->path.push_back(p);

	}
	
	void Chaser::SetRange(AABB r)
	{

		this->range = r;

	}
	
	void Chaser::SetZone(AABB z)
	{

		this->zone = z;

	}

	void Chaser::SetOffset(float o)
	{

		this->yOffset = o;

	}

	void Chaser::SetSpeed(float s)
	{

		this->speed = s;

	}

	void Chaser::SetWait(double w)
	{

		this->waitTime = w;

	}

	void Chaser::SetWeapon(Weapon* w)
	{

		this->weapon = w;

	}

	void Chaser::Pause()
	{

		this->GameObject::Pause();

		if (this->weapon)
		{

			this->weapon->Pause();

		}

	}

	void Chaser::Unpause()
	{

		this->GameEntity::Pause();

		if (this->weapon)
		{

			this->weapon->Unpause();

		}

	}

	FSM::FSM() : Enemy()
	{

		if (!FSM::init)
		{

			FSM::Initialize();

		}

	}

	FSM::FSM(Level* l, int mH, int d, float x, float y, float w, float h) : Enemy(l, mH, d, x, y, w, h)
	{

		if (!FSM::init)
		{

			FSM::Initialize();

		}

		this->stateAnimations["Idle"] = std::pair<int, bool>(FSM_IDLE_ANIM, true);

	}

	void FSM::Initialize()
	{

		FSM::states["Charge"] = FSM::FSMCharge;
		FSM::states["Shoot"] = FSM::FSMShoot;
		FSM::states["Turn"] = FSM::FSMTurn;
		FSM::states["GoTo"] = FSM::FSMGoto;
		FSM::states["Idle"] = FSM::FSMIdle;
		FSM::states["Hurt"] = FSM::FSMHurt;
		FSM::states["Ambush"] = FSM::FSMAmbush;
		FSM::states["SelfDestruct"] = FSM::FSMSelfDestruct;

		compareFunctions["<="] = LessEqual;
		compareFunctions[">="] = GreaterEqual;
		compareFunctions["=="] = Equal;
		compareFunctions["!="] = NotEqual;

	}

	FSM::~FSM()
	{

		for (int i = 0; i < this->weapons.size(); i++)
		{

			if (this->weapons[i])
			{

				delete this->weapons[i];
				this->weapons[i] = NULL;

			}

		}

		this->weapons.clear();

		if (this->loadedSounds && this->invulnerableEffect)
		{

			Mix_FreeChunk(this->invulnerableEffect);
			this->invulnerableEffect = NULL;

		}

	}

	void FSM::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		this->GameEntity::Update(entities, attacks, deltaTime);
		bool decide = false;

		if (this->choice.size() == 0)
		{

			if (GetTime() - this->lastSwitch >= this->decisionDelay)
			{

				if (this->scripted.size() != 0)
				{

					std::istringstream parse(this->scripted[0]);
					parse >> this->choice;

					int numParameters;
					parse >> numParameters;

					if (numParameters > 0)
					{

						this->fsm[this->choice].clear();

					}

					for (int i = 0; i < numParameters; i++)
					{

						float p;
						parse >> p;

						this->fsm[this->choice].push_back(p);

					}

					this->scripted.erase(this->scripted.begin());

				}
				else
				{

					int currentIndex = 0;

					while (currentIndex >= 0)
					{

						currentIndex = (FSM::Evaluate(this, &this->nodes[currentIndex]) ? this->nodes[currentIndex].right : this->nodes[currentIndex].left);

					}

					decide = true;

				}

				this->lastSwitch = GetTime();

			}

		}

		FSM::SwitchFSM(this, entities, attacks, deltaTime);

		this->hurtBox.SetX(this->X() + (this->direction > 0 ? this->hurtOffset.X() : this->Width() - this->hurtOffset.X() - this->hurtBox.Width()));
		this->hurtBox.SetY(this->Y() + this->hurtOffset.Y());

		if (this->choice.size() > 0)
		{

			if (this->sheet.CurrentAnimation() == this->stateAnimations[this->choice].first)
			{

				this->sheet.ShiftAnimation(this->stateAnimations[this->choice].first, this->stateAnimations[this->choice].second);

			}
			else
			{

				this->sheet.PlayAnimation(this->stateAnimations[this->choice].first, this->stateAnimations[this->choice].second);

			}

		}
		else
		{

			this->sheet.PlayAnimation(this->stateAnimations["Idle"].first, true);

		}

		if (decide)
		{

			this->hurt = false;

		}

	}

	bool FSM::Evaluate(FSM* fsm, DecisionNode* node)
	{

		if (node->left == -1 || node->right == -1)
		{

			std::istringstream parse(node->value);
			parse >> fsm->choice;

			if (fsm->choice == "scripted")
			{

				fsm->scripted.clear();

				int numActions;
				parse >> numActions;

				for (int i = 0; i < numActions; i++)
				{

					std::string builder = "";
					std::string piece;

					parse >> piece;

					while (piece != ",")
					{

						builder += piece + " ";
						parse >> piece;

					}

					fsm->scripted.push_back(builder);

				}
	
				parse.clear();
				parse.str(fsm->scripted[0]);
				parse >> fsm->choice;

			}

			int numParameters;
			parse >> numParameters;

			if (numParameters > 0)
			{

				fsm->fsm[fsm->choice].clear();

			}

			for (int i = 0; i < numParameters; i++)
			{

				float p;
				parse >> p;

				fsm->fsm[fsm->choice].push_back(p);

			}

			if (fsm->scripted.size() > 0)
			{

				fsm->scripted.erase(fsm->scripted.begin());

			}

			return (node->right == -1);

		}
		
		std::istringstream parse(node->value);

		std::string lHS;
		parse >> lHS;

		if (lHS == "last")
		{

			lHS = fsm->lastChoice;

			std::string comparator, val;
			parse >> comparator;
			parse >> val;

			if (comparator == "!=")
			{

				return lHS != val;

			}
			else
			{

				return lHS == val;

			}

		}
		else if (lHS == "loc")
		{

			Vector2 loc(fsm->X(), fsm->Y());
			float x, y;

			std::string comparator;

			parse >> comparator;
			parse >> x;
			parse >> y;

			if (comparator == "!=")
			{

				return (loc.X() != x) || (loc.Y() != y);

			}
			else
			{

				return (loc.X() == x) && (loc.Y() == y);

			}

		}
		else
		{

			float data, value;
			std::string comparator;

			parse >> comparator;
			parse >> value;

			if (lHS == "health")
			{

				data = fsm->health;

			}
			else if (lHS == "hurt")
			{

				data = fsm->hurt;

			}
			else if (lHS == "random")
			{

				data = RandomBoundedInteger(1, 100);

			}

			return compareFunctions[comparator](data, value);

		}

	}

	void FSM::SwitchFSM(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		if (fsm->choice.size() > 0)
		{

			FSM::states[fsm->choice](fsm, entities, attacks, deltaTime);
			
		}
		else
		{

			fsm->choice = "";

		}

	}

	void FSM::FSMGoto(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Vector2 dir(fsm->fsm["GoTo"][1] - fsm->X(), fsm->fsm["GoTo"][2] - fsm->Y());
		dir.Normalize();

		fsm->SetVelocity(dir * (fsm->fsm["GoTo"][0] * deltaTime));

		fsm->Move(fsm->velocity);

		Vector2 newDir(fsm->fsm["GoTo"][1] - fsm->X(), fsm->fsm["GoTo"][2] - fsm->Y());

		fsm->stateAnimations[fsm->choice].first = (int)fsm->fsm[fsm->choice][3];

		if (newDir.Dot(dir) <= 0)
		{

			fsm->SetX(fsm->fsm["GoTo"][1]);
			fsm->SetY(fsm->fsm["GoTo"][2]);

			fsm->lastChoice = fsm->choice;
			fsm->choice = "";

			fsm->lastSwitch = GetTime();

			fsm->stateAnimations["Idle"].first = fsm->fsm["GoTo"][4];

		}

	}

	void FSM::FSMIdle(FSM* fsm, std::vector<GameEntity*>& entitites, std::vector<Attack*>& attacks, float deltaTime)
	{

		fsm->lastChoice = fsm->choice;
		fsm->choice = "";

	}

	void FSM::FSMHurt(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		if (!fsm->hurt && !fsm->sheet.Playing())
		{

			FSM::FSMIdle(fsm, entities, attacks, deltaTime);

		}

	}

	void FSM::FSMCharge(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		if (fsm->fsm["Charge"].size() <= 1)
		{

			fsm->fsm["Charge"].push_back(fsm->X());

		}

		if (fsm->fsm["Charge"][1] != fsm->X())
		{

			fsm->lastChoice = fsm->choice;
			fsm->choice = "";

			fsm->lastSwitch = GetTime();

			fsm->fsm["Charge"].pop_back();

		}
		else
		{

			fsm->SetVelocity(Vector2(fsm->direction * fsm->fsm["Charge"][0] * deltaTime, 0));

			fsm->Move(fsm->velocity);

			fsm->fsm["Charge"][1] = fsm->X();

		}

	}

	void FSM::FSMTurn(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		fsm->direction = -fsm->direction;

		fsm->lastChoice = fsm->choice;
		fsm->choice = "";

		fsm->lastSwitch = GetTime();

	}

	void FSM::FSMShoot(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		Weapon* weapon = fsm->weapons[fsm->fsm["Shoot"][0]];

		fsm->stateAnimations[fsm->choice].first = (int)fsm->fsm[fsm->choice][1];

		if (!fsm->sheet.Playing() && weapon->Ready())
		{

			weapon->Use(attacks, Vector2(fsm->direction, 0));

		}

		if (!weapon->Ready())
		{

			fsm->lastChoice = fsm->choice;
			fsm->choice = "";

			fsm->lastSwitch = GetTime();

		}

	}

	void FSM::FSMAmbush(FSM* fsm, std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		//Wait
		if (fsm->fsm["Ambush"].size() == 8)
		{

			AABB zone(fsm->X() + fsm->fsm["Ambush"][0], fsm->Y() + fsm->fsm["Ambush"][1], fsm->fsm["Ambush"][2], fsm->fsm["Ambush"][3]);

			for (int i = 0; i < entities.size(); i++)
			{

				if (entities[i]->Intersects(&zone) && entities[i]->Type()[0] != fsm->Type()[0])
				{

					fsm->stateAnimations[fsm->choice].first = (int)fsm->fsm["Ambush"][7];

					//Trigger Ambush
					Vector2 unit(fsm->fsm["Ambush"][4], fsm->fsm["Ambush"][5]);
					unit.Normalize();

					fsm->fsm["Ambush"].push_back(unit.X());
					fsm->fsm["Ambush"].push_back(unit.Y());

					fsm->fsm["Ambush"].push_back(fsm->X() + fsm->fsm["Ambush"][4]);
					fsm->fsm["Ambush"].push_back(fsm->Y() + fsm->fsm["Ambush"][5]);

				}

			}

		}
		//Ambush
		else
		{

			Vector2 unit(fsm->fsm["Ambush"][8], fsm->fsm["Ambush"][9]);

			fsm->SetVelocity(unit * (fsm->fsm["Ambush"][6] * deltaTime));
			fsm->Move(fsm->velocity);

			Vector2 distance(fsm->fsm["Ambush"][10] - fsm->X(), fsm->fsm["Ambush"][11] - fsm->Y());

			if (distance.Dot(unit) <= 0)
			{

				fsm->SetX(fsm->fsm["Ambush"][10]);
				fsm->SetY(fsm->fsm["Ambush"][11]);

				fsm->lastChoice = fsm->choice;
				fsm->choice = "";

				fsm->fsm["Ambush"].pop_back();
				fsm->fsm["Ambush"].pop_back();
				fsm->fsm["Ambush"].pop_back();
				fsm->fsm["Ambush"].pop_back();

				fsm->lastSwitch = GetTime();

			}

		}

	}

	void FSM::FSMSelfDestruct(FSM* fsm, std::vector<GameEntity*>& entitites, std::vector<Attack*>& attacks, float deltaTime)
	{

		fsm->hurtBox.SetWidth(-10000);
		fsm->hurtBox.SetHeight(-10000); //HAX: Ensure there's no intersection

		if (fsm->fsm["SelfDestruct"].size() > 0 && !fsm->sheet.Playing())
		{

			fsm->active = false;

		}

		if (fsm->fsm["SelfDestruct"].size() == 0)
		{

			fsm->fsm["SelfDestruct"].push_back(1);

		}

	}

	Enemy* FSM::Clone(Vector2 location, std::string type)
	{

		FSM* newFSM = new FSM(this->level, this->MaxHealth(), this->Direction(), location.X(), location.Y(), this->Width(), this->Height());
		this->DataCopy(newFSM);

		newFSM->direction = this->direction;

		newFSM->fsm = this->fsm;
		newFSM->decisionDelay = this->decisionDelay;
		newFSM->lastSwitch = GetTime();

		newFSM->stateAnimations = this->stateAnimations;
		newFSM->nodes = this->nodes;

		newFSM->hurtBox = this->hurtBox;
		newFSM->hurtBox.SetX(newFSM->hurtBox.X() + location.X());
		newFSM->hurtBox.SetY(newFSM->hurtBox.Y() + location.Y());

		newFSM->hurtOffset = this->hurtOffset;

		newFSM->invulnerableEffect = this->invulnerableEffect;

		for (int i = 0; i < this->weapons.size(); i++)
		{

			newFSM->weapons.push_back(this->weapons[i]->Clone(newFSM));

		}

		newFSM->choice = newFSM->lastChoice = "";

		return newFSM;

	}

	void FSM::Hurt(Attack* a)
	{

		if (this->hurtBox.Intersects(a))
		{

			this->GameEntity::Hurt(a);

			this->hurt = true;

		}
		else
		{

			if (this->invulnerableEffect)
			{

				PlaySound(this->invulnerableEffect, this, 0);

			}

		}

	}

	void FSM::AddState(std::string s, int anim, bool loop)
	{

		this->fsm[s] = std::vector<float>();

		this->stateAnimations[s] = std::pair<int, bool>(anim, loop);

	}

	void FSM::AddNode(DecisionNode node)
	{

		this->nodes.push_back(node);

	}

	void FSM::PushParameter(std::string s, float p)
	{

		this->fsm[s].push_back(p);

	}

	void FSM::SetWeapon(Weapon* w)
	{

		this->weapons.push_back(w);

	}

	void FSM::SetInvulnerableEffect(Mix_Chunk* e)
	{

		this->invulnerableEffect = e;

	}

	void FSM::SetDecisionDelay(float d)
	{

		this->decisionDelay = d;

	}

	void FSM::SetHurtBox(AABB box)
	{

		this->hurtBox = box;

	}

	void FSM::SetHurtOffset(Vector2 o)
	{

		this->hurtOffset = o;

	}

	void FSM::Pause()
	{

		this->GameObject::Pause();

		for (int i = 0; i < this->weapons.size(); i++)
		{

			this->weapons[i]->Pause();

		}

	}

	void FSM::Unpause()
	{

		this->GameEntity::Pause();

		this->lastSwitch = this->PauseTime(this->lastSwitch);
		
		for (int i = 0; i < this->weapons.size(); i++)
		{

			this->weapons[i]->Unpause();

		}

	}


}
