//Includes

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "ScriptedAction.h"
#include "Player.h"
#include "SaveData.h"

using namespace Engine2D;

namespace Platformer
{

	int EscapeMinigame::hard = 0;

	ScriptedAction::ScriptedAction() : ScriptedAction(0)
	{

	}

	ScriptedAction::ScriptedAction(float d)
	{

		this->duration = d;

	}

	ScriptedAction::~ScriptedAction()
	{

		UI::AccessUI()->RequestRenderAction(NULL);

	}

	void ScriptedAction::Start()
	{
		
		this->startTime = GetTime();

	}

	void ScriptedAction::Render()
	{

	}

	bool ScriptedAction::Active()
	{

		return ((GetTime() - this->startTime) <= this->duration);

	}

	void ScriptedAction::HandleAnimation(float deltaTime)
	{

	}

	void ScriptedAction::Pause()
	{

		this->paused = true;
		this->pauseTime = GetTime();

	}

	void ScriptedAction::Unpause()
	{

		this->paused = false;
		this->startTime = GetTime() - (this->pauseTime - this->startTime);

	}

	PlayerHurtAction::PlayerHurtAction() : ScriptedAction()
	{

	}

	PlayerHurtAction::PlayerHurtAction(int direction, float s, float d) : ScriptedAction(d)
	{

		this->speed = s;
		this->direction = direction;

	}

	PlayerHurtAction::~PlayerHurtAction()
	{

	}

