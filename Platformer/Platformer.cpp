//Includes
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "Platformer.h"
#include "SaveData.h"
#include "CharacterSelect.h"
#include "ResultsScreen.h"
#include "Event.h"

namespace Platformer
{

	int fps_Counter = 0;
	int numFrames = 0;
	double lastFrame = 0;

	Platformer::Platformer()
	{

		this->player = 0;
		this->level = 0;

	}

	Platformer::~Platformer()
	{

		for (int i = 0; i < this->entities.size(); i++)
		{

			if (this->entities[i])
			{

				delete this->entities[i];
				this->entities[i] = NULL;

			}

		}

		for (int i = 0; i < this->attacks.size(); i++)
		{

			if (this->attacks[i])
			{

				delete this->attacks[i];
				this->attacks[i] = NULL;

			}

		}

		if (this->level)
		{

			delete this->level;
			this->level = 0;

		}

	}

	void Platformer::Initialize(int argc, char* argv[])
	{

		this->player = new Player(this);

		this->entities.push_back(this->player);

		this->pauseText = NULL;

	}

	void Platformer::LoadContent(int argc, char* argv[])
	{

		//Quick and dirty
		if (argc >= 3)
		{

			std::istringstream parse(argv[1]);
			int spawn;
			parse >> spawn;

			this->LoadLevel(argv[0], spawn);

			this->player->LoadEntity(std::ifstream(argv[2]), true);

			this->player->Move(this->level->SpawnIndex(spawn));

		}

	}

	void Platformer::Start()
	{

		this->level->Start();

		this->framesInWall = 0;

	}

	void Platformer::LoadLevel(std::string levelName, int spawnIndex)
	{

		//Clean
		if (this->level)
		{

			delete this->level;
			this->level = NULL;

		}

		for (int i = 0; i < this->attacks.size(); i++)
		{

			delete this->attacks[i];
			this->attacks.erase(this->attacks.begin() + i);
			i--;

		}

		for (int i = 1; i < this->entities.size(); i++)
		{

			delete this->entities[i];
			this->entities[i] = NULL;

			this->entities.erase(this->entities.begin() + i);
			i--;

		}

		for (int i = 0; i < this->items.size(); i++)
		{

			delete this->items[i];
			this->items[i] = NULL;

			this->items.erase(this->items.begin() + i);
			i--;

		}

		//Load New Level
		this->level = new Level(this);
		this->level->LoadLevel(levelName);

		this->currentLevel = levelName;

		Vector2 spawn;
		if (spawnIndex >= 0)
		{

			spawn = this->level->SpawnIndex(spawnIndex);

		}
		else
		{

			spawn = this->level->SpawnPoint(Vector2(this->player->X(), this->player->Y()));

		}

		this->player->SetX(spawn.X());
		this->player->SetY(spawn.Y());

		this->player->SetLevel(this->level);

		this->level->Refill(this->player->Respawn());

	}

	void Platformer::Update(float deltaTime)
	{

		this->RemoveEntities();
		this->RemoveAttacks();

		this->player->SetVelocity(Vector2());

		this->level->Update(this->entities, this->attacks, deltaTime);

		this->player->SetRawVelocity(Vector2(0, this->player->RawVelocity().Y()));

		this->UpdatePlayer(deltaTime);

		for (int i = 1; i < this->entities.size(); i++)
		{

			this->entities[i]->Update(this->entities, this->attacks, deltaTime);

			if (this->entities[i]->ShouldMove())
			{

				this->level->Collision()->PlatformCollision(this->entities[i], true);

			}

		}

		this->UpdateAttacks(deltaTime);

		if (this->level->Collision()->Intersects(this->player, false))
		{

			if (this->framesInWall >= FRAMES_IN_WALL_MAX)
			{

				this->player->Die();

				this->player->RequestEvent("Respawn");

				this->framesInWall = -1;

			}

			this->framesInWall++;

		}
		else
		{

			this->framesInWall = 0;

		}

		for (int i = 0; i < this->items.size(); i++)
		{

			this->items[i]->Update(deltaTime);

			if (this->player->Intersects(this->items[i]->Box()))
			{

				if (this->items[i]->Block())
				{

					this->RegisterBlock(this->items[i]->Name());

				}

				this->player->AddItem(this->items[i]);

				this->items.erase(this->items.begin() + i);
				i--;

				UI::AccessUI()->PlayItemEffect();

			}

		}

		PumpSounds(this->world);

		if (engine.Transitioned())
		{

			this->LoadLevel(this->currentLevel, this->spawn);
			engine.BeginScreenTransition(-1);

			this->level->Start();

		}

		if (!this->level->Intersects(this->player))
		{

			if (this->player->Active() || this->player->Scripted())
			{

				this->player->Die();

				this->player->RequestEvent("Respawn");

			}

		}

	}

