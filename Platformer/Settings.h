#pragma once

//Includes
#include <SDL.h>
#include <map>

//Custom Includes
#include "InputMapper.h"

#define NUM_INPUTS 8

#define SETTINGS_FILE "./Saves/Preferences.ini"

#define DELIMITER "="

#define WIDTH_CODE "width"
#define HEIGHT_CODE "height"
#define FULL_CODE "fullscreen"
#define SOUND_CODE "sound volume"
#define MUSIC_CODE "music volume"
#define DEBUG_CODE "debug"
#define KEY_CODE "key"
#define HARD_CODE "hard"

#define UP_CODE 0
#define DOWN_CODE 1
#define RIGHT_CODE 2
#define LEFT_CODE 3
#define SELECT_CODE 4
#define START_CODE 5
#define JUMP_CODE 6
#define ACTION_CODE 7

#define REPUTATION_CODE "reputation"
#define REMOVE_CODE "remove"

void ChangeWindow();

using namespace Engine2D;

namespace Platformer
{

	struct ControlPreference
	{
	
		friend class InputMapper;
		friend class Settings;

	private:

		SDL_Scancode keys[NUM_INPUTS] = { SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_A, SDL_SCANCODE_TAB, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_K, SDL_SCANCODE_J };

	public:

		bool Down(int inputCode);
		bool Pressed(int inputCode);
		bool Lifted(int inputCode);

	};

	class Settings
	{

	//Methods

	public:

		static Settings* AccessSettings();
		static void LoadSettings();

		ControlPreference* Controls();

		void SetDebug(bool d);
		bool Debug();

		void SetSoundVolume(int s);
		void SetMusicVolume(int m);
		int SoundVolume();
		int MusicVolume();

		std::string InputText(int index);

		int ReputationByType(std::string t);
		bool RemoveByType(std::string t);

		void ExportSettings();

		void ChangeWindowSize(int w, int h);
		void ChangeFullscreen(bool f);

		bool HardMode();
		void SetHardMode(bool h);

	protected:

		Settings();

	//Data

	public:

	protected:

		std::map<std::string, int> reputationByType;
		std::map<std::string, bool> removeByType;

		static Settings* singleton;

		ControlPreference controls;

		bool debug;

		int soundVolume;
		int musicVolume;

		bool hardMode = false;

	};

}