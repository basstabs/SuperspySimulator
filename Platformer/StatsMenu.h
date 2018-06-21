#pragma once

//Includes

//Custom Includes
#include "GameState.h"

using namespace Engine2D;

#define AVAILABLE_POINTS_X 512
#define AVAILABLE_POINTS_Y 80
#define REQUIRED_POINTS_X 512
#define REQUIRED_POINTS_Y 112

#define BASE_HEALTH 5

namespace Platformer
{

	class StatsMenu : public MenuState
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

	//Data

	public:

	protected:

		int required = 0;

		int selectedX;
		int selectedY;

		SDL_Texture* selectedTexture;

	};

}