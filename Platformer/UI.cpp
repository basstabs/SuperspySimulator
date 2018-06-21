//Includes
#include <SDL_image.h>
#include <sstream>

//Custom Includes
#include "UI.h"
#include "Helpers.h"
#include "Settings.h"
#include "SaveData.h"
#include "ScriptedAction.h"

using namespace Engine2D;

namespace Platformer
{

	UI* UI::singleton = NULL;

	UI::UI()
	{

		this->action = NULL;

	}

	UI::~UI()
	{

		if (this->cursorSurface)
		{

			SDL_DestroyTexture(this->cursorSurface);
			this->cursorSurface = NULL;

		}

		if (this->iconSurface)
		{

			SDL_DestroyTexture(this->iconSurface);
			this->iconSurface = NULL;

		}

		if (this->healthBackground)
		{

			SDL_DestroyTexture(this->healthBackground);
			this->healthBackground = NULL;

		}

		if (this->healthSurface)
		{

			SDL_DestroyTexture(this->healthSurface);
			this->healthSurface = NULL;

		}

		if (this->reputationBackground)
		{

			SDL_DestroyTexture(this->reputationBackground);
			this->reputationBackground = NULL;

		}

		if (this->reputationSurface)
		{

			SDL_DestroyTexture(this->reputationSurface);
			this->reputationSurface = NULL;

		}

		if (this->reputationSymbol)
		{

			SDL_DestroyTexture(this->reputationSymbol);
			this->reputationSymbol = NULL;

		}

		if (this->uiFont)
		{

			TTF_CloseFont(this->uiFont);
			this->uiFont = NULL;

		}

		if (this->iconData)
		{

			delete iconData;
			iconData = NULL;

		}

		if (this->confirmEffect)
		{

			Mix_FreeChunk(this->confirmEffect);
			this->confirmEffect = NULL;

		}

		if (this->cancelEffect)
		{

			Mix_FreeChunk(this->cancelEffect);
			this->cancelEffect = NULL;

		}

		if (this->healEffect)
		{

			Mix_FreeChunk(this->healEffect);
			this->healEffect = NULL;

		}

		if (this->itemEffect)
		{

			Mix_FreeChunk(this->itemEffect);
			this->itemEffect = NULL;

		}

	}

	UI* UI::AccessUI()
	{

		if (singleton)
		{

			return singleton;

		}

		singleton = new UI();
		return singleton;

	}

