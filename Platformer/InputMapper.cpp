//Includes

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "InputMapper.h"
#include "UI.h"

std::string inputButtons[] = {"UpMap", "DownMap", "LeftMap", "RightMap", "SelectMap", "StartMap", "JumpMap", "ShootMap"};

namespace Platformer
{

	void InputMapper::Initialize(int argc, char* argv[])
	{

		this->keyColor.r = this->keyColor.g = this->keyColor.b = 25;

		this->rebindKey = -1;

	}

	void InputMapper::LoadContent(int argc, char* argv[])
	{

		this->MenuFactory("./Assets/Data/Menu/InputMapper.plMNU", Settings::AccessSettings()->MusicVolume());

	}

	void InputMapper::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		if (this->rebindKey < 0)
		{

			for (int i = 0; i < NUM_INPUTS; i++)
			{

				if (this->inputs[inputButtons[i]]->Value())
				{

					UI::AccessUI()->PlayConfirmEffect();

					this->rebindKey = i;

					engine.RegisterMapper(this);

				}

			}

		}

		if (this->inputs["BackButton"]->Value())
		{

			if (this->rebindKey < 0)
			{

				UI::AccessUI()->PlayConfirmEffect();

				this->SetRunning(false);

			}
			else
			{

				UI::AccessUI()->PlayCancelEffect();

			}

		}

	}

	void InputMapper::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		ControlPreference* controls = Settings::AccessSettings()->Controls();
		for (int i = 0; i < NUM_INPUTS; i++)
		{

			if (i != this->rebindKey)
			{

				Vector2 center = this->inputs[inputButtons[i]]->Center();
				std::string name = SDL_GetScancodeName(controls->keys[i]);

				SDL_Texture* textSurface = CreateFontTexture(this->font, this->keyColor, name);

				SDL_Rect rect;
				rect.x = center.X() + KEY_X_OFFSET;
				rect.y = center.Y() + KEY_Y_OFFSET;
				SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);
				rect.y -= rect.h / 2;

				SDL_RenderCopy(renderer, textSurface, NULL, &rect);

				SDL_DestroyTexture(textSurface);

			}

		}

		UI::AccessUI()->RenderReputation();

		UI::AccessUI()->RenderMouse();

	}

	void InputMapper::Pause()
	{

	}

	void InputMapper::Unpause()
	{

	}

	bool InputMapper::UnpauseCondition()
	{
	
		return true;

	}

	void InputMapper::Shutdown()
	{

		Settings::AccessSettings()->ExportSettings();

	}

	void InputMapper::SetKey(SDL_Scancode key)
	{

		if (this->rebindKey >= 0 && this->rebindKey < NUM_INPUTS)
		{

			UI::AccessUI()->PlayConfirmEffect();

			ControlPreference* controls = Settings::AccessSettings()->Controls();
			controls->keys[this->rebindKey] = key;

			this->rebindKey = -1;

		}

	}

}