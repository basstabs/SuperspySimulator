#pragma once

//Includes

//Custom Includes
#include "Engine.h"
#include "GameState.h"
#include "Customizer.h"

#define CHARACTER_SELECT_RENDER_X 300
#define CHARACTER_SELECT_RENDER_Y 135
#define CHARACTER_SELECT_NAME_X 320
#define CHARACTER_SELECT_NAME_Y 225
#define CHARACTER_SELECT_STATUS_X 320
#define CHARACTER_SELECT_STATUS_Y 257

using namespace Engine2D;

namespace Platformer
{

	class CharacterSelect : public MenuState
	{

	//Methods

	public:

		void Initialize(int argc, char* argv[]);
		void LoadContent(int argc, char* argv[]);

		void Update(float deltaTime);
		void Render(float deltaTime);

		void Pause();
		void Unpause();
		bool UnpauseCondition();

		void Shutdown();

	protected:

		void SelectCharacter();

	//Data

	public:

	protected:

		Spritesheet playerSheet;
		double lastSwitch = 0;

		std::vector<std::string> characters;
		int currentCharacter;

		CustomizerCore core;

		SDL_Color capturedColor;
		std::string defeatedText;

	};

}

