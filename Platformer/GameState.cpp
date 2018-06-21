//Includes
#include <fstream>
#include <sstream>

//Custom Includes
#include "GameState.h"
#include "Helpers.h"

namespace Engine2D
{

	GameState::GameState()
	{

	}

	GameState::~GameState()
	{

	}

	void GameState::Start()
	{

	}

	bool GameState::Running()
	{

		return this->running;

	}

	void GameState::SetRunning(bool r)
	{

		this->running = r;

	}

	MenuInput::MenuInput(int x, int y, int w, int h) : AABB(x, y, w, h)
	{

	}

	Button::Button(int x, int y, int w, int h) : MenuInput(x, y, w, h)
	{

	}

	Button::~Button()
	{

		if (this->created)
		{

			if (this->surface)
			{

				SDL_DestroyTexture(this->surface);
				this->surface = NULL;

			}

		}

	}

	void MenuInput::SetValue(int v)
	{

	}

	void Button::CreateFromText(TTF_Font* font, std::string text, SDL_Color color)
	{

		this->surface = CreateFontTexture(font, color, text);

		int w, h;
		SDL_QueryTexture(this->surface, NULL, NULL, &w, &h);

		this->SetWidth(w);
		this->SetHeight(h);

		this->src.x = this->src.y = 0;
		this->src.w = w;
		this->src.h = h;

		this->created = true;

	}

	void Button::Update(float deltaTime)
	{

		this->pressed = false;

		if (this->Contains(Vector2(mouseX, mouseY)) && InputMousePress(SDL_BUTTON_LEFT))
		{

			this->pressed = true;

		}

	}

	void Button::Render(float deltaTime)
	{

		SDL_Rect rect;
		rect.x = this->X();
		rect.y = this->Y();
		rect.w = this->Width();
		rect.h = this->Height();

		SDL_RenderCopy(renderer, this->surface, &this->src, &rect);

	}

	int Button::Value()
	{

		if (this->pressed)
		{

			this->pressed = false;

			return 1;

		}

		return 0;

	}

	void Button::SetSurface(SDL_Texture* s)
	{

		this->surface = s;

	}

	SDL_Rect* Button::Source()
	{

		return &this->src;

	}

	Slider::Slider(int x, int y, int w, int h) : MenuInput(x, y, w, h)
	{

	}

	void Slider::Update(float deltaTime)
	{

		if (this->Contains(Vector2(MouseX(), MouseY())) && InputMouseDown(SDL_BUTTON_LEFT))
		{

			this->val = MouseX() - this->X();

		}

	}

	void Slider::Render(float deltaTime)
	{

		SDL_Rect rect;
		rect.x = this->X();
		rect.y = this->Y();
		rect.w = this->Width();
		rect.h = this->Height();

		SDL_RenderCopy(renderer, this->backgroundSurface, NULL, &rect);

		rect.x += this->Value();
		SDL_QueryTexture(this->surface, NULL, NULL, &rect.w, &rect.h);
		
		SDL_RenderCopy(renderer, this->surface, NULL, &rect);

	}

	int Slider::Value()
	{

		return this->val;

	}

	void Slider::SetValue(int v)
	{

		this->val = v;

	}

	void Slider::SetSurfaces(SDL_Texture* b, SDL_Texture* s)
	{

		this->backgroundSurface = b;
		this->surface = s;

	}

	MenuState::MenuState()
	{

	}

	MenuState::~MenuState()
	{

		if (this->font)
		{

			TTF_CloseFont(this->font);
			this->font = NULL;

		}

		for (int i = 0; i < this->textures.size(); i++)
		{

			if (this->textures[i])
			{

				SDL_DestroyTexture(this->textures[i]);
				this->textures[i] = NULL;

			}

		}

		std::map<std::string, MenuInput*>::iterator it;
		for (it = this->inputs.begin(); it != this->inputs.end(); it++)
		{

			if (it->second)
			{

				delete it->second;
				it->second = NULL;

			}

		}

		if (this->bgm)
		{

			Mix_FreeMusic(this->bgm);
			this->bgm = NULL;

		}

	}

	void MenuState::Start()
	{

		if (this->bgm && !Mix_PlayingMusic())
		{

			Mix_PlayMusic(this->bgm, true);

		}

	}

	void MenuState::Update(float deltaTime)
	{

		std::map<std::string, MenuInput*>::iterator it;
		for (it = this->inputs.begin(); it != this->inputs.end(); it++)
		{

			it->second->Update(deltaTime);

		}

	}

	void MenuState::Render(float deltaTime)
	{

		ClearBuffer(this->background);

		std::map<std::string, MenuInput*>::iterator it;
		for (it = this->inputs.begin(); it != this->inputs.end(); it++)
		{

			it->second->Render(deltaTime);

		}

		for (int i = 0; i < this->text.size(); i++)
		{

			SDL_Texture* textSurface = CreateFontTexture(this->font, this->text[i].color, this->text[i].text);

			SDL_Rect rect;
			rect.x = this->text[i].pos.X();
			rect.y = this->text[i].pos.Y();
			SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

			SDL_RenderCopy(renderer, textSurface, NULL, &rect);

			SDL_DestroyTexture(textSurface);

		}

	}

