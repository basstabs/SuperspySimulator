//Includes
#include <SDL_image.h>
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "LevelSelect.h"
#include "Platformer.h"
#include "Settings.h"
#include "SaveData.h"
#include "Customizer.h"

namespace Platformer
{

	void LevelSelect::Initialize(int argc, char* argv[])
	{

		this->currentLevel = 0;

	}

	void LevelSelect::LoadContent(int argc, char* argv[])
	{

		this->MenuFactory("./Assets/Data/Menu/LevelSelect.plMNU", Settings::AccessSettings()->MusicVolume());

		if (argc > 1)
		{

			std::ifstream file(argv[0]);

			std::string line;
			std::getline(file, line);
			std::istringstream input(line);

			int numLevels;
			input >> numLevels;

			for (int i = 0; i < numLevels; i++)
			{

				LevelData data;

				std::getline(file, line);
				input.clear();
				input.str(line);

				input >> data.numDamsels;
				input >> data.numDossiers;
				input >> data.reputationForCompletion;

				std::getline(file, data.name);
				std::getline(file, data.shorthand);
				std::getline(file, data.startLevel);

				std::getline(file, line);
				input.clear();
				input.str(line);

				input >> data.startSpawn;

				this->levels.push_back(data);

				std::getline(file, line);
				this->levelSurfaces.push_back(LoadTexture(line));

			}

			file.close();

			this->frame = LoadTexture(argv[1]);

		}

	}