	void Platformer::RemoveEntities()
	{

		for (int i = 1; i < this->entities.size(); i++)
		{

			if (!this->entities[i]->Active())
			{

				//No dangling references on attacks
				for (int j = 0; j < this->attacks.size(); j++)
				{

					if(this->attacks[j]->Parent() == this->entities[i])
					{

						this->attacks[j]->SetParent(NULL);

					}

				}

				delete this->entities[i];
				this->entities[i] = NULL;

				this->entities.erase(this->entities.begin() + i);
				i--;

			}

		}

	}

	void Platformer::RemoveAttacks()
	{

		for (int i = 0; i < this->attacks.size(); i++)
		{

			if (this->attacks[i]->ShouldRemove(this->world))
			{

				delete this->attacks[i];
				this->attacks.erase(this->attacks.begin() + i);
				i--;

			}

		}

	}

	void Platformer::UpdatePlayer(float deltaTime)
	{

		this->player->Update(this->entities, this->attacks, deltaTime);

		Vector2 oldLocation(this->player->X(), this->player->Y());
		if (this->level->Collision()->PlatformCollision(this->player, this->player->OneWay()))
		{

			this->player->CanJump();

		}

		Vector2 newLocation(this->player->X(), this->player->Y());
		newLocation = newLocation - oldLocation;
		if (newLocation.LengthSquared() >= MOVEMENT_MAX * MOVEMENT_MAX)
		{

			this->player->SetX(oldLocation.X());
			this->player->SetY(oldLocation.Y());

			this->player->Die();
			this->player->RequestEvent("Respawn");

		}

		this->BoundCamera();

	}

	void Platformer::BoundCamera()
	{

		this->world.SetX(this->player->X() - (PLATFORMER_TARGET_WIDTH / 2));
		this->world.SetY(this->player->Y() - (PLATFORMER_TARGET_HEIGHT / 2));

		float x = this->world.X();
		float w = PLATFORMER_TARGET_WIDTH;
		float w2 = this->level->Width();

		if (this->world.X() < 0)
		{

			this->world.SetX(0);

		}

		if (this->world.X() + PLATFORMER_TARGET_WIDTH > this->level->Width())
		{

			this->world.SetX(this->level->Width() - PLATFORMER_TARGET_WIDTH);

		}

		if (this->world.Y() < 0)
		{

			this->world.SetY(0);

		}

		if (this->world.Y() + PLATFORMER_TARGET_HEIGHT > this->level->Height())
		{

			this->world.SetY(this->level->Height() - PLATFORMER_TARGET_HEIGHT);

		}

		if(this->level->LockX() != -1)
		{

			this->world.SetX(this->level->LockX());

		}

		if(this->level->LockY() != -1)
		{

			this->world.SetY(this->level->LockY());

		}

		if (this->player->IsHurt())
		{

			this->world.SetX(this->world.X() + cos(GetTime()) * SCREEN_SHAKE_HURT);
			this->world.SetY(this->world.Y() + sin(GetTime()) * SCREEN_SHAKE_HURT);

		}

	}

	void Platformer::UpdateAttacks(float deltaTime)
	{

		for (int i = 0; i < this->attacks.size(); i++)
		{

			if (this->attacks[i]->Active())
			{

				this->attacks[i]->Update(this->entities, deltaTime);

			}

		}

	}

