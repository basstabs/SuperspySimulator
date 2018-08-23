//Includes
#include <SDL_image.h>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"

int numKeys;
const Uint8* currentKeyStates = SDL_GetKeyboardState(&numKeys);
const Uint8* previousKeyStates = new Uint8[numKeys];

int mouseX;
int mouseY;
Uint32 mouseButtons = 0;
Uint32 previousMouseButtons = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

SDL_Texture* blankSurface = NULL;

Engine2D::Engine engine;

namespace Engine2D
{

	std::vector<SoundHolder> soundQueue;

	void SeedRandom(int seed)
	{

		srand(seed);

	}

	int RandomBoundedInteger(int min, int max)
	{

		return rand() % (max - min + 1) + min;

	}

	void PrepareBlank()
	{

		blankSurface = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, PLATFORMER_TARGET_WIDTH, PLATFORMER_TARGET_HEIGHT);

		SDL_SetTextureBlendMode(blankSurface, SDL_BLENDMODE_BLEND);

		SDL_SetRenderTarget(renderer, blankSurface);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderTarget(renderer, NULL);

	}

	double GetTime()
	{

		return SDL_GetTicks();

	}

	void Delay(double time)
	{

		SDL_Delay(time);

	}

	void PumpKeyState()
	{

		memcpy((void*)previousKeyStates, currentKeyStates, sizeof(Uint8) * numKeys);

		previousMouseButtons = mouseButtons;
		mouseButtons = SDL_GetMouseState(NULL, NULL);

	}

	bool InputKeyDown(SDL_Keycode key)
	{

		return currentKeyStates[key];

	}

	bool InputKeyPress(SDL_Keycode key)
	{

		return currentKeyStates[key] && !previousKeyStates[key];

	}

	bool InputKeyLift(SDL_Keycode key)
	{

		return !currentKeyStates[key] && previousKeyStates[key];

	}

	bool InputMouseDown(int button)
	{

		return mouseButtons & SDL_BUTTON(button);

	}

	bool InputMousePress(int button)
	{

		return (mouseButtons & SDL_BUTTON(button)) && !(previousMouseButtons & SDL_BUTTON(button));

	}

	bool InputMouseLift(int button)
	{

		return !(mouseButtons & SDL_BUTTON(button)) && (previousMouseButtons & SDL_BUTTON(button));

	}

	int MouseX()
	{

		return mouseX;

	}

	int MouseY()
	{

		return mouseY;

	}

	void ClearBuffer()
	{

		SDL_Color color;
		color.r = 0;
		color.g = 0;
		color.b = 0;

		ClearBuffer(color);

	}

	void ClearBuffer(SDL_Color color)
	{

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderClear(renderer);

	}

	void ShowBuffer()
	{

		SDL_RenderPresent(renderer);

	}

	void FillAABB(AABB* box, Vector2 world, int r, int g, int b, int a)
	{

		SDL_Rect rect;
		rect.x = box->X() - world.X();
		rect.y = box->Y() - world.Y();
		rect.w = box->Width();
		rect.h = box->Height();
		
		SDL_SetRenderDrawColor(renderer, r, g, b, a);

		SDL_RenderFillRect(renderer, &rect);

	}

	void DrawAABB(AABB* box, Vector2 world, int r, int g, int b, int a)
	{

		SDL_Rect rect;
		rect.x = box->X() - world.X();
		rect.y = box->Y() - world.Y();
		rect.w = box->Width();
		rect.h = box->Height();

		SDL_SetRenderDrawColor(renderer, r, g, b, a);

		SDL_RenderDrawRect(renderer, &rect);

	}

	SDL_Texture* LoadTexture(std::string fileName)
	{

		if (fileName.size() > 0)
		{

			SDL_Surface* loadedSurface = IMG_Load(fileName.c_str());
			SDL_Texture* finalSurface = NULL;
			if (loadedSurface == NULL)
			{

				return NULL;

			}
			else
			{

				finalSurface = SDL_CreateTextureFromSurface(renderer, loadedSurface);
				if (finalSurface == NULL)
				{

					return NULL;

				}

				SDL_FreeSurface(loadedSurface);

			}

			return finalSurface;

		}

		return NULL;

	}

	Mix_Chunk* LoadSound(int volume, std::string fileName)
	{
		
		if (fileName.size() > 0)
		{

			Mix_Chunk* sound = Mix_LoadWAV(fileName.c_str());
			Mix_VolumeChunk(sound, volume);

			return sound;

		}

		return NULL;

	}
	
	Mix_Music* LoadMusic(int volume, std::string fileName)
	{

		if (fileName.size() > 0)
		{

			Mix_Music* music = Mix_LoadMUS(fileName.c_str());
			Mix_VolumeMusic(volume);

			return music;

		}

		return NULL;

	}
	
	void PlaySound(Mix_Chunk* sound, AABB* origin, int loops)
	{

		SoundHolder holder;
		holder.effect = sound;
		holder.origin = origin;
		holder.numLoops = loops;

		soundQueue.push_back(holder);

	}

	void PumpSounds(Vector2 world)
	{

		AABB viewport(world.X() - BUFFER, world.Y() - BUFFER, PLATFORMER_TARGET_WIDTH + (2 * BUFFER), PLATFORMER_TARGET_HEIGHT + (2 * BUFFER));

		for (int i = 0; i < soundQueue.size(); i++)
		{

			if (soundQueue[i].origin && viewport.Intersects(soundQueue[i].origin))
			{

				Mix_PlayChannel(-1, soundQueue[i].effect, soundQueue[i].numLoops);
			
			}

		}

		soundQueue.clear();

	}

	SDL_Texture* CreateFontTexture(TTF_Font* font, SDL_Color color, std::string text)
	{

		SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), color);

		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

		SDL_FreeSurface(surf);

		return tex;

	}

	void Cleanup()
	{

		if (blankSurface)
		{

			SDL_DestroyTexture(blankSurface);
			blankSurface = NULL;

		}

		if (previousKeyStates)
		{

			delete previousKeyStates;
			previousKeyStates = NULL;

		}

		if (renderer)
		{

			SDL_DestroyRenderer(renderer);
			renderer = NULL;

		}

		if (window)
		{

			SDL_DestroyWindow(window);
			window = NULL;

		}

	}
}