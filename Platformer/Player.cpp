//Includes
#include <sstream>
#include <SDL_image.h>
#include <iostream>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "Player.h"
#include "Platformer.h"

namespace Platformer
{

	Player::Player(Platformer* p)
	{

		this->parent = p;

	}

	Player::Player(Platformer* p, Level* l, int mH, int d, float x, float y, float w, float h) : GameEntity(l, mH, d, x, y, w, h)
	{

		this->parent = p;

	}

	Player::~Player()
	{

		if (this->weapon)
		{

			delete this->weapon;
			this->weapon = NULL;

		}

		if (this->jumpEffect)
		{

			Mix_FreeChunk(this->jumpEffect);
			this->jumpEffect = NULL;

		}

		if (this->scripted)
		{

			delete this->scripted;
			this->scripted = NULL;

		}

		for (int i = 0; i < this->inventory.size(); i++)
		{

			if (this->inventory[i])
			{

				delete this->inventory[i];
				this->inventory[i] = NULL;

			}

		}

	}

	void Player::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		ControlPreference* controls = Settings::AccessSettings()->Controls();

		this->GameEntity::Update(entities, attacks, deltaTime);

		if (controls->Pressed(START_CODE))
		{

			engine.Pause();

		}

		if (this->scripted)
		{

			if (!this->scripted->Active())
			{

				delete this->scripted;
				this->scripted = NULL;

			}
			else
			{

				this->scripted->Update(this, attacks, UI::AccessUI(), deltaTime);

			}

		}

		if (!this->scripted)
		{

			if (controls->Pressed(SELECT_CODE))
			{

				this->health = -1;
				this->Die();

			}

			//Horizontal Movement
			if (controls->Down(LEFT_CODE))
			{

				this->direction = -1;

				this->SetRawVelocity(Vector2(this->rawVelocity.X() + -this->movementSpeed, this->rawVelocity.Y()));

			}

			if (controls->Down(RIGHT_CODE))
			{

				this->direction = 1;

				this->SetRawVelocity(Vector2(this->rawVelocity.X() + this->movementSpeed, this->rawVelocity.Y()));

			}

			//Jumping
			if (!controls->Down(JUMP_CODE))
			{

				this->jumpKeyUp = true;

			}

			if (!this->jump && controls->Down(JUMP_CODE) && this->jumpTime < PLAYER_MAX_JUMP_LENGTH && !this->jumpKeyUp)
			{

				this->rawVelocity.SetY(this->rawVelocity.Y() + this->jumpAcceleration * deltaTime);
				this->jumpTime++;

			}

			if (this->jump && controls->Down(JUMP_CODE) && !controls->Down(DOWN_CODE) && this->rawVelocity.Y() < PLAYER_COYOTE_TIME * GRAVITY * deltaTime)
			{

				this->jump = false;
				this->jumpTime = 0;
				this->rawVelocity.SetY(this->jumpSpeed);

				this->jumpKeyUp = false;

				if (this->jumpEffect)
				{

					PlaySound(this->jumpEffect, this, 0);

				}

			}

			this->rawVelocity.SetY(this->rawVelocity.Y() + GRAVITY * deltaTime);

			//Jump Down
			if (controls->Down(JUMP_CODE) && controls->Down(DOWN_CODE) && this->oneWay)
			{

				this->oneWay = false;
				this->oneWayTime = GetTime();

			}

			if (!this->oneWay && GetTime() - this->oneWayTime > PLAYER_ONEWAY_TIME)
			{

				this->oneWay = true;

			}

			//Shooting
			if (controls->Down(ACTION_CODE) && this->weapon->Ready())
			{

				this->Shoot(attacks);

			}

		}

		float oldX = this->velocity.X();
		this->velocity = this->rawVelocity * deltaTime;
		this->Move(this->velocity);

		this->velocity.SetX(this->velocity.X() + oldX);

		bool shooting = !this->weapon->Ready();

