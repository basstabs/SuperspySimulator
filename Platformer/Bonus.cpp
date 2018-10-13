//Includes
#include <SDL_image.h>

//Custom Includes
#include "Helpers.h"
#include "Bonus.h"
#include "Settings.h"
#include "UI.h"

namespace Platformer
{

	void Bonus::Initialize(int argc, char* argv[])
	{

		this->current = 0;

	}

	void Bonus::LoadContent(int argc, char* argv[])
	{

		this->MenuFactory("./Assets/Data/Menu/Bonus.plMNU", Settings::AccessSettings()->MusicVolume());

		if (argc > 0)
		{

			this->frame = LoadTexture(argv[0]);

		}

	}

	void Bonus::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		if (this->inputs["BonusUp"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->current = (this->current + 1) % (this->textures.size() - 1);

		}

		if (this->inputs["BonusDown"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			if (this->current == 0)
			{

				this->current = this->textures.size() - 1;

			}

			this->current--;

		}

		if (this->inputs["Back"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->SetRunning(false);

		}

	}

	void Bonus::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		UI::AccessUI()->RenderReputation();

		SDL_Rect rect;
		SDL_QueryTexture(this->textures[this->current + 1], NULL, NULL, &rect.w, &rect.h);
		rect.x = PLATFORMER_TARGET_WIDTH / 2 - rect.w / 2;
		rect.y = PLATFORMER_TARGET_HEIGHT / 2 - rect.h / 2;

		SDL_RenderCopy(renderer, this->textures[this->current + 1], NULL, &rect);

		SDL_QueryTexture(this->frame, NULL, NULL, &rect.w, &rect.h);
		rect.x = PLATFORMER_TARGET_WIDTH / 2 - rect.w / 2;
		rect.y = PLATFORMER_TARGET_HEIGHT / 2 - rect.h / 2;

		SDL_RenderCopy(renderer, this->frame, NULL, &rect);

		UI::AccessUI()->RenderMouse();

	}

	void Bonus::Pause()
	{

	}

	void Bonus::Unpause()
	{

	}

	bool Bonus::UnpauseCondition()
	{

		return true;

	}

	void Bonus::Shutdown()
	{

		if (this->frame)
		{

			SDL_DestroyTexture(this->frame);
			this->frame = NULL;

		}

	}

}
