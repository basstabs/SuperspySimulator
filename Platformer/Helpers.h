#pragma once

//Includes
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

//Custom Includes
#include "AABB.h"
#include "Engine.h"

#define PLATFORMER_TARGET_WIDTH 640
#define PLATFORMER_TARGET_HEIGHT 360

extern const Uint8* currentKeyStates;
extern const Uint8* previousKeyStates;

extern int mouseX;
extern int mouseY;
extern Uint32 mouseButtons;
extern Uint32 previousMouseButtons;

extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern SDL_Texture* blankSurface;

extern Engine2D::Engine engine;

extern int windowWidth;
extern int windowHeight;
extern bool fullScreen;

namespace Engine2D
{

	struct SoundHolder
	{

		Mix_Chunk* effect;
		AABB* origin;
		int numLoops;

	};

	void SeedRandom(int seed);
	int RandomBoundedInteger(int min, int max);

	extern std::vector<SoundHolder> soundQueue;

	void PrepareBlank();

	double GetTime();
	void Delay(double time);

	void PumpKeyState();
	bool InputKeyDown(SDL_Keycode key);
	bool InputKeyPress(SDL_Keycode key);
	bool InputKeyLift(SDL_Keycode key);
	
	bool InputMouseDown(int button);
	bool InputMousePress(int button);
	bool InputMouseLift(int button);
	int MouseX();
	int MouseY();

	void ClearBuffer();
	void ClearBuffer(SDL_Color color);

	void ShowBuffer();
	void FillAABB(AABB* box, Vector2 world, int r, int g, int b, int a);
	void DrawAABB(AABB* box, Vector2 world, int r, int g, int b, int a);

	SDL_Texture* LoadTexture(std::string fileName);
	Mix_Chunk* LoadSound(int volume, std::string fileName);
	Mix_Music* LoadMusic(int volume, std::string fileName);

	void PlaySound(Mix_Chunk* sound, AABB* origin, int loops);
	void PumpSounds(Vector2 world);

	SDL_Texture* CreateFontTexture(TTF_Font* font, SDL_Color color, std::string text);

	void Cleanup();

}