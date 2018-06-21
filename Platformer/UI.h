#pragma once

//Includes
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <fstream>
#include <vector>

//Custom Includes
#include "Vector2.h"
#include "Items.h"

#define ICON_SIZE 32

#define JOIN_CODE 8

#define UI_TRANSPARENCY 128

using namespace Engine2D;

namespace Platformer
{

	class ScriptedAction;

	class UI
	{

	//Methods

	public:

		~UI();

		static UI* AccessUI();

		void LoadUI(std::string fileName);

		void Render(int max, int current, float health, float deltaTime);
		void RenderInventory(std::vector<Item*> inventory, float deltaTime);
		void RenderReputation();

		void RequestRenderAction(ScriptedAction* a);
		void RequestRenderInput(std::vector<int> indices, float transparency);
		void RequestNeededReputation(int n);
		void RenderInput(std::vector<int> indices, float transparency, float deltaTime);
		void RenderMouse();

		void RenderText(Vector2 pos, std::string text, SDL_Color color, bool center = true);

		void PlayConfirmEffect();
		void PlayCancelEffect();
		void PlayHealEffect();
		void PlayItemEffect();

		void UpdateVolumes();

		SDL_Texture* InputsTexture();
		SDL_Rect* InputSource(int i);

	protected:

		UI();

	//Data

	public:

	protected:

		static UI* singleton;

		SDL_Texture* cursorSurface;

		SDL_Texture* iconSurface;
		SDL_Rect* iconData;

		SDL_Texture* healthSurface;
		SDL_Texture* healthBackground;

		SDL_Texture* reputationSurface;
		SDL_Texture* reputationBackground;
		SDL_Texture* reputationSymbol;

		int reputationR, reputationG, reputationB;
		int reputationNeededR, reputationNeededG, reputationNeededB;

		int healthX, healthY, reputationX, reputationY;
		int healthOffset, healthCap, reputationOffset, reputationCap;

		int atRiskR, atRiskG, atRiskB;

		bool modHealth;
		float healthThreshold;
		int healthR, healthG, healthB;
		
		TTF_Font* uiFont;
		SDL_Color healthTextColor;
		int textHeightOffset;

		int inventoryX, inventoryY, inventoryStep, inventoryPerLine;

		bool temp = false;
		std::vector<int> tempIndices;
		float tempTransparency;

		ScriptedAction* action;

		int needed = 0;

		Mix_Chunk* confirmEffect;
		Mix_Chunk* cancelEffect;
		Mix_Chunk* healEffect;
		Mix_Chunk* itemEffect;

	};

}
