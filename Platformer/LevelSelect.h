#pragma once

//Includes

//Custom Includes
#include "Engine.h"
#include "SaveData.h"
#include "GameState.h"

using namespace Engine2D;

#define SELECT_COLOR_R 25
#define SELECT_COLOR_G 25
#define SELECT_COLOR_B 25
#define SELECT_COLOR_A 255

namespace Platformer
{

	class LevelSelect : public MenuState
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

		void CreateDamsels(int n);
		void GeneratePlayerSprites();

	//Data

	public:

	protected:

		bool start = false;

		std::vector<LevelData> levels;
		std::vector<SDL_Texture*> levelSurfaces;

		SDL_Texture* frame;

		int currentLevel;

	};

}
