#pragma once

//Includes

//Custom Includes
#include "GameState.h"

using namespace Engine2D;

namespace Platformer
{

	class SettingsMenu : public MenuState
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