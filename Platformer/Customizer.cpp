//Includes
#include <fstream>
#include <sstream>
#include <SDL_image.h>

//Custom Includes
#include "Customizer.h"
#include "Helpers.h"
#include "Platformer.h"
#include "SaveData.h"
#include "Settings.h"
#include "StatsMenu.h"

namespace Platformer
{

	Customizer::Customizer(bool r)
	{

		this->randomize = r;

		this->core.currentHair = 0;
		this->core.currentCostume = 0;

		this->core.hairColor.r = 255;
		this->core.hairColor.g = 255;
		this->core.hairColor.b = 255;

	}

	Customizer::~Customizer()
	{

	}

	CustomizerCore::~CustomizerCore()
	{

		if (this->frame)
		{

			SDL_DestroyTexture(this->frame);
			this->frame = NULL;

		}

		if (this->playerBase)
		{

			SDL_DestroyTexture(this->playerBase);
			this->playerBase = NULL;

		}

		if (this->koBase)
		{

			SDL_DestroyTexture(this->koBase);
			this->koBase = NULL;

		}

		if (this->bonds)
		{

			SDL_DestroyTexture(this->bonds);
			this->bonds = NULL;

		}

		if (this->bondsFore)
		{

			SDL_DestroyTexture(this->bondsFore);
			this->bondsFore = NULL;

		}

		if (this->playerOutput)
		{

			SDL_DestroyTexture(this->playerOutput);
			this->playerOutput = NULL;

		}

		if (this->koOutput)
		{

			SDL_DestroyTexture(this->koOutput);
			this->koOutput = NULL;

		}

		for (int i = 0; i < this->hairStyles.size(); i++)
		{

			if (this->hairStyles[i])
			{

				SDL_DestroyTexture(this->hairStyles[i]->texture);
				SDL_DestroyTexture(this->hairStyles[i]->koTexture);

				delete this->hairStyles[i];
				this->hairStyles[i] = NULL;

			}

		}

		for (int i = 0; i < this->costumes.size(); i++)
		{

			if (this->costumes[i])
			{

				SDL_DestroyTexture(this->costumes[i]->texture);
				SDL_DestroyTexture(this->costumes[i]->koTexture);

				delete this->costumes[i];
				this->costumes[i] = NULL;

			}

		}

	}

	void Customizer::Initialize(int argc, char* argv[])
	{

		this->oldName = SaveData::AccessSaveData()->Name();

	}

