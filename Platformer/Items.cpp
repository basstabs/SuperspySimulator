//Includes

//Custom Includes
#include "Helpers.h"
#include "Items.h"

namespace Platformer
{

	Item::Item(std::string t, float x, float y, float w, float h)
	{

		this->type = t;
		this->box = AABB(x, y, w, h);

	}

	Item::~Item()
	{

		if (this->createdThumbnail && this->thumbnail)
		{

			SDL_DestroyTexture(this->thumbnail);
			this->thumbnail = NULL;

		}

	}

	void Item::Load(std::string sheetFile, std::string sheetImage, std::string thumb)
	{

		this->sheet.LoadSpritesheet(sheetFile);
		this->sheet.LoadSurface(sheetImage);

		this->thumbnail = LoadTexture(thumb);

		this->sheet.PlayAnimation(RandomBoundedInteger(0, this->sheet.NumAnimations() - 1), true);

	}

	void Item::Update(float deltaTime)
	{

		this->sheet.Update(deltaTime);

	}

	void Item::Render(Vector2 world, float deltaTime)
	{

		this->sheet.Render(Vector2(this->box.X(), this->box.Y()) - world, false);

	}

	void Item::SetBlock(std::string n, bool b)
	{

		this->block = b;

		this->name = n;

	}

	bool Item::Block()
	{

		return this->block;

	}

	std::string Item::Name()
	{

		return this->name;

	}

	std::string Item::Type()
	{

		return this->type;

	}

	void Item::SetType(std::string t)
	{

		this->type = t;

	}

	AABB* Item::Box()
	{

		return &this->box;

	}

	SDL_Texture* Item::Thumbnail()
	{

		return this->thumbnail;

	}

	Item* Item::Clone(Vector2 pos)
	{

		Item* item = new Item(this->Type(), pos.X() - this->box.Width() / 2, pos.Y() - this->box.Height() / 2, this->box.Width(), this->box.Height());
		item->name = this->name;

		item->block = this->block;

		item->createdThumbnail = false;
		item->sheet.Clone(&this->sheet);
		item->thumbnail = this->thumbnail;

		item->sheet.PlayAnimation(RandomBoundedInteger(0, item->sheet.NumAnimations() - 1), true);

		return item;

	}

}