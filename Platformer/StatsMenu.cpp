//Includes
#include <sstream>

//Custom Includes
#include "StatsMenu.h"
#include "Settings.h"
#include "UI.h"
#include "SaveData.h"
#include "Helpers.h"

namespace Platformer
{

	void StatsMenu::Initialize(int argc, char* argv[])
	{

	}

	void StatsMenu::LoadContent(int argc, char* argv[])
	{

		this->MenuFactory("./Assets/Data/Menu/Stats.plMNU", Settings::AccessSettings()->MusicVolume());

		MenuInput* input = this->inputs[SaveData::AccessSaveData()->CurrentWeapon()];
		this->selectedX = input->X();
		this->selectedY = input->Y();

		this->selectedTexture = this->textures[6]; //Screw it, this whole class is a hardcoded, lazy, disgusting mess, so why break the trend :P

	}

	void StatsMenu::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		this->required = 0;

		if (this->inputs["Pistol"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->selectedX = this->inputs["Pistol"]->X();
			this->selectedY = this->inputs["Pistol"]->Y();

			SaveData::AccessSaveData()->SetCurrentWeapon("");

		}

		if (this->inputs["Handcannon"]->Contains(Vector2(mouseX, mouseY)))
		{

			this->required = SaveData::AccessSaveData()->WeaponUnlockCost("Handcannon");

		}

		if (this->inputs["Handcannon"]->Value())
		{

			if (SaveData::AccessSaveData()->AvailablePoints() >= this->required)
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->selectedX = this->inputs["Handcannon"]->X();
				this->selectedY = this->inputs["Handcannon"]->Y();

				SaveData::AccessSaveData()->SetCurrentWeapon("Handcannon");

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["Phaser"]->Contains(Vector2(mouseX, mouseY)))
		{

			this->required = SaveData::AccessSaveData()->WeaponUnlockCost("Phaser");

		}

		if (this->inputs["Phaser"]->Value())
		{

			if (SaveData::AccessSaveData()->AvailablePoints() >= this->required)
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->selectedX = this->inputs["Phaser"]->X();
				this->selectedY = this->inputs["Phaser"]->Y();

				SaveData::AccessSaveData()->SetCurrentWeapon("Phaser");

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["Uzi"]->Contains(Vector2(mouseX, mouseY)))
		{

			this->required = SaveData::AccessSaveData()->WeaponUnlockCost("Uzi");

		}

		if (this->inputs["Uzi"]->Value())
		{

			if (SaveData::AccessSaveData()->AvailablePoints() >= this->required)
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->selectedX = this->inputs["Uzi"]->X();
				this->selectedY = this->inputs["Uzi"]->Y();

				SaveData::AccessSaveData()->SetCurrentWeapon("Uzi");

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["Magnum"]->Contains(Vector2(mouseX, mouseY)))
		{

			this->required = SaveData::AccessSaveData()->WeaponUnlockCost("Magnum");

		}

		if (this->inputs["Magnum"]->Value())
		{

			if (SaveData::AccessSaveData()->AvailablePoints() >= this->required)
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->selectedX = this->inputs["Magnum"]->X();
				this->selectedY = this->inputs["Magnum"]->Y();

				SaveData::AccessSaveData()->SetCurrentWeapon("Magnum");

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["HealthDown"]->Value())
		{

			if (SaveData::AccessSaveData()->HealthBonus() > 0)
			{

				UI::AccessUI()->PlayConfirmEffect();

				SaveData::AccessSaveData()->AddHealthBonus(-1);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["InvulnDown"]->Value())
		{

			if (SaveData::AccessSaveData()->InvulnBonus() > 0)
			{

				UI::AccessUI()->PlayConfirmEffect();

				SaveData::AccessSaveData()->AddInvulnBonus(-1);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["EscapeDown"]->Value())
		{

			if (SaveData::AccessSaveData()->EscapeBonus() > 0)
			{

				UI::AccessUI()->PlayConfirmEffect();

				SaveData::AccessSaveData()->AddEscapeBonus(-1);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["HealthUp"]->Contains(Vector2(mouseX, mouseY)))
		{

			this->required = SaveData::AccessSaveData()->HealthBonus() + 1;

		}

		if (this->inputs["InvulnUp"]->Contains(Vector2(mouseX, mouseY)))
		{

			this->required = SaveData::AccessSaveData()->InvulnBonus() + 1;

		}

		if (this->inputs["EscapeUp"]->Contains(Vector2(mouseX, mouseY)))
		{

			this->required = SaveData::AccessSaveData()->EscapeBonus() + 1;

		}

		if (this->inputs["HealthUp"]->Value())
		{

			if (SaveData::AccessSaveData()->AvailablePoints() >= this->required)
			{

				UI::AccessUI()->PlayConfirmEffect();

				SaveData::AccessSaveData()->AddHealthBonus(1);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["InvulnUp"]->Value())
		{

			if (SaveData::AccessSaveData()->AvailablePoints() >= this->required)
			{

				UI::AccessUI()->PlayConfirmEffect();

				SaveData::AccessSaveData()->AddInvulnBonus(1);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["EscapeUp"]->Value())
		{

			if (SaveData::AccessSaveData()->AvailablePoints() >= this->required)
			{

				UI::AccessUI()->PlayConfirmEffect();

				SaveData::AccessSaveData()->AddEscapeBonus(1);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

		if (this->inputs["Back"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->SetRunning(false);

		}

	}

	void StatsMenu::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		SDL_Rect rect;
		SDL_Color textColor;
		textColor.r = textColor.g = textColor.b = 25;

		rect.x = this->selectedX;
		rect.y = this->selectedY;
		rect.w = rect.h = 32;
		SDL_RenderCopy(renderer, this->selectedTexture, NULL, &rect);
		
		std::ostringstream parse;
		parse << SaveData::AccessSaveData()->HealthBonus() + BASE_HEALTH;

		SDL_Texture* textSurface = CreateFontTexture(this->font, textColor, parse.str());

		rect.x = (this->inputs["HealthUp"]->X() + this->inputs["HealthDown"]->X() + this->inputs["HealthDown"]->Width()) / 2;
		rect.y = this->inputs["HealthUp"]->Y();
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);
		rect.x -= rect.w / 2;

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		parse.clear();
		parse.str("");
		parse << SaveData::AccessSaveData()->InvulnBonus();

		textSurface = CreateFontTexture(this->font, textColor, parse.str());

		rect.x = (this->inputs["InvulnUp"]->X() + this->inputs["InvulnDown"]->X() + this->inputs["InvulnDown"]->Width()) / 2;
		rect.y = this->inputs["InvulnUp"]->Y();
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);
		rect.x -= rect.w / 2;

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		parse.clear();
		parse.str("");
		parse << SaveData::AccessSaveData()->EscapeBonus();

		textSurface = CreateFontTexture(this->font, textColor, parse.str());

		rect.x = (this->inputs["EscapeUp"]->X() + this->inputs["EscapeDown"]->X() + this->inputs["EscapeDown"]->Width()) / 2;
		rect.y = this->inputs["EscapeUp"]->Y();
		SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);
		rect.x -= rect.w / 2;

		SDL_RenderCopy(renderer, textSurface, NULL, &rect);
		SDL_DestroyTexture(textSurface);

		parse.clear();
		parse.str("");
		parse << SaveData::AccessSaveData()->AvailablePoints();

		UI::AccessUI()->RenderText(Vector2(AVAILABLE_POINTS_X, AVAILABLE_POINTS_Y), "Available: " + parse.str(), textColor, false);

		parse.clear();
		parse.str("");

		if (this->required > 0)
		{

			parse << this->required;

			if (this->required > SaveData::AccessSaveData()->AvailablePoints())
			{

				textColor.g = 25;
				textColor.r = 225;

			}
			else
			{

				textColor.r = 25;
				textColor.g = 225;

			}

		}

		UI::AccessUI()->RenderText(Vector2(REQUIRED_POINTS_X, REQUIRED_POINTS_Y), "Required: " + parse.str(), textColor, false);

		UI::AccessUI()->RenderReputation();

		UI::AccessUI()->RenderMouse();

	}

	void StatsMenu::Pause()
	{

	}

	void StatsMenu::Unpause()
	{

	}

	bool StatsMenu::UnpauseCondition()
	{

		return true;

	}

	void StatsMenu::Shutdown()
	{

		SaveData::AccessSaveData()->ExportSaveData();

	}

}