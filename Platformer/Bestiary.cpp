//Includes
#include <fstream>
#include <sstream>
#include <SDL_ttf.h>

//Custom Includes
#include "Bestiary.h"
#include "Settings.h"
#include "UI.h"
#include "SaveData.h"
#include "Helpers.h"
#include "Customizer.h"

namespace Platformer
{

	Bestiary::Bestiary()
	{

	}

	Bestiary::~Bestiary()
	{

		for (int i = 0; i < this->entries.size(); i++)
		{

			if (this->entries[i])
			{

				delete this->entries[i];
				this->entries[i] = NULL;

			}

		}

	}

	void Bestiary::Initialize(int argc, char* argv[])
	{

		this->currentEntry = 0;

	}

	void Bestiary::LoadContent(int argc, char* argv[])
	{

		if (argc >= 1)
		{

			std::ifstream file(argv[0]);

			std::string line;
			std::getline(file, line);
			std::istringstream input(line);

			int r, g, b;

			input >> r;
			input >> g;
			input >> b;

			this->textColor.r = r;
			this->textColor.g = g;
			this->textColor.b = b;

			std::getline(file, line);
			input.clear();
			input.str(line);

			int numEntries;

			input >> numEntries;

			for (int i = 0; i < numEntries; i++)
			{

				Entry* entry = new Entry();

				std::getline(file, entry->name);
				std::getline(file, entry->type);
				
				std::getline(file, line);
				input.clear();
				input.str(line);

				input >> entry->required;

				std::getline(file, entry->discovered);
				
				std::getline(file, line);
				input.clear();
				input.str(line);

				int numLines;

				input >> numLines;

				for (int j = 0; j < numLines; j++)
				{

					std::getline(file, line);
					entry->lines.push_back(line);

				}

				std::getline(file, line);
				entry->sheet.LoadSpritesheet(line);
				std::getline(file, line);
				entry->sheet.LoadSurface(line);

				entry->sheet.PlayAnimation(0, true);

				this->entries.push_back(entry);

			}

			this->MenuFactory("./Assets/Data/Menu/Bestiary.plMNU", Settings::AccessSettings()->MusicVolume());

			this->lock = this->textures[0]; //Yep, it bit me

		}

	}

	void Bestiary::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		Entry* entry = this->entries[this->currentEntry];
		entry->sheet.Update(deltaTime);

		if (GetTime() - this->lastSwitch > BESTIARY_ANIMATION_DURATION)
		{

			entry->sheet.PlayAnimation((entry->sheet.CurrentAnimation() + 1) % entry->sheet.NumAnimations(), true);
			this->lastSwitch = GetTime();

		}

		if (this->inputs["PageDown"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			if (this->currentEntry == 0)
			{

				this->currentEntry = this->entries.size();

			}

			this->currentEntry = (this->currentEntry - 1) % this->entries.size();

		}

		if (this->inputs["PageUp"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->currentEntry = (this->currentEntry + 1) % this->entries.size();

		}

		if (this->inputs["Back"]->Value())
		{

				UI::AccessUI()->PlayConfirmEffect();

				this->SetRunning(false);

		}

	}

	void Bestiary::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		Entry* entry = this->entries[this->currentEntry];

		if (entry->required <= SaveData::AccessSaveData()->InfoByType(entry->type))
		{

			this->RenderText(entry->name, Vector2(BESTIARY_NAME_X, BESTIARY_NAME_Y));
			this->RenderText(entry->discovered, Vector2(BESTIARY_DISCOVERED_X, BESTIARY_DISCOVERED_Y));

			for (int i = 0; i < entry->lines.size(); i++)
			{

				this->RenderText(entry->lines[i], Vector2(BESTIARY_DESC_X, BESTIARY_DESC_START_Y + i * BESTIARY_DESC_FONT_STEP));

			}

			entry->sheet.Render(Vector2(BESTIARY_DRAW_X, BESTIARY_DRAW_Y), SDL_FLIP_NONE);

		}
		else
		{

			this->RenderText("???", Vector2(BESTIARY_NAME_X, BESTIARY_NAME_Y));
			this->RenderText("???", Vector2(BESTIARY_DISCOVERED_X, BESTIARY_DISCOVERED_Y));

			SDL_Rect dest;
			dest.x = BESTIARY_DRAW_X;
			dest.y = BESTIARY_DRAW_Y;

			SDL_Rect src;
			src.x = LOCK_SRC_X;
			src.y = LOCK_SRC_Y;
			dest.w = src.w = LOCK_SRC_W;
			dest.h = src.h = LOCK_SRC_H;
			
			SDL_RenderCopy(renderer, this->lock, &src, &dest);


		}

		UI::AccessUI()->RenderMouse();

	}

	void Bestiary::RenderText(std::string text, Vector2 loc)
	{

		SDL_Texture* textSurface = CreateFontTexture(this->font, this->textColor, text);

		SDL_Rect rect;
		rect.x = loc.X();
		rect.y = loc.Y();
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);

		SDL_DestroyTexture(textSurface);

	}

	void Bestiary::Pause()
	{

	}

	void Bestiary::Unpause()
	{

	}

	bool Bestiary::UnpauseCondition()
	{

		return true;

	}

	void Bestiary::Shutdown()
	{

	}

}