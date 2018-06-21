//Includes
#include <fstream>
#include <sstream>
#include <SDL_image.h>

//Custom Includes
#include "Helpers.h"
#include "Spritesheet.h"

namespace Engine2D
{

	Frame::Frame(int sX, int sY, int sW, int sH, double d)
	{

		this->srcX = sX;
		this->srcY = sY;
		this->srcWidth = sW;
		this->srcHeight = sH;

		this->duration = d;

	}

	Frame::~Frame()
	{

	}

	int Frame::SourceX()
	{

		return this->srcX;

	}

	int Frame::SourceY()
	{

		return this->srcY;

	}

	int Frame::SourceWidth()
	{

		return this->srcWidth;

	}

	int Frame::SourceHeight()
	{

		return this->srcHeight;

	}

	double Frame::Duration()
	{

		return this->duration;

	}

	Frame Frame::Clone()
	{

		return Frame(this->srcX, this->srcY, this->srcWidth, this->srcHeight, this->duration);

	}

	Animation::Animation()
	{

		this->Stop();

	}

	Animation::~Animation()
	{

	}

	void Animation::Reset()
	{

		this->currentIndex = 0;

	}

	void Animation::Play(bool l, double c, int start)
	{

		this->loop = l;

		this->currentIndex = start;
		this->lastFrame = c;

	}

	void Animation::Update(float deltaTime)
	{

		if (this->currentIndex >= 0 && GetTime() - this->lastFrame >= this->frames[this->currentIndex]->Duration())
		{

			this->lastFrame = GetTime();
			this->currentIndex++;

			if (this->currentIndex >= this->frames.size())
			{

				if (this->loop)
				{

					this->currentIndex = 0;

				}
				else
				{

					this->Stop();

				}

			}

		}

	}

	void Animation::Stop()
	{

		this->currentIndex = -1;
		this->loop = false;

	}

	void Animation::AddFrame(Frame* frame)
	{

		this->frames.push_back(frame);

	}

	void Animation::Pause()
	{

		this->paused = true;
		this->pauseTime = GetTime();

	}

	void Animation::Unpause()
	{

		this->paused = false;
		this->lastFrame = GetTime() - (this->pauseTime - this->lastFrame);

	}

	Frame* Animation::CurrentFrame()
	{

		if (this->currentIndex < 0)
		{

			return NULL;

		}

		return this->frames[this->currentIndex];

	}

	int Animation::CurrentIndex()
	{

		return this->currentIndex;

	}

	double Animation::CurrentTime()
	{

		return this->lastFrame;

	}

	std::vector<Frame*> Animation::Frames()
	{

		return this->frames;

	}

	Spritesheet::Spritesheet()
	{

		this->currentAnimation = -1;

	}

	Spritesheet::~Spritesheet()
	{

		if (this->createdSurface)
		{

			SDL_DestroyTexture(this->surface);
			this->surface = NULL;

		}

	}

	void Spritesheet::Reset()
	{

		this->currentAnimation = -1;

		for (int i = 0; i < this->animations.size(); i++)
		{

			this->animations[i].Reset();

		}

	}

	void Spritesheet::LoadSpritesheet(std::string fileName)
	{

		std::ifstream infile(fileName);
		
		std::string line;
		std::getline(infile, line);

		std::stringstream in(line);
		
		in >> this->defaultX;
		in >> this->defaultY;
		in >> this->defaultWidth;
		in >> this->defaultHeight;

		int numFrames, numAnimations;

		std::getline(infile, line);
		in.clear();
		in.str(line);

		in >> numFrames;
		in >> numAnimations;

		for (int i = 0; i < numFrames; i++)
		{

			int x, y, w, h;
			double d;

			std::getline(infile, line);
			in.clear();
			in.str(line);

			in >> x;
			in >> y;
			in >> w;
			in >> h;
			in >> d;

			this->frames.push_back(Frame(x, y, w, h, d));

		}

		for (int i = 0; i < numAnimations; i++)
		{

			Animation newAnimation;

			int numAnimationFrames;

			std::getline(infile, line);
			in.clear();
			in.str(line);

			in >> numAnimationFrames;

			std::getline(infile, line);
			in.clear();
			in.str(line);

			int index;

			for (int j = 0; j < numAnimationFrames; j++)
			{

				in >> index;
				newAnimation.AddFrame(&(this->frames[index]));

			}

			this->animations.push_back(newAnimation);

		}

		infile.close();

	}