	void CustomizerCore::Load(std::string base, std::string options, std::string level)
	{

		this->playerBase = LoadTexture(base + ".png");
		this->koBase = LoadTexture(base + "KO.png");

		if (level.size() > 0)
		{

			this->bonds = LoadTexture(level + "BondsKO.png");
			this->bondsFore = LoadTexture(level + "BondsForeKO.png");

		}

		this->frame = LoadTexture("./Assets/Images/Frame.png");

		int w, h;
		SDL_QueryTexture(this->playerBase, NULL, NULL, &w, &h);
		this->playerOutput = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

		SDL_QueryTexture(this->koBase, NULL, NULL, &w, &h);
		this->koOutput = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

		std::ifstream file(options);

		std::string line;
		std::getline(file, line);

		std::istringstream input(line);

		int numNames;
		input >> numNames;

		for (int i = 0; i < numNames; i++)
		{

			std::getline(file, line);
			this->names.push_back(line);

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numHair;
		input >> numHair;

		for (int i = 0; i < numHair; i++)
		{

			CustomizationOption* newHair = new CustomizationOption();

			std::getline(file, line);
			newHair->name = line;

			std::getline(file, line);
			newHair->fileName = line;
			newHair->texture = LoadTexture(line + ".png");
			newHair->koTexture = LoadTexture(line + "KO.png");

			std::getline(file, line);
			input.clear();
			input.str(line);

			input >> newHair->reputation;

			this->hairStyles.push_back(newHair);

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numOutfit;
		input >> numOutfit;

		for (int i = 0; i < numOutfit; i++)
		{

			CustomizationOption* newOutfit = new CustomizationOption();

			std::getline(file, line);
			newOutfit->name = line;

			std::getline(file, line);
			newOutfit->fileName = line;
			newOutfit->texture = LoadTexture(line + ".png");
			newOutfit->koTexture = LoadTexture(line + "KO.png");

			std::getline(file, line);
			input.clear();
			input.str(line);

			input >> newOutfit->reputation;

			this->costumes.push_back(newOutfit);

		}

		int numProfiles;

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> numProfiles;

		int r, g, b;

		for (int i = 0; i < numProfiles; i++)
		{

			std::getline(file, line);
			input.clear();
			input.str(line);

			input >> r;
			input >> g;
			input >> b;

			SDL_Color col;
			col.r = r;
			col.g = g;
			col.b = b;

			this->hairProfiles.push_back(col);

		}

	}

	void Customizer::LoadContent(int argc, char* argv[])
	{

		if (argc >= 3)
		{

			this->core.Load(argv[0], argv[1]);

			this->playerSheet.LoadSpritesheet(argv[2]);
			this->playerSheet.SetSurface(this->core.playerOutput);

			this->MenuFactory("./Assets/Data/Menu/Customizer.plMNU", Settings::AccessSettings()->MusicVolume());

			SaveData* data = SaveData::AccessSaveData();

			if (this->randomize)
			{

				do
				{

					this->core.currentHair = RandomBoundedInteger(0, this->core.hairStyles.size() - 1);
					this->core.currentCostume = RandomBoundedInteger(0, this->core.costumes.size() - 1);


					this->inputs["RedSlider"]->SetValue(RandomBoundedInteger(0, 255));
					this->inputs["GreenSlider"]->SetValue(RandomBoundedInteger(0, 255));
					this->inputs["BlueSlider"]->SetValue(RandomBoundedInteger(0, 255));

					this->TextInput::text = this->core.names[RandomBoundedInteger(0, this->core.names.size() - 1)];

				}
				while (!this->core.Unlocked());

			}
			else
			{

				this->inputs["RedSlider"]->SetValue(data->HairColor().r);
				this->inputs["GreenSlider"]->SetValue(data->HairColor().g);
				this->inputs["BlueSlider"]->SetValue(data->HairColor().b);

				this->core.currentHair = data->HairIndex();
				this->core.currentCostume = data->CostumeIndex();

				this->TextInput::text = data->Name();

			}

			this->core.lock = this->textures[2]; // Will bite me later, but I'm too lazy to do this properly

		}

	}

	void Customizer::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		if (GetTime() - this->lastSwitch > ANIMATION_DURATION)
		{

			this->playerSheet.PlayAnimation((this->playerSheet.CurrentAnimation() + 1) % ANIMATION_MAX, true);
			this->lastSwitch = GetTime();

		}

		this->core.hairColor.r = this->inputs["RedSlider"]->Value();
		this->core.hairColor.g = this->inputs["GreenSlider"]->Value();
		this->core.hairColor.b = this->inputs["BlueSlider"]->Value();

		if (this->inputs["NameChange"]->Value())
		{

			this->textInputActive = !this->textInputActive;

			if (this->textInputActive)
			{

				engine.RegisterTextInput((TextInput*)this);

			}
			else
			{

				engine.StopTextInput();

			}

		}

		if (this->inputs["CostumeDown"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			if (this->core.currentCostume == 0)
			{

				this->core.currentCostume = this->core.costumes.size();

			}

			this->core.currentCostume = (this->core.currentCostume - 1) % this->core.costumes.size();

		}

		if (this->inputs["CostumeUp"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->core.currentCostume = (this->core.currentCostume + 1) % this->core.costumes.size();

		}

		if (this->inputs["HairDown"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			if (this->core.currentHair == 0)
			{

				this->core.currentHair = this->core.hairStyles.size();

			}

			this->core.currentHair = (this->core.currentHair - 1) % this->core.hairStyles.size();

		}

		if (this->inputs["HairUp"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->core.currentHair = (this->core.currentHair + 1) % this->core.hairStyles.size();

		}

		this->playerSheet.Update(deltaTime);

		this->core.PreparePlayer();

		UI::AccessUI()->RequestNeededReputation(fmaxf(this->core.costumes[this->core.currentCostume]->reputation, this->core.hairStyles[this->core.currentHair]->reputation));

		if (this->inputs["Stats"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			StatsMenu* stats = new StatsMenu();
			stats->Initialize(0, NULL);
			stats->LoadContent(0, NULL);
			stats->SetRunning(true);

			engine.RegisterState(stats);

		}

		if (this->inputs["Back"]->Value())
		{

			if (this->core.Unlocked() && ((this->TextInput::text == this->oldName) || (!SaveData::AccessSaveData()->Exists(this->TextInput::text))))
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->core.OutputPlayer(OUTPUT_PLAYER_FILE, OUTPUT_PLAYERKO_FILE);

				this->SetRunning(false);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (InputKeyPress(SDL_SCANCODE_RETURN))
		{

			this->textInputActive = false;
			engine.StopTextInput();

		}

	}

	void CustomizerCore::PreparePlayer()
	{

		SDL_Rect rect;
		rect.x = 0;
		rect.y = 0;
		SDL_QueryTexture(this->playerOutput, NULL, NULL, &rect.w, &rect.h);

		SDL_Color col;
		col.a = col.r = col.g = col.b = 0;

		SDL_SetRenderTarget(renderer, this->playerOutput);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(this->playerOutput, SDL_BLENDMODE_BLEND);
		ClearBuffer(col);

		SDL_RenderCopyEx(renderer, this->playerBase, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		if (this->currentHair >= 0 && this->currentHair < this->hairStyles.size())
		{

			SDL_SetTextureColorMod(this->hairStyles[this->currentHair]->texture, hairColor.r, hairColor.g, hairColor.b);

			SDL_RenderCopyEx(renderer, this->hairStyles[this->currentHair]->texture, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		}

		if (this->currentCostume >= 0 && this->currentCostume < this->costumes.size())
		{

			SDL_RenderCopyEx(renderer, this->costumes[this->currentCostume]->texture, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		}

		SDL_SetRenderTarget(renderer, NULL);

		rect.x = 0;
		rect.y = 0;
		SDL_QueryTexture(this->koOutput, NULL, NULL, &rect.w, &rect.h);

		SDL_SetRenderTarget(renderer, this->koOutput);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(this->koOutput, SDL_BLENDMODE_BLEND);
		ClearBuffer(col);

		if (this->bonds)
		{

			SDL_RenderCopyEx(renderer, this->bonds, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		}

		SDL_RenderCopyEx(renderer, this->koBase, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		if (this->currentHair >= 0 && this->currentHair < this->hairStyles.size())
		{

			SDL_SetTextureColorMod(this->hairStyles[this->currentHair]->koTexture, hairColor.r, hairColor.g, hairColor.b);

			SDL_RenderCopyEx(renderer, this->hairStyles[this->currentHair]->koTexture, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		}

		if (this->currentCostume >= 0 && this->currentCostume < this->costumes.size())
		{

			SDL_RenderCopyEx(renderer, this->costumes[this->currentCostume]->koTexture, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		}

		if (this->bondsFore)
		{

			SDL_RenderCopyEx(renderer, this->bondsFore, &rect, &rect, 0, NULL, SDL_FLIP_NONE);

		}

		SDL_SetRenderTarget(renderer, NULL);

	}

	void CustomizerCore::OutputPlayer(std::string output, std::string koOutput)
	{

		Uint32 format = 0;

		SDL_Rect rect;

		SDL_Surface* surface;

		if (output.size() > 0)
		{

			SDL_SetRenderTarget(renderer, this->playerOutput);

			rect.x = 0;
			rect.y = 0;
			SDL_QueryTexture(this->playerOutput, &format, NULL, &rect.w, &rect.h);

			surface = SDL_CreateRGBSurfaceWithFormat(NULL, rect.w, rect.h, 32, format);

			SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

			IMG_SavePNG(surface, output.c_str());

			SDL_FreeSurface(surface);

		}

		SDL_SetRenderTarget(renderer, this->koOutput);

		SDL_QueryTexture(this->koOutput, &format, NULL, &rect.w, &rect.h);

		surface = SDL_CreateRGBSurfaceWithFormat(NULL, rect.w, rect.h, 32, format);

		SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);

		IMG_SavePNG(surface, koOutput.c_str());

		SDL_FreeSurface(surface);

		SDL_SetRenderTarget(renderer, NULL);

	}

	void Customizer::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		SDL_Rect rect;
		rect.x = rect.y = PLATFORMER_TARGET_WIDTH / 8 - FRAME_OFFSET;
		SDL_QueryTexture(this->core.frame, NULL, NULL, &rect.w, &rect.h);
		rect.y -= rect.h / 2 + 3; //Ugly, yes :P

		AABB background(rect.x + FRAME_OFFSET, rect.y + FRAME_OFFSET, rect.w - 2 * FRAME_OFFSET, rect.h - 2 * FRAME_OFFSET);
		FillAABB(&background, Vector2(), this->background.r - 100, this->background.g - 100, this->background.b - 100, 255);

		SDL_RenderCopy(renderer, this->core.frame, NULL, &rect);

		this->playerSheet.Render(Vector2(PLATFORMER_TARGET_WIDTH / 8, PLATFORMER_TARGET_HEIGHT / 8), false);
		
		if (!this->core.Unlocked())
		{

			SDL_Rect src;
			src.x = LOCK_SRC_X;
			src.y = LOCK_SRC_Y;
			src.w = LOCK_SRC_W;
			src.h = LOCK_SRC_H;

			rect.x = rect.x + (rect.w / 2) - src.w / 2;
			rect.y = rect.y + (rect.h / 2) - src.h / 2;
			rect.w = src.w;
			rect.h = src.h;

			SDL_RenderCopy(renderer, this->core.lock, &src, &rect);

		}

		SDL_Color textColor;
		textColor.r = CUSTOMIZER_COLOR_R;
		textColor.g = CUSTOMIZER_COLOR_G;
		textColor.b = CUSTOMIZER_COLOR_B;
		textColor.a = CUSTOMIZER_COLOR_A;

		SDL_Texture* textSurface = CreateFontTexture(this->font, textColor, this->core.costumes[this->core.currentCostume]->name);

		rect.x = (this->inputs["CostumeUp"]->X() + this->inputs["CostumeDown"]->X() + this->inputs["CostumeDown"]->Width()) / 2;
		rect.y = this->inputs["CostumeUp"]->Y();
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);
		rect.x -= rect.w / 2;

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		textSurface = CreateFontTexture(this->font, textColor, this->core.hairStyles[this->core.currentHair]->name);

		rect.x = (this->inputs["HairUp"]->X() + this->inputs["HairDown"]->X() + this->inputs["HairDown"]->Width()) / 2;
		rect.y = this->inputs["HairUp"]->Y();
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);
		rect.x -= rect.w / 2;

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		std::string nameText = this->TextInput::text + (this->textInputActive ? "|" : "");
		textSurface = CreateFontTexture(this->font, textColor, nameText);
		rect.x = (this->inputs["NameChange"]->X() + this->inputs["NameChange"]->Width() + 8);
		rect.y = (this->inputs["NameChange"]->Y() + 3);
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		UI::AccessUI()->RenderReputation();

		UI::AccessUI()->RenderMouse();

	}

	void Customizer::Pause()
	{

	}

	void Customizer::Unpause()
	{

	}

	bool Customizer::UnpauseCondition()
	{

		return true;

	}

	void Customizer::Shutdown()
	{

		SaveData* data = SaveData::AccessSaveData();

		data->SetHairIndex(this->core.currentHair);
		data->SetCostumeIndex(this->core.currentCostume);
		data->SetHairColor(this->core.hairColor);

		data->SetName(this->TextInput::text);
		engine.StopTextInput();

		data->ExportSaveData();

	}
	bool CustomizerCore::Unlocked()
	{

		return (this->costumes[this->currentCostume]->reputation <= SaveData::AccessSaveData()->GetMaxReputation()) && (this->hairStyles[this->currentHair]->reputation <= SaveData::AccessSaveData()->GetMaxReputation());

	}


}