	void UI::LoadUI(std::string fileName)
	{

		std::ifstream file(fileName);

		std::string line;
		std::getline(file, line);
		this->cursorSurface = LoadTexture(line);
		
		std::getline(file, line);
		this->iconSurface = LoadTexture(line);

		std::getline(file, line);
		std::istringstream input(line);

		int numIcons;
		input >> numIcons;

		this->iconData = new SDL_Rect[numIcons];
		for (int i = 0; i < numIcons; i++)
		{

			std::getline(file, line);
			input.clear();
			input.str(line);

			int x, y;
			input >> x;
			input >> y;

			this->iconData[i].x = x;
			this->iconData[i].y = y;
			this->iconData[i].w = ICON_SIZE;
			this->iconData[i].h = ICON_SIZE;

		}

		std::getline(file, line);
		this->healthBackground = LoadTexture(line);
		
		std::getline(file, line);
		this->healthSurface = LoadTexture(line);

		std::getline(file, line);
		this->reputationBackground = LoadTexture(line);

		std::getline(file, line);
		this->reputationSurface = LoadTexture(line);

		std::getline(file, line);
		input.clear();
		input.str(line);
		input >> this->healthX;
		input >> this->healthY;
		input >> this->healthOffset;
		input >> this->healthCap;

		std::getline(file, line);
		input.clear();
		input.str(line);
		input >> this->reputationX;
		input >> this->reputationY;
		input >> this->reputationOffset;
		input >> this->reputationCap;

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> this->atRiskR;
		input >> this->atRiskG;
		input >> this->atRiskB;

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> this->modHealth;

		if (this->modHealth)
		{

			std::getline(file, line);
			input.clear();
			input.str(line);
			input >> this->healthThreshold;
			input >> this->healthR;
			input >> this->healthG;
			input >> this->healthB;

		}

		std::string font;
		std::getline(file, font);

		std::getline(file, line);
		input.clear();
		input.str(line);

		int textSize;
		input >> textSize;
		input >> this->textHeightOffset;

		this->uiFont = TTF_OpenFont(font.c_str(), textSize);

		std::getline(file, line);
		input.clear();
		input.str(line);

		int r, g, b;

		input >> r;
		input >> g;
		input >> b;

		this->healthTextColor.r = r;
		this->healthTextColor.g = g;
		this->healthTextColor.b = b;
		this->healthTextColor.a = 255;

		std::getline(file, line);
		if (line.length() > 0)
		{

			this->confirmEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), line);

		}

		std::getline(file, line);
		if (line.length() > 0)
		{

			this->cancelEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), line);

		}

		std::getline(file, line);
		if (line.length() > 0)
		{

			this->healEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), line);

		}

		std::getline(file, line);
		if (line.length() > 0)
		{

			this->itemEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), line);

		}

		std::getline(file, line);
		this->reputationSymbol = LoadTexture(line);

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> this->reputationR;
		input >> this->reputationG;
		input >> this->reputationB;

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> this->reputationNeededR;
		input >> this->reputationNeededG;
		input >> this->reputationNeededB;

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> this->inventoryX;
		input >> this->inventoryY;
		input >> this->inventoryStep;
		input >> this->inventoryPerLine;

		file.close();

		SDL_SetTextureAlphaMod(this->healthBackground, UI_TRANSPARENCY);
		SDL_SetTextureAlphaMod(this->healthSurface, UI_TRANSPARENCY);
		SDL_SetTextureAlphaMod(this->reputationBackground, UI_TRANSPARENCY);
		SDL_SetTextureAlphaMod(this->reputationSurface, UI_TRANSPARENCY);

	}

	void UI::RequestRenderAction(ScriptedAction* a)
	{

		this->action = a;

	}

	void UI::RequestNeededReputation(int n)
	{

		this->needed = n;

	}

	void UI::Render(int max, int current, float health, float deltaTime)
	{

		SDL_Rect dest;
		SDL_QueryTexture(this->healthBackground, NULL, NULL, &dest.w, &dest.h);
		dest.x = this->healthX;
		dest.y = this->healthY;

		SDL_RenderCopy(renderer, this->healthBackground, NULL, &dest);

		float centerX = dest.x + dest.w / 2;
		float centerY = dest.y + dest.h / 2;

		SDL_Rect src;

		SDL_QueryTexture(this->healthSurface, NULL, NULL, &src.w, &src.h);
		src.x = 0;
		src.y = 0;
		src.w = this->healthOffset + health * (src.w - this->healthOffset - this->healthCap);
		dest.w = src.w;

		if (this->modHealth)
		{

			if (health <= this->healthThreshold)
			{

				SDL_SetTextureColorMod(this->healthSurface, this->healthR, this->healthG, this->healthB);

			}

		}

		SDL_RenderCopy(renderer, this->healthSurface, &src, &dest);

		SDL_SetTextureColorMod(this->healthSurface, 255, 255, 255);

		std::ostringstream convert;
		std::string healthText = "";

		convert << current;
		healthText += convert.str();

		convert.str("");
		convert.clear();
		convert << max;

		healthText += " / ";
		healthText += convert.str();

		SDL_Texture* healthTexture = CreateFontTexture(this->uiFont, this->healthTextColor, healthText);
		SDL_QueryTexture(healthTexture, NULL, NULL, &dest.w, &dest.h);

		dest.x = centerX - dest.w / 2;
		dest.y = centerY - dest.h / 2 + this->textHeightOffset;

		SDL_RenderCopy(renderer, healthTexture, NULL, &dest);
		SDL_DestroyTexture(healthTexture);

		if (this->action)
		{

			this->action->Render();

			this->action = NULL;

		}

		if (this->temp)
		{

			this->temp = false;
			this->RenderInput(this->tempIndices, this->tempTransparency, deltaTime);

		}

		this->RenderReputation();

	}

	void UI::RenderInventory(std::vector<Item*> inventory, float deltaTime)
	{

		SDL_Rect dest;

		for (int i = 0; i < inventory.size(); i++)
		{

			SDL_QueryTexture(inventory[i]->Thumbnail(), NULL, NULL, &dest.w, &dest.h);

			dest.x = this->inventoryX + (i % this->inventoryPerLine) * this->inventoryStep;
			dest.y = this->inventoryY + (i / this->inventoryPerLine) * this->inventoryStep;

			SDL_RenderCopy(renderer, inventory[i]->Thumbnail(), NULL, &dest);

		}

	}

	void UI::RenderReputation()
	{

		SaveData* data = SaveData::AccessSaveData();

		int width, height;

		SDL_Rect dest;
		SDL_QueryTexture(this->reputationBackground, NULL, NULL, &width, &height);
		dest.x = this->reputationX;
		dest.y = this->reputationY;
		dest.w = width;
		dest.h = height;

		SDL_RenderCopy(renderer, this->reputationBackground, NULL, &dest);

		float centerX = dest.x + width/ 2;
		float centerY = dest.y + height / 2;

		SDL_Rect src;

		float atRisk = ((float)((data->GetAtRiskReputation() + data->GetReputation()) % REPUTATION_LEVEL_SIZE)) / REPUTATION_LEVEL_SIZE;

		src.y = 0;
		src.w = this->reputationOffset + atRisk * (width - this->reputationOffset - this->reputationCap);
		src.h = height;
		src.x = width - src.w;
		dest.x = this->reputationX + width - src.w;
		dest.w = src.w;

		SDL_SetTextureColorMod(this->reputationSurface, this->atRiskR, this->atRiskG, this->atRiskB);

		SDL_RenderCopy(renderer, this->reputationSurface, &src, &dest);

		SDL_SetTextureColorMod(this->reputationSurface, 255, 255, 255);

		if ((data->GetReputation() % REPUTATION_LEVEL_SIZE) + data->GetAtRiskReputation() < REPUTATION_LEVEL_SIZE)
		{

			float reputation = ((float)(data->GetReputation() % REPUTATION_LEVEL_SIZE)) / REPUTATION_LEVEL_SIZE;

			src.w = this->reputationOffset + reputation * (width - this->reputationOffset - this->reputationCap);
			src.h = height;
			src.x = width - src.w;
			dest.x = this->reputationX + width - src.w;
			dest.w = src.w;

			SDL_RenderCopy(renderer, this->reputationSurface, &src, &dest);

		}

		std::ostringstream convert;
		std::string reputationText = " Lvl. ";

		SDL_Color color;

		if (this->needed > SaveData::AccessSaveData()->GetReputation() + SaveData::AccessSaveData()->GetAtRiskReputation())
		{

			convert << this->needed / REPUTATION_LEVEL_SIZE;

			color.r = this->reputationNeededR;
			color.g = this->reputationNeededG;
			color.b = this->reputationNeededB;

			this->needed = 0;

		}
		else
		{

			convert << (SaveData::AccessSaveData()->GetReputation() + SaveData::AccessSaveData()->GetAtRiskReputation()) / REPUTATION_LEVEL_SIZE;

			bool atRisk = (SaveData::AccessSaveData()->GetReputation() + SaveData::AccessSaveData()->GetAtRiskReputation()) / REPUTATION_LEVEL_SIZE > (SaveData::AccessSaveData()->GetReputation()) / REPUTATION_LEVEL_SIZE;

			if (atRisk)
			{

				color.r = this->atRiskR;
				color.g = this->atRiskG;
				color.b = this->atRiskB;

			}
			else
			{

				color.r = this->reputationR;
				color.g = this->reputationG;
				color.b = this->reputationB;

			}

		}

		reputationText += convert.str() + " ";

		SDL_Texture* reputationTexture = CreateFontTexture(this->uiFont, color, reputationText);
		SDL_QueryTexture(reputationTexture, NULL, NULL, &dest.w, &dest.h);
		dest.x = this->reputationX - dest.w;
		dest.y = this->reputationY + height - dest.h;

		SDL_RenderCopy(renderer, reputationTexture, NULL, &dest);

		SDL_DestroyTexture(reputationTexture);

		SDL_QueryTexture(this->reputationSymbol, NULL, NULL, &dest.w, &dest.h);

		dest.x = dest.x - dest.w;
		dest.y = this->reputationY;

		SDL_RenderCopy(renderer, this->reputationSymbol, NULL, &dest);

	}

	void UI::RenderMouse()
	{

		int width, height;

		SDL_Rect dest;
		SDL_QueryTexture(this->cursorSurface, NULL, NULL, &width, &height);
		dest.x = MouseX();
		dest.y = MouseY();
		dest.w = width;
		dest.h = height;

		SDL_RenderCopy(renderer, this->cursorSurface, NULL, &dest);

	}

	void UI::RequestRenderInput(std::vector<int> indices, float transparency)
	{

		this->temp = true;

		this->tempIndices = indices;
		this->tempTransparency = transparency;

	}

	void UI::RenderInput(std::vector<int> indices, float transparency, float deltaTime)
	{

		SDL_SetTextureAlphaMod(this->iconSurface, transparency);

		float width = (2 * indices.size() - 1) * ICON_SIZE;

		for (int i = 0; i < indices.size(); i++)
		{

			SDL_Rect dest;
			dest.x = PLATFORMER_TARGET_WIDTH / 2 - width / 2 + (2 * ICON_SIZE * i);
			dest.y = PLATFORMER_TARGET_HEIGHT / 2 - ICON_SIZE / 2;
			dest.w = ICON_SIZE;
			dest.h = ICON_SIZE;

			SDL_RenderCopyEx(renderer, this->iconSurface, &(this->iconData[indices[i]]), &dest, 0, NULL, SDL_FLIP_NONE);

			if (i < indices.size() - 1)
			{

				dest.x += ICON_SIZE;

				SDL_RenderCopyEx(renderer, this->iconSurface, &this->iconData[JOIN_CODE], &dest, 0, NULL, SDL_FLIP_NONE);

			}

		}

		SDL_SetTextureAlphaMod(this->iconSurface, 255);

	}

	void UI::RenderText(Vector2 pos, std::string text, SDL_Color color, bool center)
	{

		SDL_Rect dest;
		SDL_Texture* textSurface = CreateFontTexture(this->uiFont, color, text);
		SDL_QueryTexture(textSurface, NULL, NULL, &dest.w, &dest.h);
		dest.x = pos.X() - (center ? dest.w / 2 : 0);
		dest.y = pos.Y() - (center ? dest.h / 2 : 0);

		SDL_RenderCopy(renderer, textSurface, NULL, &dest);

		SDL_DestroyTexture(textSurface);

	}

	void UI::PlayConfirmEffect()
	{

		Mix_PlayChannel(-1, this->confirmEffect, 0);

	}

	void UI::PlayCancelEffect()
	{

		Mix_PlayChannel(-1, this->cancelEffect, 0);

	}

	void UI::PlayHealEffect()
	{

		Mix_PlayChannel(-1, this->healEffect, 0);

	}

	void UI::PlayItemEffect()
	{

		Mix_PlayChannel(-1, this->itemEffect, 0);

	}

	void UI::UpdateVolumes()
	{

		Mix_VolumeChunk(this->confirmEffect, Settings::AccessSettings()->SoundVolume());
		Mix_VolumeChunk(this->cancelEffect, Settings::AccessSettings()->SoundVolume());
		Mix_VolumeChunk(this->healEffect, Settings::AccessSettings()->SoundVolume());
		Mix_VolumeChunk(this->itemEffect, Settings::AccessSettings()->SoundVolume());

		Mix_VolumeMusic(Settings::AccessSettings()->MusicVolume());

	}

	SDL_Texture* UI::InputsTexture()
	{

		return this->iconSurface;

	}

	SDL_Rect* UI::InputSource(int i)
	{

		return &this->iconData[i];

	}

}