	void PlayerHurtAction::Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime)
	{

		obj->SetRawVelocity(Vector2(this->speed * this->direction, obj->RawVelocity().Y() + GRAVITY * deltaTime));
		obj->SetVelocity(obj->Velocity() * deltaTime);
		obj->Move(obj->Velocity());

	}

	std::string PlayerHurtAction::Type()
	{

		return "PlayerHurt";

	}

	FallingKOAction::FallingKOAction() : ScriptedAction()
	{

	}

	FallingKOAction::FallingKOAction(float d, bool f) : ScriptedAction(d)
	{

		this->falling = f;

	}

	FallingKOAction::~FallingKOAction()
	{

	}

	void FallingKOAction::Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime)
	{

		if (!this->play && this->falling && obj->RawVelocity().Y() == 0)
		{

			obj->PlayAnimation(0, false);
			this->play = true;

			obj->RequestEvent("Respawn");

		}

		if (obj->RawVelocity().Y() >= 0)
		{

			this->falling = true;

		}

		obj->SetRawVelocity(Vector2(0, obj->RawVelocity().Y() + GRAVITY * deltaTime));

		obj->SetVelocity(obj->RawVelocity() * deltaTime);
		
		obj->Move(obj->Velocity());

	}

	bool FallingKOAction::Active()
	{

		return true;

	}

	std::string FallingKOAction::Type()
	{

		return "FallingKO";

	}

	HopAction::HopAction(Player* p) : HopAction(p, 0)
	{

	}

	HopAction::HopAction(Player* p, float d) : ScriptedAction(d)
	{

		this->parent = p;
		this->parent->sheet.PlayAnimation(PLAYER_ANIM_CAPTURE, false);

		this->parent->SetRawVelocity(Vector2());

		this->captured = false;

	}

	HopAction::~HopAction()
	{

	}

	void HopAction::Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime)
	{

		if(!this->captured)
		{

			this->startTime = GetTime();

			if (!this->parent->sheet.Playing())
			{

				this->captured = true;

			}

		}

		if (this->captured)
		{

			ControlPreference* controls = Settings::AccessSettings()->Controls();

			if (this->parent->rawVelocity.Y() != 0)
			{

				//Horizontal Movement
				if (controls->Down(LEFT_CODE))
				{

					this->parent->direction = -1;

					this->parent->SetRawVelocity(Vector2(this->parent->rawVelocity.X() + -this->parent->movementSpeed, this->parent->rawVelocity.Y()));

				}

				if (controls->Down(RIGHT_CODE))
				{

					this->parent->direction = 1;

					this->parent->SetRawVelocity(Vector2(this->parent->rawVelocity.X() + this->parent->movementSpeed, this->parent->rawVelocity.Y()));

				}

			}

			//Jumping
			if (!controls->Down(JUMP_CODE))
			{

				this->jumpKeyUp = true;

			}

			if (!this->parent->jump && controls->Down(JUMP_CODE) && this->parent->jumpTime < PLAYER_MAX_JUMP_LENGTH && !this->jumpKeyUp)
			{

				this->parent->rawVelocity.SetY(this->parent->rawVelocity.Y() + this->parent->jumpAcceleration * deltaTime);
				this->parent->jumpTime++;

			}

			if (this->parent->jump && controls->Down(JUMP_CODE) && !controls->Down(DOWN_CODE) && this->parent->rawVelocity.Y() < PLAYER_COYOTE_TIME * GRAVITY * deltaTime)
			{

				this->parent->jump = false;
				this->parent->jumpTime = 0;
				this->parent->rawVelocity.SetY(this->parent->jumpSpeed);

				this->jumpKeyUp = false;

				if (this->parent->jumpEffect)
				{

					PlaySound(this->parent->jumpEffect, this->parent, 0);

				}

			}

			this->parent->rawVelocity.SetY(this->parent->rawVelocity.Y() + GRAVITY * deltaTime);

			//Jump Down
			if (controls->Down(JUMP_CODE) && controls->Down(DOWN_CODE) && this->parent->oneWay)
			{

				this->parent->oneWay = false;
				this->parent->oneWayTime = GetTime();

			}

			if (!this->parent->oneWay && GetTime() - this->parent->oneWayTime > PLAYER_ONEWAY_TIME)
			{

				this->parent->oneWay = true;

			}

		}

	}

	void HopAction::HandleAnimation(float deltaTime)
	{

		if (this->captured)
		{

			if (this->parent->jump)
			{

				//Standing
				this->parent->sheet.PlayAnimation(PLAYER_ANIM_HOP_STAND, true);

			}
			else if (this->parent->rawVelocity.Y() < GRAVITY * deltaTime)
			{

				//Jumping
				this->parent->sheet.PlayAnimation(PLAYER_ANIM_HOP_JUMP, true);

			}
			else
			{

				//Falling
				this->parent->sheet.PlayAnimation(PLAYER_ANIM_HOP_FALL, true);

			}

		}

	}

	std::string HopAction::Type()
	{

		return "Hop";

	}

	EscapeMinigame::EscapeMinigame(int n, Player* p) : EscapeMinigame(n, p, 0)
	{

	}

	EscapeMinigame::EscapeMinigame(int n, Player* p, float d) : ScriptedAction(d)
	{

		this->parent = p;

		p->PlayAnimation(RandomBoundedInteger(1, n - 1), true);

		EscapeMinigame::hard += 1;
		this->difficulty = EscapeMinigame::hard - SaveData::AccessSaveData()->EscapeBonus();
		this->difficulty = fminf(fmaxf(this->difficulty, 1), ESCAPE_MAX_DIFFICULTY);

		this->lastButton = GetTime();

		std::string shorthand = SaveData::AccessSaveData()->CurrentLevel().shorthand;
		this->base = LoadTexture("./Assets/Images/Pieces/" + shorthand + "EscapeBarBase.png");
		this->filler = LoadTexture("./Assets/Images/Pieces/" + shorthand + "EscapeBarFill.png");
		this->hair = SaveData::AccessSaveData()->EscapeHairFileName();

		this->success = 0.5;

	}

	EscapeMinigame::~EscapeMinigame()
	{

		if (this->base)
		{

			SDL_DestroyTexture(this->base);
			this->base = NULL;

		}

		if (this->filler)
		{

			SDL_DestroyTexture(this->filler);
			this->filler = NULL;

		}

		if (this->hair)
		{

			SDL_DestroyTexture(this->hair);
			this->hair = NULL;

		}

	}

	void EscapeMinigame::Update(GameEntity* obj, std::vector<Attack*> attacks, UI* ui, float deltaTime)
	{

		if (GetTime() - this->startTime >= ESCAPE_START_TIME)
		{

			if (this->button >= 0)
			{

				ControlPreference* controls = Settings::AccessSettings()->Controls();

				bool fail = false;

				for (int i = 0; i < 8; i++)
				{

					if (i != this->button && controls->Pressed(i))
					{

						fail = true;

						break;

					}

				}

				fail = fail || (GetTime() - this->lastButton >= this->EscapeDuration());

				if (fail)
				{

					this->success -= 1.0 / (2 * (this->difficulty + ESCAPE_BASE_DIVISION));
					this->button = -1;

					this->lastButton = GetTime();

					ui->PlayCancelEffect();

				}

				if (controls->Pressed(this->button))
				{

					this->success += 1.0 / (2 * (this->difficulty + ESCAPE_BASE_DIVISION));
					this->button = -1;

					this->lastButton = GetTime();

					ui->PlayConfirmEffect();

				}

				std::vector<int> buttonVector;
				buttonVector.push_back(this->button);
				ui->RequestRenderInput(buttonVector, ESCAPE_TRANSPARENCY);

			}

			if (this->button < 0 && GetTime() - this->lastButton >= ESCAPE_INTERVAL)
			{

				this->PickButton();

				this->lastButton = GetTime();

			}

			if (this->success >= 1)
			{

				this->parent->MaximizeHealth();

				ui->PlayHealEffect();

			}
			else if (this->success <= 0)
			{

				this->parent->RequestEvent("GameOver");

			}

		}

		UI::AccessUI()->RequestRenderAction(this);

	}

	void EscapeMinigame::Render()
	{

		Vector2 location(7 * PLATFORMER_TARGET_WIDTH / 8, PLATFORMER_TARGET_HEIGHT / 4);

		int w, h;

		SDL_Rect dest;
		SDL_QueryTexture(this->base, NULL, NULL, &w, &h);

		dest.x = location.X() - w / 2;
		dest.y = location.Y() - h / 2;
		dest.w = w;
		dest.h = h;

		SDL_RenderCopy(renderer, this->base, NULL, &dest);

		int srch;

		SDL_Rect src;
		src.x = 0;
		src.y = 0;
		SDL_QueryTexture(this->filler, NULL, NULL, &src.w, &srch);
		dest.h = src.h = (this->success) * (srch);
		src.y = srch - src.h;

		dest.y += h - dest.h;

		SDL_RenderCopy(renderer, this->filler, &src, &dest);
		
		dest.x = location.X() - w / 2;
		dest.y = location.Y() - h / 2;
		dest.w = w;
		dest.h = h;

		SDL_RenderCopy(renderer, this->hair, NULL, &dest);

	}

	void EscapeMinigame::PickButton()
	{

		//TODO: Make this more robust
		this->button = RandomBoundedInteger(0, 3);

	}

	bool EscapeMinigame::Active()
	{

		return this->success < 1;

	}

	double EscapeMinigame::EscapeDuration()
	{

		return (fmaxf(1, ESCAPE_MAX_DIFFICULTY - this->difficulty) / ESCAPE_MAX_DIFFICULTY) * ESCAPE_DURATION;

	}

	void EscapeMinigame::Reset()
	{

		EscapeMinigame::hard = 1;

	}

	std::string EscapeMinigame::Type()
	{
		
		return "EscapeMinigame";

	}

}