	void Platformer::Render(float deltaTime)
	{

		this->level->DrawBackground(this->world, deltaTime);

		for (int i = 0; i < this->attacks.size(); i++)
		{

			this->attacks[i]->Render(this->world, deltaTime);

		}

		for (int i = 0; i < this->entities.size(); i++)
		{

			this->entities[i]->Render(this->world, deltaTime);

		}

		for (int i = 0; i < this->items.size(); i++)
		{

			this->items[i]->Render(this->world, deltaTime);

		}

		this->level->DrawForeground(this->world, deltaTime);

		if (Settings::AccessSettings()->Debug())
		{

			this->DebugRender(deltaTime);

		}

		UI::AccessUI()->Render(this->player->MaxHealth(), this->player->Health(), this->player->HealthPercentage(), deltaTime);
		UI::AccessUI()->RenderInventory(this->player->Inventory(), deltaTime);

		this->level->DrawUI(UI::AccessUI(), deltaTime);

		if (engine.Paused())
		{

			SDL_SetTextureColorMod(blankSurface, 64, 64, 64);
			SDL_SetTextureAlphaMod(blankSurface, 160);

			SDL_Rect dest = { 0, 0, PLATFORMER_TARGET_WIDTH, PLATFORMER_TARGET_HEIGHT };
			SDL_RenderCopyEx(renderer, blankSurface, NULL, &dest, 0, NULL, SDL_FLIP_NONE);

			if (!this->pauseText)
			{

				UI::AccessUI()->RenderText(Vector2(PLATFORMER_TARGET_WIDTH / 2.0, PLATFORMER_TARGET_HEIGHT / 2.0), "Paused", { 255, 255, 255 });

				UI::AccessUI()->RenderText(Vector2((PLATFORMER_TARGET_WIDTH / 2.0) + 16, (PLATFORMER_TARGET_HEIGHT / 2.0) + 64), "Resume", { 224, 224, 224 }, false);
				UI::AccessUI()->RenderText(Vector2((PLATFORMER_TARGET_WIDTH / 2.0) + 16, (PLATFORMER_TARGET_HEIGHT / 2.0) + 96), "Title Screen", { 224, 224, 224 }, false);
				UI::AccessUI()->RenderText(Vector2((PLATFORMER_TARGET_WIDTH / 2.0) + 16, (PLATFORMER_TARGET_HEIGHT / 2.0) + 128), "Quit", { 224, 224, 224 }, false);

				dest.w = dest.h = 32;
				dest.x = (PLATFORMER_TARGET_WIDTH / 2.0) - 64;
				dest.y = (PLATFORMER_TARGET_HEIGHT / 2.0) + 56;

				SDL_Texture* tex = UI::AccessUI()->InputsTexture();

				SDL_Rect* src = UI::AccessUI()->InputSource(START_CODE);
				SDL_RenderCopy(renderer, tex, src, &dest);

				dest.y += 32;
				src = UI::AccessUI()->InputSource(ACTION_CODE);
				SDL_RenderCopy(renderer, tex, src, &dest);

				dest.y += 32;
				src = UI::AccessUI()->InputSource(SELECT_CODE);
				SDL_RenderCopy(renderer, tex, src, &dest);

			}
			else
			{

				for (int i = 0; i < this->pauseText->text.size(); i++)
				{

					UI::AccessUI()->RenderText(this->pauseText->text[i].first, this->pauseText->text[i].second, { 255, 255, 255 }, false);

				}

				for (int i = 0; i < this->pauseText->inputs.size(); i++)
				{

					SDL_Texture* tex = UI::AccessUI()->InputsTexture();

					SDL_Rect* src = UI::AccessUI()->InputSource(this->pauseText->inputs[i].second);

					SDL_Rect dest;
					dest.w = dest.h = 32;
					dest.x = this->pauseText->inputs[i].first.X();
					dest.y = this->pauseText->inputs[i].first.Y();

					SDL_RenderCopy(renderer, tex, src, &dest);

				}

			}

		}

	}

	void Platformer::DebugRender(float deltaTime)
	{

		for (int i = 0; i < this->entities.size(); i++)
		{

			FillAABB(this->entities[i], this->world, 0, 0, 0, 100);

		}

		for (int i = 0; i < this->attacks.size(); i++)
		{

			FillAABB(this->attacks[i], this->world, 0, 0, 0, 75);

		}

		for (int i = 0; i < this->items.size(); i++)
		{

			FillAABB(this->items[i]->Box(), this->world, 160, 32, 240, 75);

		}

		this->level->DebugDraw(this->world, deltaTime);

		/*SDL_Color textColor;
		textColor.g = textColor.b = 125;
		textColor.r = 255;

		if (numFrames >= DEBUG_FRAME_COUNTER || fps_Counter < 0)
		{

			fps_Counter = 0;
			numFrames = 0;

		}

		fps_Counter += 1000.0 / (GetTime() - lastFrame);
		lastFrame = GetTime();
		numFrames++;

		std::ostringstream parse;
		parse << fps_Counter / numFrames;
		
		UI::AccessUI()->RenderText(Vector2(DEBUG_FPS_X, DEBUG_FPS_Y), parse.str(), textColor, true);

		Broken by framerate changes, maybe fix sometime */
	}

	void Platformer::Pause()
	{

		this->level->Pause();

		for (int i = 0; i < this->entities.size(); i++)
		{

			this->entities[i]->Pause();

		}

		for (int i = 0; i < this->attacks.size(); i++)
		{

			this->attacks[i]->Pause();

		}

	}

	void Platformer::Unpause()
	{

		this->level->Unpause();

		for (int i = 0; i < this->entities.size(); i++)
		{

			this->entities[i]->Unpause();

		}

		for (int i = 0; i < this->attacks.size(); i++)
		{

			this->attacks[i]->Unpause();

		}

		this->pauseText = NULL;

	}

