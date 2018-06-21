//Includes
#include <SDL_image.h>
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "CharacterSelect.h"
#include "Settings.h"
#include "SaveData.h"
#include "Customizer.h"

namespace Platformer
{

	void CharacterSelect::Initialize(int argc, char* argv[])
	{

		this->currentCharacter = 0;

		this->characters = SaveData::Characters();

		this->capturedColor.r = CUSTOMIZER_COLOR_R;
		this->capturedColor.g = CUSTOMIZER_COLOR_G;
		this->capturedColor.b = CUSTOMIZER_COLOR_B;
		this->capturedColor.a = CUSTOMIZER_COLOR_A;

	}

	void CharacterSelect::LoadContent(int argc, char* argv[])
	{

		this->MenuFactory("./Assets/Data/Menu/CharacterSelect.plMNU", Settings::AccessSettings()->MusicVolume());

		if (argc >= 3)
		{

			this->core.Load(argv[0], argv[1]);

			this->playerSheet.LoadSpritesheet(argv[2]);
			this->playerSheet.SetSurface(this->core.playerOutput);

			this->SelectCharacter();

		}

	}

	void CharacterSelect::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		if (GetTime() - this->lastSwitch > ANIMATION_DURATION)
		{

			this->playerSheet.PlayAnimation((this->playerSheet.CurrentAnimation() + 1) % ANIMATION_MAX, true);
			this->lastSwitch = GetTime();

		}

		if (this->inputs["Next"]->Value())
		{

			if (this->characters.size() > 1)
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->currentCharacter = (this->currentCharacter + 1) % this->characters.size();

				this->SelectCharacter();

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["Previous"]->Value())
		{

			if (this->characters.size() > 1)
			{

				UI::AccessUI()->PlayConfirmEffect();

				if (this->currentCharacter == 0)
				{

					this->currentCharacter = this->characters.size();

				}

				this->currentCharacter--;

				this->SelectCharacter();

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		this->playerSheet.Update(deltaTime);

		if (this->inputs["Back"]->Value())
		{

			if (SaveData::AccessSaveData()->DefeatedLevel().size() == 0)
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->SetRunning(false);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["New"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			SaveData::AccessSaveData()->Clear();

			char* files[3];
			files[0] = "./Assets/Images/Pieces/Base";
			files[1] = "./Saves/Customization.plCTM";
			files[2] = "./Assets/Data/Spritesheet/Player.plSPS";

			Customizer* cust = new Customizer(true);
			cust->Initialize(0, NULL);
			cust->LoadContent(3, files);
			cust->SetRunning(true);

			engine.RegisterState(cust);

			this->SetRunning(false);

		}

	}

	void CharacterSelect::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		this->playerSheet.Render(Vector2(CHARACTER_SELECT_RENDER_X, CHARACTER_SELECT_RENDER_Y), false);

		SDL_Color textColor;
		textColor.r = CUSTOMIZER_COLOR_R;
		textColor.g = CUSTOMIZER_COLOR_G;
		textColor.b = CUSTOMIZER_COLOR_B;
		textColor.a = CUSTOMIZER_COLOR_A;

		SDL_Texture* textSurface = CreateFontTexture(this->font, textColor, this->characters[this->currentCharacter]);
		
		SDL_Rect rect;
		rect.x = CHARACTER_SELECT_NAME_X;
		rect.y = CHARACTER_SELECT_NAME_Y;
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		textSurface = CreateFontTexture(this->font, this->capturedColor, this->defeatedText);

		rect.x = CHARACTER_SELECT_STATUS_X;
		rect.y = CHARACTER_SELECT_STATUS_Y;
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		UI::AccessUI()->RenderReputation();

		UI::AccessUI()->RenderMouse();

	}

	void CharacterSelect::Pause()
	{

	}

	void CharacterSelect::Unpause()
	{

	}

	bool CharacterSelect::UnpauseCondition()
	{

		return true;

	}

	void CharacterSelect::Shutdown()
	{

		this->core.OutputPlayer(OUTPUT_PLAYER_FILE, OUTPUT_PLAYERKO_FILE);

	}

	void CharacterSelect::SelectCharacter()
	{
		
		SaveData* data = SaveData::AccessSaveData();
		data->LoadSaveData(SAVE_DIRECTORY + this->characters[this->currentCharacter] + ".plSAV");
		
		this->core.currentHair = data->HairIndex();
		this->core.currentCostume = data->CostumeIndex();
		this->core.hairColor = data->HairColor();

		this->core.PreparePlayer();

		this->defeatedText = SaveData::AccessSaveData()->DefeatedLevel();
		if (this->defeatedText.size() == 0)
		{

			this->defeatedText = "Free!";
			this->capturedColor.g = 225;
			this->capturedColor.r = 25;

		}
		else
		{

			this->defeatedText = "Captured - " + this->defeatedText;
			this->capturedColor.r = 225;
			this->capturedColor.g = 25;

		}

	}

}
