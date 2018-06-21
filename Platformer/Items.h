#pragma once

//Includes
#include <SDL.h>

//Custom Includes
#include "AABB.h"
#include "Spritesheet.h"

using namespace Engine2D;

namespace Platformer
{

	class Item
	{

	//Methods

	public:

		Item(std::string t, float x, float y, float w, float h);
		~Item();

		void Load(std::string sheetFile, std::string sheetImage, std::string thumb);

		void Update(float deltaTime);
		void Render(Vector2 world, float deltaTime);

		void SetBlock(std::string n, bool b);

		bool Block();
		std::string Name();

		std::string Type();
		void SetType(std::string t);
		AABB* Box();

		SDL_Texture* Thumbnail();

		Item* Clone(Vector2 pos);

	protected:

	//Data

	public:

	protected:

		SDL_Texture* thumbnail;
		
		Spritesheet sheet;

		std::string type;
		AABB box;

		std::string name;
		bool block;

		bool createdThumbnail = true;

	};

}