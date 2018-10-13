//Includes
#include <SDL.h>
#include <iostream>

//Custom Includes
#include "Helpers.h"
#include "Engine.h"

namespace Engine2D
{

	void TextInput::Add(std::string s)
	{

		this->text += s;

	}

	void TextInput::Backspace()
	{

		if (this->text.size() > 0)
		{

			this->text.pop_back();

		}

	}

	void TextInput::Copy()
	{

		SDL_SetClipboardText(this->text.c_str());

	}

	void TextInput::Paste()
	{

		this->text = SDL_GetClipboardText();

	}

	Engine::Engine()
	{

		this->registered = NULL;

		this->mapper = NULL;
		this->textInput = NULL;

	}

	Engine::~Engine()
	{

		this->destroying = true;

		if (this->registered)
		{

			delete this->registered;
			this->registered = NULL;

		}

		while (this->states.size() > 0)
		{

			this->PopState();

		}

	}

	void Engine::EndMainLoop()
	{

		this->quit = true;
	}

	void Engine::RunMainLoop()
	{

		SDL_Event e;

		this->lastFrame = GetTime();

		this->quit = false;

		float deltaTime = 0;

		while (!this->quit)
		{

			double start = GetTime();

			while (SDL_PollEvent(&e) != 0)
			{

				if (e.type == SDL_WINDOWEVENT)
				{

					HandleWindowEvent(e.window.event);

				}
				else if (e.type == SDL_QUIT)
				{

					this->quit = true;

				}
				else if (e.type == SDL_MOUSEMOTION)
				{

					mouseX = e.motion.x;
					mouseY = e.motion.y;

				}
				else if (e.type == SDL_KEYDOWN)
				{

					if (this->mapper)
					{

						this->mapper->SetKey(e.key.keysym.scancode);
						this->mapper = NULL;

					}
					else if (this->textInput)
					{

						if(e.key.keysym.sym == SDLK_BACKSPACE)
						{
							
							this->textInput->Backspace();
						
						}
						else if(e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
						{ 
							
							this->textInput->Copy();
						
						}
						else if(e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL )
						{
							
							this->textInput->Paste();

						}

					}

				}
				else if (e.type == SDL_TEXTINPUT)
				{
					
					if (this->textInput)
					{

						if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
						{

							this->textInput->Add(e.text.text);

						}

					}

				}

			}

			if (GetTime() >= this->lastFrame + MAX_FRAME_DURATION)
			{

				deltaTime += GetTime() - this->lastFrame;
				this->lastFrame = GetTime();

				int numUpdates = 0;

				while (deltaTime >= TARGET_FRAME_DURATION)
				{

					if (this->states.size() > 0)
					{

						if (!this->states.back()->Running())
						{

							if (this->Transitioned())
							{

								this->PopState();

								if (this->states.size() <= 0)
								{

									break;

								}

								this->BeginScreenTransition(-1);

							}
							else
							{

								this->BeginScreenTransition(1);

							}

						}
						else
						{

							if (this->registered)
							{

								this->BeginScreenTransition(1);

								if (this->Transitioned())
								{

									this->PushState(this->registered);

									this->BeginScreenTransition(-1);
									this->registered = NULL;

								}

							}

						}

						if (this->paused && this->states.back()->UnpauseCondition())
						{

							this->Unpause();

						}

						if (!this->paused && this->states.back()->Running())
						{

							this->states.back()->Update(TARGET_FRAME_DURATION / 1000.0);

						}

						this->UpdateTransition(TARGET_FRAME_DURATION / 1000.0);

						deltaTime -= TARGET_FRAME_DURATION;

						numUpdates++;

						if (numUpdates >= NUM_PANIC_FRAMES)
						{

							deltaTime = 0;
							std::cout << "Panic!" << std::endl;

						}

					}

				}

				ClearBuffer();

				if (this->states.size() > 0)
				{

					this->states.back()->Render(deltaTime);

				}

				if (this->transitionAlpha > 0)
				{

					SDL_SetTextureColorMod(blankSurface, 0, 0, 0);
					SDL_SetTextureAlphaMod(blankSurface, (int)(this->transitionAlpha * 255));

					SDL_Rect dest = { 0, 0, PLATFORMER_TARGET_WIDTH, PLATFORMER_TARGET_HEIGHT };
					SDL_RenderCopyEx(renderer, blankSurface, NULL, &dest, 0, NULL, SDL_FLIP_NONE);

				}

				ShowBuffer();

				PumpKeyState();

			}

			if (this->states.size() == 0)
			{

				this->quit = true;

			}

		}

	}

	void Engine::BeginScreenTransition(int d)
	{

		this->transitionDirection = d;

	}

	void Engine::UpdateTransition(float deltaTime)
	{

		this->transitionAlpha += this->transitionDirection * SCREEN_TRANSITION_SPEED * deltaTime;
		this->transitionAlpha = fmaxf(0, fminf(1, this->transitionAlpha));

	}

	bool Engine::Transitioned()
	{

		return (this->transitionAlpha == 1 && this->transitionDirection == 1);

	}

	bool Engine::Darkened()
	{

		return this->transitionAlpha == 0;

	}

	void Engine::HandleWindowEvent(int code)
	{

		if (code == SDL_WINDOWEVENT_HIDDEN || code == SDL_WINDOWEVENT_MOVED || code == SDL_WINDOWEVENT_MINIMIZED || code == SDL_WINDOWEVENT_FOCUS_LOST)
		{

			if (!this->paused)
			{

				this->Pause();

			}

		}

	}

	void Engine::Pause()
	{

		for (int i = 0; i < this->states.size(); i++)
		{

			this->states[i]->Pause();

		}

		this->paused = true;

	}

	void Engine::Unpause()
	{

		for (int i = 0; i < this->states.size(); i++)
		{

			this->states[i]->Unpause();

		}

		this->lastFrame = GetTime();

		this->paused = false;

	}

	void Engine::PushState(GameState* state)
	{

		if (this->states.size() > 0)
		{

			this->states.back()->Pause();

		}

		this->states.push_back(state);
		this->states.back()->Start();

	}

	void Engine::RegisterState(GameState* state)
	{

		this->registered = state;

	}

	void Engine::RegisterMapper(KeyMapper* mapper)
	{

		this->mapper = mapper;

	}

	void Engine::RegisterTextInput(TextInput* text)
	{

		this->textInput = text;

	}

	void Engine::StopTextInput()
	{

		this->textInput = NULL;

	}

	void Engine::PopState()
	{

		if (!this->destroying)
		{

			this->states.back()->Shutdown();

		}

		delete this->states.back();

		this->states.pop_back();

		if (this->registered)
		{

			this->PushState(this->registered);
			this->registered = NULL;

		}
		else if (this->states.size() > 0)
		{

			this->states.back()->Unpause();
			this->states.back()->Start();

		}

		this->lastFrame = GetTime();

	}

	bool Engine::Paused()
	{

		return this->paused;

	}

}