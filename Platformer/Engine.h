#pragma once

//Libraries
#include <vector>

//Custom Includes
#include "GameState.h"

#define NUM_PANIC_FRAMES 10
#define TARGET_FRAME_DURATION (1000.0 / 60.0)
#define MAX_FRAME_DURATION (1000.0 / 60.0)

#define SCREEN_TRANSITION_SPEED 1

namespace Engine2D
{

	class KeyMapper
	{

	//Methods

	public:

		virtual void SetKey(SDL_Scancode code) = 0;

	protected:

	//Data

	public:

	protected:

	};

	class TextInput
	{

	//Methods

	public:

		void Add(std::string s);

		void Backspace();
		void Copy();
		void Paste();

	protected:

	//Data

	public:

	protected:

		std::string text;

	};

	class Engine
	{

	//Methods

	public:

		Engine();
		~Engine();

		void RunMainLoop();
		void EndMainLoop();

		void HandleWindowEvent(int code);

		void Pause();
		void Unpause();

		void PushState(GameState* state);
		void RegisterState(GameState* state);
		void RegisterMapper(KeyMapper* mapper);
		void RegisterTextInput(TextInput* text);
		void PopState();
		void StopTextInput();

		void Engine::BeginScreenTransition(int d);
		void Engine::UpdateTransition(float deltaTime);

		bool Transitioned();
		bool Darkened();

		bool Paused();

	protected:

	//Data

	public:

	protected:

		KeyMapper* mapper;
		TextInput* textInput;

		std::vector<GameState*> states;
		GameState* registered;

		double lastFrame;

		bool paused;
		bool quit = false;
		bool destroying = false;

		float transitionAlpha = 0;
		int transitionDirection = 0;

	};

}