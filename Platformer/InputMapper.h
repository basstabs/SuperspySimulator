#pragma once

//Includes

//Custom Includes
#include "Engine.h"
#include "GameState.h"

using namespace Engine2D;

#define KEY_X_OFFSET 64
#define KEY_Y_OFFSET 0

#define BLINK_DURATION 2000

namespace Platformer
{

	class InputMapper : public MenuState, KeyMapper
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

		void SetKey(SDL_Scancode key);

	protected:

	//Data

	public:

	protected:

		SDL_Color keyColor;
		int rebindKey;

	};

}