	bool Platformer::UnpauseCondition()
	{

		if (!this->pauseText)
		{

			if (Settings::AccessSettings()->Controls()->Pressed(ACTION_CODE))
			{

				this->SetRunning(false);

				SaveData::AccessSaveData()->ClearAtRisk();
				SaveData::AccessSaveData()->SetFaux(std::vector<bool>());
				EscapeMinigame::Reset();

			}

			if (Settings::AccessSettings()->Controls()->Pressed(SELECT_CODE))
			{

				engine.EndMainLoop();

			}

		}

		if (this->pauseKeyUp)
		{

			if (Settings::AccessSettings()->Controls()->Lifted(START_CODE))
			{

				this->pauseKeyUp = false;

				return true;

			}

		}

		if (!this->pauseKeyUp && !Settings::AccessSettings()->Controls()->Down(START_CODE))
		{

			this->pauseKeyUp = true;

		}

		if (engine.Transitioned())
		{

			this->LoadLevel(this->currentLevel, this->spawn);
			engine.BeginScreenTransition(-1);

			this->level->Start();

		}

		return false;
	}

	void Platformer::Shutdown()
	{

		SaveData::AccessSaveData()->ExportSaveData();

		for (int i = 0; i < this->attacks.size(); i++)
		{

			delete this->attacks[i];
			this->attacks.erase(this->attacks.begin() + i);
			i--;

		}

		for (int i = 1; i < this->entities.size(); i++)
		{

			delete this->entities[i];
			this->entities[i] = NULL;

			this->entities.erase(this->entities.begin() + i);
			i--;

		}

		for (int i = 0; i < this->items.size(); i++)
		{

			delete this->items[i];
			this->items[i] = NULL;

			this->items.erase(this->items.begin() + i);
			i--;

		}

	}

	void Platformer::RegisterAttack(Attack* a)
	{

		this->attacks.push_back(a);

	}

	void Platformer::RegisterEntity(GameEntity* e)
	{

		this->entities.push_back(e);

	}

	void Platformer::RegisterBlock(std::string b)
	{

		this->blocked.push_back(b);

	}

	void Platformer::AddItem(Item* i)
	{

		this->items.push_back(i);

	}

	void Platformer::AddToInventory(Item* i)
	{

		this->player->AddItem(i);

	}

	bool Platformer::SearchType(std::string t)
	{

		for (int i = 0; i < this->entities.size(); i++)
		{

			if (entities[i]->Type() == t)
			{

				return true;

			}

		}

		return false;
	}

	bool Platformer::Blocked(std::string b)
	{

		return std::find(this->blocked.begin(), this->blocked.end(), b) != this->blocked.end();

	}

	void Platformer::TriggerRespawn()
	{

		if (!this->player->Active())
		{

			this->spawn = -1;

			engine.BeginScreenTransition(1);

		}

	}

	void Platformer::TriggerExit()
	{

		ResultsScreen* results = new ResultsScreen(SaveData::AccessSaveData()->NumDamsels(), SaveData::AccessSaveData()->NumDossiers(), SaveData::AccessSaveData()->NumInfo(), SaveData::AccessSaveData()->CurrentLevel().reputationForCompletion);
		results->Initialize(0, NULL);
		results->LoadContent(0, NULL);
		results->SetRunning(true);

		SaveData::AccessSaveData()->ExitLevel(this->currentLevel);
		EscapeMinigame::Reset();

		this->SetRunning(false);

		engine.RegisterState(results);

	}

	void Platformer::TriggerGameOver()
	{

		SaveData::AccessSaveData()->GameOver();
		EscapeMinigame::Reset();

		char* files[3];
		files[0] = "./Assets/Images/Pieces/Base";
		files[1] = "./Saves/Customization.plCTM";
		files[2] = "./Assets/Data/Spritesheet/Player.plSPS";

		CharacterSelect* cust = new CharacterSelect();
		cust->Initialize(0, NULL);
		cust->LoadContent(3, files);
		cust->SetRunning(true);

		engine.RegisterState(cust);

		SplashPage* gameOverSplash = new SplashPage(cust);
		gameOverSplash->Initialize(0, NULL);

		char* content[1];
		content[0] = "./Assets/Data/Menu/GameOver.plSPL";
		gameOverSplash->LoadContent(1, content);
		gameOverSplash->SetRunning(true);

		engine.RegisterState(gameOverSplash);

		this->SetRunning(false);

	}

	void Platformer::TriggerDoor(std::string level, int spawnIndex)
	{

		this->spawn = spawnIndex;
		this->currentLevel = level;

		engine.BeginScreenTransition(1);

	}

	void Platformer::TriggerPauseText(PauseText* t)
	{

		this->pauseText = t;

		engine.Pause();

	}

}
