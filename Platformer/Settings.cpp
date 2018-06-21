//Includes
#include <fstream>
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"

void ChangeWindow()
{

	SDL_SetWindowSize(window, windowWidth, windowHeight);
	SDL_SetWindowFullscreen(window, (fullScreen ? SDL_WINDOW_FULLSCREEN : 0));
	
	SDL_RenderSetLogicalSize(renderer, PLATFORMER_TARGET_WIDTH, PLATFORMER_TARGET_HEIGHT);

	SDL_DestroyTexture(blankSurface);
	Engine2D::PrepareBlank();

}

namespace Platformer
{

	bool ControlPreference::Down(int inputCode)
	{

		return InputKeyDown(this->keys[inputCode]);
	}
	
	bool ControlPreference::Pressed(int inputCode)
	{

		return InputKeyPress(this->keys[inputCode]);

	}

	bool ControlPreference::Lifted(int inputCode)
	{

		return InputKeyLift(this->keys[inputCode]);

	}

	Settings* Settings::singleton = NULL;

	Settings::Settings()
	{

		this->reputationByType["Dossier"] = 250;
		this->reputationByType["Damsel"] = 500;
		this->reputationByType["AlienInfo"] = 100;

		this->removeByType["Dossier"] = true;
		this->removeByType["Damsel"] = false;
		this->removeByType["AlienInfo"] = true;

	}

	Settings* Settings::AccessSettings()
	{

		if (singleton)
		{

			return singleton;

		}

		singleton = new Settings();
		return singleton;

	}

	void Settings::LoadSettings()
	{

		Settings* settings = Settings::AccessSettings();

		std::ifstream pref("./Saves/Preferences.ini");

		std::string line;
		while (std::getline(pref, line))
		{

			std::string type = line.substr(0, line.find_first_of('='));
			std::string value = line.substr(line.find_first_of('=') + 1, std::string::npos);

			std::stringstream input(value);

			if (type == WIDTH_CODE)
			{

				input >> windowWidth;

			}
			else if (type == HEIGHT_CODE)
			{

				input >> windowHeight;

			}
			else if (type == FULL_CODE)
			{

				input >> fullScreen;

			}
			else if (type == DEBUG_CODE)
			{

				bool d;

				input >> d;
				settings->SetDebug(d);

			}
			else if (type == SOUND_CODE)
			{

				int s;

				input >> s;
				s = fminf(fmaxf(s, 0), MIX_MAX_VOLUME);

				settings->SetSoundVolume(s);

			}
			else if (type == MUSIC_CODE)
			{

				int m;

				input >> m;
				m = fminf(fmaxf(m, 0), MIX_MAX_VOLUME);

				settings->SetMusicVolume(m);

			}
			else if (type == KEY_CODE)
			{

				int key;
				int index;

				input >> index;
				input >> key;

				settings->Controls()->keys[index] = (SDL_Scancode)key;

			}
			else if (type == REPUTATION_CODE)
			{

				std::string type;
				int reputation;

				input >> type;
				input >> reputation;

				settings->reputationByType[type] = reputation;

			}
			else if (type == REMOVE_CODE)
			{

				std::string type;
				bool remove;

				input >> type;
				input >> remove;

				settings->removeByType[type] = remove;

			}

		}

		pref.close();

	}

	ControlPreference* Settings::Controls()
	{

		return &(this->controls);

	}

	void Settings::SetDebug(bool d)
	{

		this->debug = d;

	}

	bool Settings::Debug()
	{

		return this->debug;

	}

	void Settings::SetSoundVolume(int s)
	{

		this->soundVolume = s;

	}

	void Settings::SetMusicVolume(int m)
	{

		this->musicVolume = m;

	}

	int Settings::SoundVolume()
	{

		return this->soundVolume;

	}

	int Settings::MusicVolume()
	{

		return this->musicVolume;

	}

	void Settings::ChangeWindowSize(int w, int h)
	{

		windowWidth = w;
		windowHeight = h;

		ChangeWindow();

	}

	void Settings::ChangeFullscreen(bool f)
	{

		fullScreen = f;

		ChangeWindow();

	}

	int Settings::ReputationByType(std::string t)
	{

		return this->reputationByType[t];

	}

	bool Settings::RemoveByType(std::string t)
	{

		return this->removeByType[t];

	}

	void Settings::ExportSettings()
	{

		std::ofstream file(SETTINGS_FILE, std::ios::trunc);

		file << WIDTH_CODE << DELIMITER << windowWidth << std::endl;
		file << HEIGHT_CODE << DELIMITER << windowHeight << std::endl;
		file << FULL_CODE << DELIMITER << fullScreen << std::endl;
		file << SOUND_CODE << DELIMITER << this->soundVolume << std::endl;
		file << MUSIC_CODE << DELIMITER << this->musicVolume << std::endl;
		file << DEBUG_CODE << DELIMITER << this->debug << std::endl;

		file << std::endl;

		for (int i = 0; i < NUM_INPUTS; i++)
		{

			file << KEY_CODE << DELIMITER << i << " " << this->Controls()->keys[i] << std::endl;

		}

		file << std::endl;

		std::map<std::string, int>::iterator it;
		for (it = this->reputationByType.begin(); it != this->reputationByType.end(); it++)
		{

			file << REPUTATION_CODE << DELIMITER << it->first << " " << it->second << std::endl;

		}

		file << std::endl;

		std::map<std::string, bool>::iterator iter;
		for (iter = this->removeByType.begin(); iter != this->removeByType.end(); iter++)
		{

			file << REMOVE_CODE << DELIMITER << iter->first << " " << iter->second << std::endl;

		}

		file.close();

	}

}