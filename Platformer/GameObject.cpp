//Includes
#include <SDL.h>
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "Attack.h"
#include "GameObject.h"

namespace Platformer
{

	GameObject::GameObject() : AABB()
	{

	}

	GameObject::GameObject(float x, float y, float w, float h) : AABB(x, y, w, h)
	{

	}

	GameObject::~GameObject()
	{

	}

	void GameObject::Move(Vector2 m)
	{

		if (this->ShouldMove())
		{

			this->SetX(this->X() + m.X());
			this->SetY(this->Y() + m.Y());

		}

	}

	Vector2 GameObject::Velocity()
	{

		return this->velocity;

	}

	void GameObject::SetVelocity(Vector2 v)
	{

		this->velocity = v;

	}

	Vector2 GameObject::RawVelocity()
	{

		return this->rawVelocity;

	}

	void GameObject::SetRawVelocity(Vector2 r)
	{

		this->rawVelocity = r;

	}

	bool GameObject::Active()
	{

		return true;

	}

	void GameObject::Pause()
	{

		this->paused = true;
		this->pauseTime = GetTime();

	}

	void GameObject::Unpause()
	{

		this->paused = false;

	}

	double GameObject::PauseTime(double time)
	{

		return GetTime() - (this->pauseTime - time);

	}

	bool GameObject::ShouldMove()
	{

		return true;

	}

	GameEntity::GameEntity()
	{

	}

	GameEntity::GameEntity(Level* l, int mH, int d, float x, float y, float w, float h) : GameObject(x, y, w, h)
	{

		this->health = mH;
		this->maxHealth = mH;
		this->direction = d;

		this->level = l;

	}

	GameEntity::~GameEntity()
	{

		if (this->loadedSounds && this->hurtEffect)
		{

			Mix_FreeChunk(this->hurtEffect);
			this->hurtEffect = NULL;

		}

	}

	void GameEntity::DataCopy(GameEntity* entity)
	{

		entity->width = this->Width();
		entity->height = this->Height();

		entity->maxHealth = this->maxHealth;
		entity->health = this->maxHealth;

		entity->direction = this->direction;

		entity->active = true;

		entity->lastHit = GetTime() - this->hitInvulnerability;
		entity->hitInvulnerability = this->hitInvulnerability;

		entity->level = this->level;

		entity->sheet.Clone(&this->sheet);
		entity->sheet.SetSurface(this->sheet.Surface());

		entity->koSheet.Clone(&this->koSheet);
		entity->koSheet.SetSurface(this->koSheet.Surface());

		entity->lOffset = this->lOffset;
		entity->rOffset = this->rOffset;

		entity->hurtEffect = this->hurtEffect;
		entity->loadedSounds = false;

	}