	void MenuState::Pause()
	{

	}

	void MenuState::Unpause()
	{

	}

	bool MenuState::UnpauseCondition()
	{

		return true;

	}

	void MenuState::Shutdown()
	{

	}

	void MenuState::MenuFactory(std::string fileName, int volume)
	{

		std::ifstream file(fileName);

		std::string music;
		std::getline(file, music);
		this->bgm = LoadMusic(volume, music);

		std::string fontName;
		std::getline(file, fontName);

		std::string line;
		std::getline(file, line);
		std::istringstream input(line);

		int r, g, b, a;

		input >> r;
		input >> g;
		input >> b;
		input >> a;

		this->background.r = r;
		this->background.g = g;
		this->background.b = b;
		this->background.a = a;

		std::getline(file, line);
		input.clear();
		input.str(line);

		int textSize;
		input >> textSize;

		this->font = TTF_OpenFont(fontName.c_str(), textSize);

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numText;
		input >> numText;

		for (int i = 0; i < numText; i++)
		{

			MenuText newText;

			std::getline(file, newText.text);

			std::getline(file, line);
			input.clear();
			input.str(line);

			int x, y;
			input >> x;
			input >> y;

			newText.pos = Vector2(x, y);

			std::getline(file, line);
			input.clear();
			input.str(line);


			int r, g, b, a;

			input >> r;
			input >> g;
			input >> b;
			input >> a;

			newText.color.r = r;
			newText.color.g = g;
			newText.color.b = b;
			newText.color.a = a;

			this->text.push_back(newText);

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numTextures;
		input >> numTextures;

		for (int i = 0; i < numTextures; i++)
		{

			std::getline(file, line);
			SDL_Texture* texture = LoadTexture(line);

			this->textures.push_back(texture);

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numInputs;
		input >> numInputs;

		for (int i = 0; i < numInputs; i++)
		{

			MenuInput* newInput = NULL;

			std::string name;
			std::getline(file, name);

			std::getline(file, line);
			input.clear();
			input.str(line);

			int x, y, w, h;
			input >> x;
			input >> y;
			input >> w;
			input >> h;

			std::string type;
			std::getline(file, type);

			if (type == "Slider")
			{

				Slider* newSlider = new Slider(x, y, w, h);

				std::getline(file, line);
				input.clear();
				input.str(line);

				int val;
				input >> val;

				newSlider->SetValue(val);

				std::getline(file, line);
				input.clear();
				input.str(line);

				int backIndex, foreIndex;
				input >> backIndex;
				input >> foreIndex;

				newSlider->SetSurfaces(this->textures[backIndex], this->textures[foreIndex]);

				newInput = newSlider;

			}
			else if (type == "Button")
			{

				Button* newButton = new Button(x, y, w, h);

				std::getline(file, line);
				input.clear();
				input.str(line);

				int index;
				input >> index;

				newButton->SetSurface(this->textures[index]);

				std::getline(file, line);
				input.clear();
				input.str(line);

				SDL_Rect* rect = newButton->Source();
				input >> rect->x;
				input >> rect->y;
				input >> rect->w;
				input >> rect->h;

				newInput = newButton;

			}
			else if (type == "Button-Text")
			{

				Button* newButton = new Button(x, y, w, h);

				std::string text;
				std::getline(file, text);

				std::getline(file, line);
				input.clear();
				input.str(line);

				SDL_Color color;
				int r, g, b, a;

				input >> r;
				input >> g;
				input >> b;
				input >> a;

				color.r = r;
				color.g = g;
				color.b = b;
				color.a = a;

				newButton->CreateFromText(this->font, text, color);

				newInput = newButton;

			}

			this->inputs[name] = newInput;

		}

		file.close();

	}

	SplashPage::SplashPage(GameState* n)
	{

		this->nextState = n;

	}

	SplashPage::~SplashPage()
	{

		for (int i = 0; i < this->textures.size(); i++)
		{

			if (this->textures[i])
			{

				SDL_DestroyTexture(this->textures[i]);
				this->textures[i] = NULL;

			}

		}

		for (int i = 0; i < this->spriteSheets.size(); i++)
		{

			if (this->spriteSheets[i])
			{

				delete this->spriteSheets[i];
				this->spriteSheets[i] = NULL;

			}

		}

		for (int i = 0; i < this->text.size(); i++)
		{

			if (this->text[i].text)
			{

				SDL_DestroyTexture(this->text[i].text);
				this->text[i].text = NULL;

			}

		}

		if (this->font)
		{

			TTF_CloseFont(this->font);
			this->font = NULL;

		}

	}

	void SplashPage::Initialize(int argc, char* argv[])
	{

	}

	void SplashPage::LoadContent(int argc, char* argv[])
	{

		if (argc >= 1)
		{

			std::ifstream file(argv[0]);

			std::string line;
			std::getline(file, line);

			std::istringstream input(line);

			input >> this->duration;

			std::getline(file, line);
			input.clear();
			input.str(line);

			int r, g, b;

			input >> r;
			input >> g;
			input >> b;

			this->background.r = r;
			this->background.g = g;
			this->background.b = b;

			std::getline(file, line);
			input.clear();
			input.str(line);

			int numTextures, numSheets, numData, numText;

			input >> numTextures;
			input >> numSheets;
			input >> numText;
			input >> numData;

			for (int i = 0; i < numTextures; i++)
			{

				std::getline(file, line);
				SDL_Texture* tex = LoadTexture(line);

				this->textures.push_back(tex);

			}

			for (int i = 0; i < numSheets; i++)
			{

				std::getline(file, line);
				Spritesheet* sheet = new Spritesheet();
				sheet->LoadSpritesheet(line);

				std::getline(file, line);
				input.clear();
				input.str(line);

				int index;
				input >> index;

				sheet->SetSurface(this->textures[index]);

			}

			for (int i = 0; i < numData; i++)
			{

				DrawData data;

				std::getline(file, line);
				input.clear();
				input.str(line);

				float x, y;
				input >> x;
				input >> y;
				data.location = Vector2(x, y);

				std::getline(file, line);
				input.clear();
				input.str(line);

				input >> x;
				input >> y;
				data.dimensions = Vector2(x, y);

				std::getline(file, line);
				input.clear();
				input.str(line);

				input >> data.color.r;
				input >> data.color.g;
				input >> data.color.b;

				std::getline(file, line);
				input.clear();
				input.str(line);

				input >> data.index;
				input >> data.sheet;
				input >> data.flip;

				this->data.push_back(data);

			}

			if (numText > 0)
			{

				std::getline(file, line);
				input.clear();
				input.str(line);

				std::string fontName;
				int fontSize;

				input >> fontName;
				input >> fontSize;
				this->font = TTF_OpenFont(fontName.c_str(), fontSize);

			}

			for (int i = 0; i < numText; i++)
			{

				TextData newData;

				std::string text;
				std::getline(file, text);

				std::getline(file, line);
				input.clear();
				input.str(line);

				int x, y;

				input >> x;
				input >> y;

				std::getline(file, line);
				input.clear();
				input.str(line);

				input >> r;
				input >> g;
				input >> b;

				newData.color.r = r;
				newData.color.g = g;
				newData.color.b = b;

				newData.text = CreateFontTexture(this->font, newData.color, text);

				int w, h;
				SDL_QueryTexture(newData.text, NULL, NULL, &w, &h);

				newData.location = Vector2(x - w / 2, y - h / 2);

				this->text.push_back(newData);

			}

			file.close();

		}

		this->startTime = GetTime();

	}

	void SplashPage::Update(float deltaTime)
	{

		for (int i = 0; i < this->spriteSheets.size(); i++)
		{

			this->spriteSheets[i]->Update(deltaTime);

		}

		if (GetTime() - this->startTime > this->duration)
		{

			this->SetRunning(false);

		}

	}

	void SplashPage::Render(float deltaTime)
	{

		ClearBuffer(this->background);

		for (int i = 0; i < this->data.size(); i++)
		{

			SDL_Texture* texture = (this->data[i].sheet ? this->spriteSheets[this->data[i].index]->Surface() : this->textures[this->data[i].index]);

			SDL_SetTextureColorMod(texture, this->data[i].color.r, this->data[i].color.g, this->data[i].color.b);

			if (this->data[i].sheet)
			{

				this->spriteSheets[this->data[i].index]->Render(this->data[i].location, this->data[i].flip);

			}
			else
			{

				SDL_Rect rect;
				rect.x = this->data[i].location.X();
				rect.y = this->data[i].location.Y();
				rect.w = this->data[i].dimensions.X();
				rect.h = this->data[i].dimensions.Y();

				SDL_RenderCopy(renderer, texture, NULL, &rect);

			}

		}

		for (int i = 0; i < this->text.size(); i++)
		{

			SDL_Rect dest;
			dest.x = this->text[i].location.X();
			dest.y = this->text[i].location.Y();

			SDL_QueryTexture(this->text[i].text, NULL, NULL, &dest.w, &dest.h);

			SDL_RenderCopy(renderer, this->text[i].text, NULL, &dest);

		}

	}

	void SplashPage::Pause()
	{

	}

	void SplashPage::Unpause()
	{

	}

	bool SplashPage::UnpauseCondition()
	{

		return true;

	}

	void SplashPage::Shutdown()
	{

		if (this->nextState)
		{

			engine.RegisterState(this->nextState);

		}

	}

}