	void Spritesheet::LoadSurface(std::string fileName)
	{

		this->surface = LoadTexture(fileName);
		this->createdSurface = true;

	}

	void Spritesheet::PlayAnimation(int animation, bool loop)
	{

		if (animation >= 0 && animation < this->animations.size() && animation != this->currentAnimation)
		{

			this->currentAnimation = animation;
			this->animations[animation].Play(loop, GetTime(), 0);

		}

	}

	void Spritesheet::ShiftAnimation(int animation, bool loop)
	{

		if (animation >= 0 && animation < this->animations.size())
		{

			int currentIndex = (this->currentAnimation >= 0 ? this->animations[this->currentAnimation].CurrentIndex() : 0);
			double currentTime = (this->currentAnimation >= 0? this->animations[this->currentAnimation].CurrentTime() : 0);

			this->currentAnimation = animation;
			this->animations[animation].Play(loop, currentTime, currentIndex);

		}

	}

	void Spritesheet::Update(float deltaTime)
	{

		if (this->currentAnimation >= 0 && this->currentAnimation < this->animations.size())
		{

			this->animations[this->currentAnimation].Update(deltaTime);

		}

	}

	void Spritesheet::Pause()
	{

		if (this->currentAnimation >= 0 && this->currentAnimation < this->animations.size())
		{

			this->animations[this->currentAnimation].Pause();

		}

	}

	void Spritesheet::Unpause()
	{

		if (this->currentAnimation >= 0 && this->currentAnimation < this->animations.size())
		{

			this->animations[this->currentAnimation].Unpause();

		}

	}

	void Spritesheet::Render(Vector2 pos, bool flip)
	{

		Frame* frame = this->animations[(this->currentAnimation == -1 ? 0 : this->currentAnimation)].CurrentFrame();
		SDL_Rect src, dest;

		if (frame == NULL)
		{

			src.x = this->defaultX;
			src.y = this->defaultY;
			src.w = this->defaultWidth;
			src.h = this->defaultHeight;

		}
		else
		{

			src.x = frame->SourceX();
			src.y = frame->SourceY();
			src.w = frame->SourceWidth();
			src.h = frame->SourceHeight();

		}

		dest.x = pos.X();
		dest.y = pos.Y();
		dest.w = src.w;
		dest.h = src.h;

		SDL_RenderCopyEx(renderer, this->surface, &src, &dest, 0, NULL, (flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE));
		
	}

	void Spritesheet::SetSurface(SDL_Texture* surf)
	{

		this->surface = surf;

	}

	int Spritesheet::CurrentAnimation()
	{

		return this->currentAnimation;

	}

	int Spritesheet::NumAnimations()
	{

		return this->animations.size();

	}

	void Spritesheet::Clone(Spritesheet* sheet)
	{

		this->defaultX = sheet->defaultX;
		this->defaultY = sheet->defaultY;
		this->defaultWidth = sheet->defaultWidth;
		this->defaultHeight = sheet->defaultHeight;

		this->frames.clear();
		for (int i = 0; i < sheet->frames.size(); i++)
		{

			this->frames.push_back(sheet->frames[i].Clone());

		}

		this->animations.clear();
		for (int i = 0; i < sheet->animations.size(); i++)
		{

			Animation newAnimation;

			std::vector<Frame*> frames = sheet->animations[i].Frames();
			for (int j = 0; j < frames.size(); j++)
			{

				newAnimation.AddFrame(&this->frames[frames[j] - &sheet->frames[0]]);

			}

			this->animations.push_back(newAnimation);

		}

		this->currentAnimation = -1;

		this->createdSurface = false;
		this->SetSurface(sheet->surface);

	}

	SDL_Texture* Spritesheet::Surface()
	{

		return this->surface;

	}

	bool Spritesheet::Playing()
	{

		if (this->currentAnimation == -1)
		{

			return false;

		}

		return this->animations[this->currentAnimation].CurrentIndex() != -1;

	}

}