		if (this->scripted)
		{

			this->scripted->HandleAnimation(deltaTime);

		}
		else
		{

			if (this->rawVelocity.Y() > GRAVITY * deltaTime)
			{

				//Falling
				this->sheet.PlayAnimation(PLAYER_ANIM_FALL + (shooting ? 1 : 0), true);

			}
			else if (this->rawVelocity.Y() < GRAVITY * deltaTime)
			{

				//Jumping
				this->sheet.PlayAnimation(PLAYER_ANIM_JUMP + (shooting ? 1 : 0), true);

			}
			else if (this->rawVelocity.X() == 0 && this->jump)
			{

				//Standing
				this->sheet.PlayAnimation(PLAYER_ANIM_STAND + (shooting ? 1 : 0), true);

			}
			else
			{

				//Running
				this->sheet.ShiftAnimation(PLAYER_ANIM_RUN + (shooting ? 1 : 0), true);

			}

		}

	}

	void Player::CanJump()
	{

		this->jump = true;

	}

	void Player::Shoot(std::vector<Attack*>& attacks)
	{

		this->weapon->Use(attacks, Vector2(this->direction, 0));

	}

	void Player::Die()
	{

		UI::AccessUI()->RequestRenderAction(NULL);

		bool fall = true;

		if (this->health > 0)
		{

			fall = false;

			this->crushed = true;

		}
		else if (this->health >= 0)
		{

			SaveData::AccessSaveData()->ClearAtRisk();

		}

		this->health = 0;

		this->active = false;

		if (this->scripted)
		{

			delete this->scripted;
			this->scripted = NULL;

		}

		this->scripted = new FallingKOAction(this->direction, fall);
		this->scripted->Start();

	}

	void Player::Pause()
	{

		this->GameEntity::Pause();

		this->weapon->Pause();

		if (this->scripted)
		{

			this->scripted->Pause();

		}

	}

	void Player::Unpause()
	{

		this->GameEntity::Unpause();

		this->weapon->Unpause();

		this->oneWayTime = this->PauseTime(this->oneWayTime);

		if (this->scripted)
		{

			this->scripted->Unpause();

		}

	}

	const char* Player::Type()
	{

		return "Player";

	}

	bool Player::OneWay()
	{

		return this->oneWay;

	}

	void Player::Load(std::ifstream& file)
	{

		this->weapon = Weapon::LoadWeapon(file, this);

		std::string line;
		std::getline(file, line);

		if (line.size() > 0)
		{

			this->jumpEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), line);

		}

		SaveData::AccessSaveData()->AugmentPlayer(this);

		this->health = this->MaxHealth();

	}

	void Player::Hurt(Attack* a)
	{

		this->GameEntity::Hurt(a);

		if (this->Health() > 0)
		{

			int direction = (a->Center().X() >= this->Center().X() ? -1 : 1);

			if (this->scripted)
			{

				delete this->scripted;
				this->scripted = NULL;

			}

			this->scripted = new PlayerHurtAction(direction, PLAYER_HURT_SPEED, INVULNERABILITY_DURATION);
			this->scripted->Start();

			this->direction = -direction;

			this->sheet.PlayAnimation(PLAYER_ANIM_HURT, true);

		}

	}

	bool Player::IsHurt()
	{

		return this->sheet.CurrentAnimation() == PLAYER_ANIM_HURT;

	}

	int Player::Respawn()
	{

		int refill = 0;

		this->crushed = false;

		if (this->scripted)
		{

			delete this->scripted;
			this->scripted = NULL;

		}

		this->sheet.Reset();
		this->koSheet.Reset();

		if (!this->active)
		{

			this->scripted = new EscapeMinigame(this->koSheet.NumAnimations(), this);
			this->scripted->Start();

		}

		this->active = true;
		this->direction = 1;

		if (this->weapon)
		{

			this->weapon->Refresh();

		}

		for (int i = 0; i < this->inventory.size(); i++)
		{

			if (Settings::AccessSettings()->RemoveByType(this->inventory[i]->Type()))
			{

				bool remove = false;

				if (this->inventory[i]->Type() == SaveData::AccessSaveData()->CurrentLevel().shorthand + "Info")
				{

					refill++;
					remove = true;

				}

				remove = remove || this->health <= 0;

				if (remove)
				{

					delete this->inventory[i];
					this->inventory[i] = NULL;

					this->inventory.erase(this->inventory.begin() + i);
					i--;

				}

			}

		}

		return refill;

	}

	void Player::RequestEvent(std::string e)
	{

		std::istringstream parser(e);

		std::string type;
		parser >> type;

		if (type == "Respawn")
		{

			this->active = false;
			this->parent->TriggerRespawn();

			this->SetRawVelocity(Vector2());

		}
		else if (type == "Door")
		{

			std::string level;
			parser >> level;

			int spawnIndex;
			parser >> spawnIndex;

			this->parent->TriggerDoor(level, spawnIndex);

		}
		else if (type == "Exit")
		{

			for (int i = 0; i < this->inventory.size(); i++)
			{

				if (this->inventory[i])
				{

					SaveData::AccessSaveData()->ProcessItem(this->inventory[i]);

				}

			}

			this->parent->TriggerExit();

		}
		else if (type == "GameOver")
		{

			Uint32 format = 0;
			SDL_Rect rect;
			
			SDL_Color col;
			col.a = col.r = col.g = col.b = 0;

			rect.x = 0;
			rect.y = 0;
			SDL_QueryTexture(this->koSheet.Surface(), &format, NULL, &rect.w, &rect.h);

			SDL_Texture* output = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);
			SDL_SetRenderTarget(renderer, output);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_SetTextureBlendMode(output, SDL_BLENDMODE_BLEND);
			ClearBuffer(col);

			SDL_RenderCopy(renderer, this->koSheet.Surface(), NULL, &rect);

			SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(NULL, rect.w, rect.h, 32, format);
			SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

			std::string fileName = "./Assets/Images/Damsels/Damsel" + SaveData::AccessSaveData()->Name() + ".png";
			IMG_SavePNG(surface, fileName.c_str());
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(output);

			SDL_SetRenderTarget(renderer, NULL);

			this->parent->TriggerGameOver();

		}
		else if (type == "Bind")
		{

			if (!this->scripted || this->scripted->Type() == "Hop")
			{

				if (this->scripted)
				{

					delete this->scripted;
					this->scripted = NULL;

				}

				this->scripted = new HopAction(this, BIND_DURATION + this->timesBound * BIND_INCREASE);
				this->scripted->Start();

				this->timesBound++;

			}

		}

	}

	void Player::AddItem(Item* i)
	{

		if (i->Type().find("CorpseRun") != std::string::npos)
		{

			UI::AccessUI()->PlayHealEffect();

			SaveData::Rescue(i->Type().substr(i->Type().find("-") + 1));

			delete i;

		}
		else if (i->Type() == "Health")
		{

			UI::AccessUI()->PlayHealEffect();

			this->health = this->MaxHealth();

			delete i;

		}
		else
		{

			this->inventory.push_back(i);

		}

	}

	bool Player::HasItems(std::string type, int count)
	{

		int c = 0;

		for (int i = 0; i < this->inventory.size(); i++)
		{

			if (this->inventory[i]->Type() == type)
			{

				c++;

			}

		}

		return c >= count;

	}

	bool Player::Scripted()
	{

		return this->scripted != NULL;

	}

	std::vector<Item*> Player::Inventory()
	{

		return this->inventory;

	}

	bool Player::ShouldMove()
	{

		return !this->crushed;

	}

}
