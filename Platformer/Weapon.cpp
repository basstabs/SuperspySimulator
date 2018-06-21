//Includes
#include <sstream>

//Custom Includes
#include "Weapon.h"
#include "Level.h"
#include "Helpers.h"
#include "Settings.h"

namespace Platformer
{

	Weapon::Weapon(GameEntity* p, float r)
	{

		this->parent = p;

		this->rateOfUse = r;
		this->lastUsed = GetTime() - r;

	}

	Weapon::~Weapon()
	{

		if (this->loadedSounds && this->useEffect)
		{

			Mix_FreeChunk(this->useEffect);
			this->useEffect = NULL;

		}

	}

	Weapon* Weapon::LoadWeapon(std::ifstream& file, GameEntity* p)
	{

		Weapon* weap = NULL;

		std::string line;
		std::string type;
		std::getline(file, type);

		std::istringstream in;

		if (type == "Projectile" || type == "Multishot" || type == "Spawn")
		{

			std::getline(file, line);

			in.clear();
			in.str(line);

			float lX, lY, rX, rY, r;

			in >> lX;
			in >> lY;
			in >> rX;
			in >> rY;
			in >> r;

			if (type == "Projectile")
			{

				weap = new Projectile(p, lX, lY, rX, rY, r);

			}
			else if (type == "Spawn")
			{

				weap = new SpawnGun(p, lX, lY, rX, rY, r);

			}
			else
			{

				weap = new Multishot(p, lX, lY, rX, rY, r);

			}

		}
		else if (type == "Melee")
		{

			std::getline(file, line);
			in.clear();
			in.str(line);

			int d, i;
			float r;

			in >> d;
			in >> r;
			in >> i;

			std::getline(file, line);
			in.clear();
			in.str(line);

			float w, h;

			in >> w;
			in >> h;

			std::getline(file, line);
			in.clear();
			in.str(line);

			float lX, lY, rX, rY;

			in >> lX;
			in >> lY;
			in >> rX;
			in >> rY;

			weap = new Melee(p, d, w, h, lX, lY, rX, rY, r, i);

		}
		else
		{

			return NULL;

		}

		weap->Load(file);

		std::string soundFile;
		std::getline(file, soundFile);

		if (soundFile.size() > 0)
		{

			weap->useEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), soundFile);
			weap->loadedSounds = true;

		}

		return weap;

	}

	void Weapon::Use(std::vector<Attack*>& attacks, Vector2 direction)
	{

		if(GetTime() - this->lastUsed > this->rateOfUse)
		{

			this->lastUsed = GetTime();

			this->Fire(attacks, direction);

			if (this->useEffect && this->parent)
			{

				PlaySound(this->useEffect, this->parent, 0);

			}

		}

	}

	bool Weapon::Ready()
	{

		return (GetTime() - this->lastUsed > this->rateOfUse);

	}

	void Weapon::Pause()
	{

		this->pauseTime = GetTime();
		this->paused = true;

	}

	void Weapon::Unpause()
	{

		this->paused = false;
		
		this->lastUsed = this->PauseTime(this->lastUsed);

	}

	double Weapon::PauseTime(double time)
	{

		return GetTime() - (this->pauseTime - time);

	}

	void Weapon::DataCopy(Weapon* w)
	{

		w->rateOfUse = this->rateOfUse;
		w->lastUsed = GetTime();

		w->pauseTime = this->pauseTime;
		w->paused = this->paused;

		w->useEffect = this->useEffect;
		w->loadedSounds = false;

	}

	Projectile::Projectile(GameEntity* p, float lX, float lY, float rX, float rY, float r) : Weapon(p, r)
	{

		this->leftOffset = Vector2(lX, lY);
		this->rightOffset = Vector2(rX, rY);

	}

	Projectile::~Projectile()
	{

		if (this->bullet)
		{

			delete this->bullet;
			this->bullet = NULL;

		}

	}

	Vector2 Projectile::Offset()
	{

		if(this->parent->Direction() < 0)
		{

			return this->leftOffset;

		}

		return this->rightOffset;

	}

	void Projectile::Fire(std::vector<Attack*>& attacks, Vector2 direction)
	{

		Bullet* newBullet = this->bullet->Clone(Vector2(this->parent->X() + (this->parent->Direction() > 0 ? this->rightOffset.X() : this->leftOffset.X()), this->parent->Y() + (this->parent->Direction() > 0 ? this->rightOffset.Y() : this->leftOffset.Y())), this->parent->Direction());
		newBullet->SetRawVelocity(Vector2(newBullet->RawVelocity().X() * direction.X(), newBullet->RawVelocity().X() * direction.Y()));

		newBullet->Sheet()->PlayAnimation(0, true);
		attacks.push_back(newBullet);

	}

	void Projectile::Load(std::ifstream& file)
	{

		std::string line;
		std::getline(file, line);

		std::istringstream input(line);

		int hits, d, x, y, w, h;
		input >> hits;
		input >> d;
		input >> x;
		input >> y;
		input >> w;
		input >> h;

		this->bullet = new Bullet(this->parent, this->parent->GetLevel(), hits, d, x, y, w, h);
		
		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> x;

		this->bullet->SetRawVelocity(Vector2(x, 0));

		std::getline(file, line);
		this->bullet->Sheet()->LoadSpritesheet(line);
		std::getline(file, line);
		this->bullet->Sheet()->LoadSurface(line);

	}

	Weapon* Projectile::Clone(GameEntity* p)
	{

		Projectile* proj = new Projectile(p, this->leftOffset.X(), this->leftOffset.Y(), this->rightOffset.X(), this->rightOffset.Y(), this->rateOfUse);
		this->DataCopy(proj);

		proj->bullet = this->bullet->Clone(Vector2(), 1);
		proj->bullet->SetParent(p);

		return proj;

	}

	void Projectile::Refresh()
	{

		this->bullet->SetLevel(this->parent->GetLevel());

	}

	SpawnGun::SpawnGun(GameEntity* p, float lX, float lY, float rX, float rY, float r) : Weapon(p, r)
	{

		this->leftOffset = Vector2(lX, lY);
		this->rightOffset = Vector2(rX, rY);

	}

	SpawnGun::~SpawnGun()
	{

	}

	Vector2 SpawnGun::Offset()
	{

		if (this->parent->Direction() < 0)
		{

			return this->leftOffset;

		}

		return this->rightOffset;

	}

	void SpawnGun::Fire(std::vector<Attack*>& attacks, Vector2 direction)
	{

		std::string type = this->parent->Type();
		type += "-Child";
		this->parent->GetLevel()->SpawnEnemy(Vector2(this->parent->X() + (this->parent->Direction() > 0 ? this->rightOffset.X() : this->leftOffset.X()), this->parent->Y() + (this->parent->Direction() > 0 ? this->rightOffset.Y() : this->leftOffset.Y())), this->code, type);

	}

	void SpawnGun::Load(std::ifstream& file)
	{

		std::getline(file, this->code);

	}

	Weapon* SpawnGun::Clone(GameEntity* p)
	{

		SpawnGun* gun = new SpawnGun(p, this->leftOffset.X(), this->leftOffset.Y(), this->rightOffset.X(), this->rightOffset.Y(), this->rateOfUse);
		this->DataCopy(gun);

		gun->code = this->code;

		return gun;

	}

	void SpawnGun::Refresh()
	{

	}

	Multishot::Multishot(GameEntity* p, float lX, float lY, float rX, float rY, float r) : Weapon(p, r)
	{

		this->leftOffset = Vector2(lX, lY);
		this->rightOffset = Vector2(rX, rY);

	}

	Multishot::~Multishot()
	{

		for (int i = 0; i < this->bullets.size(); i++)
		{

			if (this->bullets[i])
			{

				delete this->bullets[i];
				this->bullets[i] = NULL;

			}

		}

	}

	Vector2 Multishot::Offset()
	{

		if (this->parent->Direction() < 0)
		{

			return this->leftOffset;

		}

		return this->rightOffset;

	}

	Weapon* Multishot::Clone(GameEntity* p)
	{

		Multishot* proj = new Multishot(p, this->leftOffset.X(), this->leftOffset.Y(), this->rightOffset.X(), this->rightOffset.Y(), this->rateOfUse);
		this->DataCopy(proj);

		for (int i = 0; i < this->bullets.size(); i++)
		{

			proj->bullets.push_back(this->bullets[i]->Clone(Vector2(), 1));
			proj->bullets[i]->SetParent(p);

		}

		return proj;

	}

	void Multishot::Refresh()
	{

		for (int i = 0; i < this->bullets.size(); i++)
		{

			this->bullets[i]->SetLevel(this->parent->GetLevel());

		}

	}


	void Multishot::Fire(std::vector<Attack*>& attacks, Vector2 direction)
	{

		for (int i = 0; i < this->bullets.size(); i++)
		{

			Bullet* newBullet = this->bullets[i]->Clone(Vector2(this->parent->X() + (this->parent->Direction() > 0 ? this->rightOffset.X() : this->leftOffset.X()), this->parent->Y() + (this->parent->Direction() > 0 ? this->rightOffset.Y() : this->leftOffset.Y())), this->parent->Direction());
			newBullet->SetRawVelocity(Vector2(newBullet->RawVelocity().X() * direction.X() - newBullet->RawVelocity().Y() * direction.Y(), newBullet->RawVelocity().X() * direction.Y() + newBullet->RawVelocity().Y() * direction.X()));

			newBullet->Sheet()->PlayAnimation(0, true);
			attacks.push_back(newBullet);

		}

	}

	void Multishot::Load(std::ifstream& file)
	{

		std::string line;
		std::getline(file, line);

		std::istringstream input(line);

		int numBullets;

		input >> numBullets;

		for (int i = 0; i < numBullets; i++)
		{

			std::getline(file, line);
			input.clear();
			input.str(line);

			int hits, d, x, y, w, h;
			input >> hits;
			input >> d;
			input >> x;
			input >> y;
			input >> w;
			input >> h;

			this->bullets.push_back(new Bullet(this->parent, this->parent->GetLevel(), hits, d, x, y, w, h));

			std::getline(file, line);
			input.clear();
			input.str(line);

			input >> x;
			input >> y;

			this->bullets[i]->SetRawVelocity(Vector2(x, y));

			std::getline(file, line);
			this->bullets[i]->Sheet()->LoadSpritesheet(line);
			std::getline(file, line);
			this->bullets[i]->Sheet()->LoadSurface(line);

		}

	}

	Melee::Melee(GameEntity* p, int d, float w, float h, float lX, float lY, float rX, float rY, float r, int i) : Weapon(p, r)
	{

		this->melee = new MeleeAttack(p, p->GetLevel(), d, 0, 0, w, h, lX, lY, rX, rY, i);

	}

	Melee::Melee(GameEntity* p, float r, MeleeAttack* m) : Weapon(p, r)
	{

		this->melee = m;

	}

	Melee::~Melee()
	{

		if (this->melee)
		{

			delete this->melee;
			this->melee = NULL;

		}

	}

	Weapon* Melee::Clone(GameEntity* p)
	{

		return new Melee(p, this->rateOfUse, this->melee->Clone(p));

	}

	void Melee::Refresh()
	{

		this->melee->SetLevel(this->parent->GetLevel());

	}

	void Melee::Fire(std::vector<Attack*>& attacks, Vector2 direction)
	{

		attacks.push_back(this->melee->Clone(this->parent));

	}

	void Melee::Load(std::ifstream& file)
	{

	}

}