	void LevelSelect::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		if (this->inputs["Start"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->start = true;

			SaveData::AccessSaveData()->SetCurrentLevel(this->levels[this->currentLevel]);

			this->SetRunning(false);

		}

		if (this->inputs["LevelUp"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->currentLevel = (this->currentLevel + 1) % this->levels.size();

		}

		if (this->inputs["LevelDown"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			if (this->currentLevel == 0)
			{

				this->currentLevel = this->levels.size();

			}

			this->currentLevel--;

		}

		if (this->inputs["Back"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->SetRunning(false);

		}

	}

	void LevelSelect::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		UI::AccessUI()->RenderReputation();

		SDL_Rect rect;
		SDL_QueryTexture(this->levelSurfaces[this->currentLevel], NULL, NULL, &rect.w, &rect.h);
		rect.x = PLATFORMER_TARGET_WIDTH / 2 - rect.w / 2;
		rect.y = PLATFORMER_TARGET_HEIGHT / 2 - rect.h / 2;

		SDL_RenderCopy(renderer, this->levelSurfaces[this->currentLevel], NULL, &rect);

		SDL_QueryTexture(this->frame, NULL, NULL, &rect.w, &rect.h);
		rect.x = PLATFORMER_TARGET_WIDTH / 2 - rect.w / 2;
		rect.y = PLATFORMER_TARGET_HEIGHT / 2 - rect.h / 2;

		SDL_RenderCopy(renderer, this->frame, NULL, &rect);

		SDL_Color textColor;
		textColor.r = SELECT_COLOR_R;
		textColor.g = SELECT_COLOR_G;
		textColor.b = SELECT_COLOR_B;
		textColor.a = SELECT_COLOR_A;

		UI::AccessUI()->RenderText(Vector2(PLATFORMER_TARGET_WIDTH / 2, PLATFORMER_TARGET_HEIGHT / 2 + rect.h / 2 + 32), this->levels[this->currentLevel].name, textColor, true);

		if (SaveData::AccessSaveData()->CompletedLevel(this->levels[this->currentLevel].shorthand))
		{

			std::string text;

			std::ostringstream convert;
			convert << SaveData::AccessSaveData()->DamselData(this->levels[this->currentLevel].shorthand);
			text = convert.str() + " / ";
			convert.str("");
			convert.clear();
			convert << this->levels[this->currentLevel].numDamsels;
			text += convert.str() + " Damsels Rescued";

			if (this->levels[this->currentLevel].numDamsels == SaveData::AccessSaveData()->DamselData(this->levels[this->currentLevel].shorthand))
			{

				textColor.g = 128;
				textColor.r = SELECT_COLOR_R;

			}
			else
			{

				textColor.r = 128;
				textColor.g = SELECT_COLOR_G;

			}

			UI::AccessUI()->RenderText(Vector2(PLATFORMER_TARGET_WIDTH / 2, PLATFORMER_TARGET_HEIGHT / 2 + rect.h / 2 + 64), text, textColor, true);

			convert.str("");
			convert.clear();
			convert << SaveData::AccessSaveData()->DossierData(this->levels[this->currentLevel].shorthand);
			text = convert.str() + " / ";
			convert.str("");
			convert.clear();
			convert << this->levels[this->currentLevel].numDossiers;
			text += convert.str() + " Dossiers Collected";

			if (this->levels[this->currentLevel].numDossiers == SaveData::AccessSaveData()->DossierData(this->levels[this->currentLevel].shorthand))
			{

				textColor.g = 128;
				textColor.r = SELECT_COLOR_R;

			}
			else
			{

				textColor.g = SELECT_COLOR_G;
				textColor.r = 128;

			}


			UI::AccessUI()->RenderText(Vector2(PLATFORMER_TARGET_WIDTH / 2, PLATFORMER_TARGET_HEIGHT / 2 + rect.h / 2 + 96), text, textColor, true);

		}

		UI::AccessUI()->RenderMouse();

	}

	void LevelSelect::Pause()
	{

	}

	void LevelSelect::Unpause()
	{

	}

	bool LevelSelect::UnpauseCondition()
	{

		return true;

	}

	void LevelSelect::Shutdown()
	{

		if (this->start)
		{

			LevelData data = SaveData::AccessSaveData()->CurrentLevel();

			this->CreateDamsels(data.numDamsels);

			this->GeneratePlayerSprites();

			Platformer* platformer = new Platformer();
			platformer->Initialize(0, NULL);

			std::string level = "./Assets/Data/Level/" + data.startLevel;
			std::ostringstream spawn;
			spawn << data.startSpawn;
			std::string st = spawn.str();

			char* files[3];
			files[0] = (char*)level.c_str();
			files[1] = (char*)st.c_str();
			files[2] = "./Assets/Data/Master/Player.plETY";
			platformer->LoadContent(3, files);
			platformer->SetRunning(true);

			engine.RegisterState(platformer);

		}

		for (int i = 0; i < this->levelSurfaces.size(); i++)
		{

			if (this->levelSurfaces[i])
			{

				SDL_DestroyTexture(this->levelSurfaces[i]);
				this->levelSurfaces[i] = NULL;

			}

		}

		if (this->frame)
		{

			SDL_DestroyTexture(this->frame);
			this->frame = NULL;

		}

	}

	void LevelSelect::CreateDamsels(int n)
	{

		CustomizerCore core;
		core.Load("./Assets/Images/Pieces/Base", "./Saves/Customization.plCTM", "./Assets/Images/Pieces/" + (this->levels[this->currentLevel].shorthand == "Tutorial" ? "Tropic" : this->levels[this->currentLevel].shorthand));

		std::ostringstream index;

		for (int i = 0; i < n; i++)
		{

			core.currentHair = core.currentCostume = -1;

			while ((core.currentHair < 0 && core.currentCostume < 0) || (core.currentHair == SaveData::AccessSaveData()->HairIndex() && core.currentCostume == SaveData::AccessSaveData()->CostumeIndex()))
			{

				core.currentHair = RandomBoundedInteger(0, core.hairStyles.size() - 1);
				core.currentCostume = RandomBoundedInteger(0, core.costumes.size() - 1);

			}

			int colorIndex = RandomBoundedInteger(0, core.hairProfiles.size() - 1);
			core.hairColor = core.hairProfiles[colorIndex];

			index << i;

			core.PreparePlayer();
			core.OutputPlayer("", "./Assets/Images/Damsels/Damsel" + index.str() + ".png");

			index.str("");
			index.clear();

		}

	}

	void LevelSelect::GeneratePlayerSprites()
	{

		std::string bondsName = (this->levels[this->currentLevel].shorthand == "Tutorial" ? "Tropic" : this->levels[this->currentLevel].shorthand);

		//Heroine
		SDL_Texture* base = LoadTexture(OUTPUT_PLAYER_FILE);
		SDL_Texture* weapon = LoadTexture("./Assets/Images/Pieces/" + SaveData::AccessSaveData()->WeaponImageName());
		SDL_Texture* bonds = LoadTexture("./Assets/Images/Pieces/" + bondsName + "Bonds.png");

		Uint32 format = 0;
		SDL_Rect rect;

		SDL_Color col;
		col.a = col.r = col.g = col.b = 0;

		rect.x = 0;
		rect.y = 0;
		SDL_QueryTexture(base, &format, NULL, &rect.w, &rect.h);

		SDL_Texture* output = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);
		SDL_SetRenderTarget(renderer, output);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(output, SDL_BLENDMODE_BLEND);
		ClearBuffer(col);

		SDL_RenderCopy(renderer, base, NULL, &rect);
		SDL_RenderCopy(renderer, weapon, NULL, &rect);
		SDL_RenderCopy(renderer, bonds, NULL, &rect);

		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(NULL, rect.w, rect.h, 32, format);
		SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

		IMG_SavePNG(surface, "./Assets/Images/Heroine.png");
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(output);

		SDL_SetRenderTarget(renderer, NULL);

		SDL_DestroyTexture(bonds);
		SDL_DestroyTexture(weapon);
		SDL_DestroyTexture(base);

		//HeroineKO
		bonds = LoadTexture("./Assets/Images/Pieces/" + bondsName + "BondsKO.png");
		base = LoadTexture(OUTPUT_PLAYERKO_FILE);
		SDL_Texture* bondsFore = LoadTexture("./Assets/Images/Pieces/" + bondsName + "BondsForeKO.png");

		col.a = col.r = col.g = col.b = 0;

		rect.x = 0;
		rect.y = 0;
		SDL_QueryTexture(base, &format, NULL, &rect.w, &rect.h);

		output = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rect.w, rect.h);
		SDL_SetRenderTarget(renderer, output);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(output, SDL_BLENDMODE_BLEND);
		ClearBuffer(col);

		SDL_RenderCopy(renderer, bonds, NULL, &rect);
		SDL_RenderCopy(renderer, base, NULL, &rect);
		SDL_RenderCopy(renderer, bondsFore, NULL, &rect);

		surface = SDL_CreateRGBSurfaceWithFormat(NULL, rect.w, rect.h, 32, format);
		SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

		IMG_SavePNG(surface, "./Assets/Images/HeroineKO.png");
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(output);

		SDL_SetRenderTarget(renderer, NULL);

		SDL_DestroyTexture(bondsFore);
		SDL_DestroyTexture(base);
		SDL_DestroyTexture(bonds);

	}

}
