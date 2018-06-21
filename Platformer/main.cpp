//Includes
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <time.h>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "Engine.h"
#include "MainMenu.h"
#include "UI.h"
#include "SaveData.h"

int windowWidth = 0;
int windowHeight = 0;
bool fullScreen = false;

void LoadUserPreferences()
{

	Platformer::Settings::LoadSettings();

}

void LoadUI()
{

	Platformer::UI::AccessUI()->LoadUI("./Saves/UISettings.plUI");

}

int main(int argc, char * argv[])
{

	LoadUserPreferences();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{

		printf("Could not initialize SDL. Error: %s\n", SDL_GetError());

	}
	else
	{

		window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, (fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN));
		if (window == NULL)
		{

			printf("Could not create window. Error: %s\n", SDL_GetError());

		}
		else
		{

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer == NULL)
			{

				printf("Could not create renderer. Error: %s\n", SDL_GetError());
			
			}
			else
			{

				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{

					printf("Could not initialize SDL Image library. Error: %s\n", IMG_GetError());

				}
				else
				{

					TTF_Init();
					Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);

					SDL_RenderSetLogicalSize(renderer, PLATFORMER_TARGET_WIDTH, PLATFORMER_TARGET_HEIGHT);

					SDL_ShowCursor(SDL_DISABLE);

					PrepareBlank();

					LoadUI();

					std::ifstream lastSave("./Saves/LastSave.txt");
					std::string name;
					std::getline(lastSave, name);
					lastSave.close();

					Platformer::SaveData::LoadSaveData("./Saves/" + name + ".plSAV");

					Platformer::MainMenu* menu = new Platformer::MainMenu();
					menu->Initialize(0, NULL);
					menu->LoadContent(0, NULL);
					menu->SetRunning(true);

					engine.PushState(menu);

					SeedRandom(time(0));

					engine.RunMainLoop();

				}

			}

		}

	}

	Cleanup();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	Mix_Quit();
	SDL_Quit();

	return 0;

}