	void GameEntity::LoadEntity(std::ifstream& input, bool close)
	{

		std::string line;
		std::getline(input, line);

		std::istringstream in(line);

		in >> this->x;
		in >> this->y;
		in >> this->width;
		in >> this->height;

		std::getline(input, line);
		in.clear();
		in.str(line);

		in >> this->maxHealth;
		this->health = this->maxHealth;
		in >> this->direction;
		in >> this->hitInvulnerability;

		this->lastHit = -this->hitInvulnerability;

		std::getline(input, line);
		this->sheet.LoadSpritesheet(line);
		std::getline(input, line);
		this->sheet.LoadSurface(line);

		std::getline(input, line);
		this->koSheet.LoadSpritesheet(line);
		std::getline(input, line);
		this->koSheet.LoadSurface(line);

		std::getline(input, line);
		if (line.size() > 0)
		{

			this->hurtEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), line);
			this->loadedSounds = true;

		}

		std::getline(input, line);
		in.clear();
		in.str(line);

		float xL, yL, xR, yR;
		in >> xL;
		in >> yL;
		in >> xR;
		in >> yR;

		this->lOffset = Vector2(xL, yL);
		this->rOffset = Vector2(xR, yR);

		this->Load(input);

		if (close)
		{

			input.close();

		}

	}

	void GameEntity::LoadSpritesheet(std::string fileName, std::string imageFile)
	{

		this->sheet.LoadSpritesheet(fileName);
		this->sheet.LoadSurface(imageFile);

	}

	void GameEntity::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		this->sheet.Update(deltaTime);
		this->koSheet.Update(deltaTime);

	}

	bool GameEntity::Active()
	{

		return this->active;

	}

	void GameEntity::SetActive(bool a)
	{

		this->active = a;

	}

	double GameEntity::LastHit()
	{

		return this->lastHit;

	}

	float GameEntity::HitInvulnerability()
	{

		return this->hitInvulnerability;

	}

	void GameEntity::Hurt(Attack* a)
	{

		if (GetTime() - this->lastHit >= this->HitInvulnerability())
		{

			this->health = fmaxf(0, this->health - a->Damage());
			this->lastHit = GetTime();

			if (this->hurtEffect)
			{

				PlaySound(this->hurtEffect, this, 0);

			}

			if (this->health <= 0)
			{

				this->Die();

			}
		
		}

	}

	void GameEntity::Die()
	{

		this->health = 0;

		this->active = false;

	}

	bool GameEntity::IsHurt()
	{

		return GetTime() - this->LastHit() < this->HitInvulnerability();

	}

	void GameEntity::PlayAnimation(int index, bool loop)
	{

		if (this->Health() <= 0)
		{

			this->koSheet.PlayAnimation(index, loop);

		}
		else
		{

			this->sheet.PlayAnimation(index, loop);

		}
	}

	void GameEntity::Render(Vector2 world, float deltaTime)
	{

		SDL_Texture* surface;
		Spritesheet* spriteSheet;

		if (this->Health() <= 0)
		{

			surface = this->koSheet.Surface();
			spriteSheet = &this->koSheet;

		}
		else
		{

			surface = this->sheet.Surface();
			spriteSheet = &this->sheet;

		}

		if (GetTime() - this->lastHit < this->HitInvulnerability())
		{

			float cos = cosf(GetTime());
			SDL_SetTextureColorMod(surface, 255, cos * 255, cos * 255);

		}

		spriteSheet->Render(Vector2(this->X() + this->Offset().X(), this->Y() + this->Offset().Y()) - world, (this->direction < 0));

		SDL_SetTextureColorMod(surface, 255, 255, 255);

	}

	void GameEntity::Unpause()
	{

		this->GameObject::Unpause();

		this->lastHit = this->PauseTime(this->lastHit);

	}

	const char* GameEntity::Type()
	{

		return "Game Entity";

	}

	void GameEntity::SetLevel(Level* l)
	{

		this->level = l;

	}

	void GameEntity::SetOffset(Vector2 o)
	{

		this->lOffset = o;
		this->rOffset = o;

	}

	void GameEntity::SetDirection(int d)
	{

		this->direction = d;

	}

	Vector2 GameEntity::Offset()
	{

		return (this->Direction() > 0 ? this->rOffset : this->lOffset);

	}

	int GameEntity::Direction()
	{

		return this->direction;

	}

	float GameEntity::HealthPercentage()
	{

		return (float)this->health / (float)this->maxHealth;

	}

	int GameEntity::Health()
	{

		return this->health;

	}

	int GameEntity::MaxHealth()
	{

		return this->maxHealth;

	}

	void GameEntity::MaximizeHealth()
	{

		this->health = this->maxHealth;

		this->koSheet.Reset();

	}

	Level* GameEntity::GetLevel()
	{

		return this->level;

	}

	void GameEntity::Load(std::ifstream& file)
	{

	}

	void GameEntity::RequestEvent(std::string e)
	{

	}

	Spritesheet* GameEntity::Sheet()
	{

		return &this->sheet;

	}

}
