//Includes
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "SettingsMenu.h"
#include "InputMapper.h"
#include "UI.h"

namespace Platformer
{

	void SettingsMenu::Initialize(int argc, char* argv[])
	{

	}

	void SettingsMenu::LoadContent(int argc, char* argv[])
	{

		this->MenuFactory("./Assets/Data/Menu/Settings.plMNU", Settings::AccessSettings()->MusicVolume());

		this->inputs["SoundSlider"]->SetValue(Settings::AccessSettings()->SoundVolume() * 2);
		this->inputs["MusicSlider"]->SetValue(Settings::AccessSettings()->MusicVolume() * 2);

	}

	void SettingsMenu::Update(float deltaTime)
	{

		static int lastSound = this->inputs["SoundSlider"]->Value();
		static int lastMusic = this->inputs["MusicSlider"]->Value();

		this->MenuState::Update(deltaTime);

		if (this->inputs["Controls"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			InputMapper* mapper = new InputMapper();
			mapper->Initialize(0, NULL);
			mapper->LoadContent(0, NULL);
			mapper->SetRunning(true);

			engine.RegisterState(mapper);

		}

		Settings::AccessSettings()->SetSoundVolume(this->inputs["SoundSlider"]->Value() / 2);
		Settings::AccessSettings()->SetMusicVolume(this->inputs["MusicSlider"]->Value() / 2);
		UI::AccessUI()->UpdateVolumes();

		if (lastSound != this->inputs["SoundSlider"]->Value())
		{

			UI::AccessUI()->PlayItemEffect();

		}

		lastSound = this->inputs["SoundSlider"]->Value();
		lastMusic = this->inputs["MusicSlider"]->Value();

		if (this->inputs["Fullscreen"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			Settings::AccessSettings()->ChangeFullscreen(true);
			Settings::AccessSettings()->ChangeWindowSize(1280, 720);

		}

		if (this->inputs["640x360"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			Settings::AccessSettings()->ChangeFullscreen(false);
			Settings::AccessSettings()->ChangeWindowSize(640, 360);

		}

		if (this->inputs["1280x720"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			Settings::AccessSettings()->ChangeFullscreen(false);
			Settings::AccessSettings()->ChangeWindowSize(1280, 720);

		}

		if (this->inputs["1920x1080"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			Settings::AccessSettings()->ChangeFullscreen(false);
			Settings::AccessSettings()->ChangeWindowSize(1920, 1080);

		}

		if (this->inputs["Back"]->Value())
		{

			UI::AccessUI()->PlayConfirmEffect();

			this->SetRunning(false);

		}

	}

	void SettingsMenu::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		UI::AccessUI()->RenderReputation();

		UI::AccessUI()->RenderMouse();

	}

	void SettingsMenu::Pause()
	{

	}

	void SettingsMenu::Unpause()
	{

	}

	bool SettingsMenu::UnpauseCondition()
	{

		return true;

	}

	void SettingsMenu::Shutdown()
	{

		Settings::AccessSettings()->ExportSettings();

	}

}