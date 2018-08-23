#pragma once

//Includes
#include <SDL.h>
#include <string>
#include <vector>

//Custom Includes
#include "Engine.h"
#include "GameState.h"
#include "Player.h"

using namespace Engine2D;

#define ANIMATION_DURATION 3000
#define ANIMATION_MAX 8

#define OUTPUT_PLAYER_FILE "./Assets/Images/HeroineBase.png"
#define OUTPUT_PLAYERKO_FILE "./Assets/Images/HeroineKOBase.png"

#define CUSTOMIZER_COLOR_R 25
#define CUSTOMIZER_COLOR_G 25
#define CUSTOMIZER_COLOR_B 25
#define CUSTOMIZER_COLOR_A 255

#define FRAME_OFFSET 5

#define LOCK_SRC_X 32
#define LOCK_SRC_Y 64
#define LOCK_SRC_W 32
#define LOCK_SRC_H 32

namespace Platformer
{

	struct CustomizationOption
	{

		SDL_Texture* texture = NULL;
		SDL_Texture* koTexture = NULL;

		std::string fileName;
		std::string name;

		int reputation;

	};

	struct CustomizerCore
	{

		~CustomizerCore();

		SDL_Texture* playerBase = NULL;
		SDL_Texture* koBase = NULL;
		SDL_Texture* playerOutput = NULL;
		SDL_Texture* koOutput = NULL;

		SDL_Texture* frame;
		SDL_Texture* lock;

		SDL_Texture* playerBonds = NULL;
		SDL_Texture* bonds = NULL;
		SDL_Texture* bondsFore = NULL;

		int currentHair;
		int currentCostume;

		SDL_Color hairColor;

		std::vector<CustomizationOption*> hairStyles;
		std::vector<CustomizationOption*> costumes;
		std::vector<SDL_Color> hairProfiles;
		std::vector<std::string> names;

		void Load(std::string base, std::string options, std::string level = "");

		void PreparePlayer(bool render = true);
		void OutputPlayer(std::string output, std::string koOutput);

		bool Unlocked();

	};

	class Customizer : public MenuState, public TextInput
	{

	//Methods

	public:

		Customizer(bool r = false);
		~Customizer();

		void Initialize(int argc, char* argv[]);
		void LoadContent(int argc, char* argv[]);

		void Update(float deltaTime);
		void Render(float deltaTime);

		void Pause();
		void Unpause();
		bool UnpauseCondition();

		void Shutdown();

	protected:


	//Data

	public:

	protected:

		Spritesheet playerSheet;
		Spritesheet koSheet;

		Spritesheet* currentSheet;

		CustomizerCore core;

		double lastSwitch = 0;

		bool randomize;
		bool textInputActive = false;

		std::string oldName;

	};

}