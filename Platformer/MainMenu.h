#pragma once

//Includes

//Custom Includes
#include "Engine.h"
#include "GameState.h"

using namespace Engine2D;

#define BONUS_X 64
#define BONUS_Y 224

namespace Platformer
{

	class MainMenu : public MenuState
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

	};

}
