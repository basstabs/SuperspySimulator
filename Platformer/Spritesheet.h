#pragma once

//Includes
#include <SDL.h>
#include <string>
#include <vector>

//Custom Includes

namespace Engine2D
{

	class Frame
	{

	//Methods

	public:

		Frame(int sX, int sY, int sW, int sH, double d);
		~Frame();

		int SourceX();
		int SourceY();
		int SourceWidth();
		int SourceHeight();

		double Duration();

		Frame Clone();

	protected:

	//Data

	public:

	protected:

		int srcX;
		int srcY;
		int srcWidth;
		int srcHeight;

		double duration;

	};

	class Animation
	{

	//Methods

	public:

		Animation();
		~Animation();

		void Reset();

		void Play(bool l, double c, int start = 0);
		void Update(float deltaTime);
		void Stop();

		void AddFrame(Frame* frame);

		void Pause();
		void Unpause();

		Frame* CurrentFrame();
		int CurrentIndex();
		double CurrentTime();

		std::vector<Frame*> Frames();

	protected:

	//Data

	public:

	protected:

		std::vector<Frame*> frames;

		int currentIndex;
		bool loop;

		double lastFrame;

		bool paused;
		double pauseTime;

	};

	class Spritesheet
	{

	//Methods

	public:

		Spritesheet();
		~Spritesheet();

		void Reset();

		void LoadSpritesheet(std::string fileName);
		void LoadSurface(std::string fileName);
		void PlayAnimation(int index, bool loop);
		void ShiftAnimation(int index, bool loop);
		void Update(float deltaTime);

		void Pause();
		void Unpause();

		void Render(Vector2 pos, bool flip);

		void SetSurface(SDL_Texture* surf);

		int CurrentAnimation();
		int NumAnimations();

		void Clone(Spritesheet* sheet);

		SDL_Texture* Surface();

		bool Playing();

	protected:

	//Data

	public:

	protected:

		int defaultX;
		int defaultY;
		int defaultWidth;
		int defaultHeight;

		std::vector<Frame> frames;
		std::vector<Animation> animations;

		int currentAnimation;

		bool createdSurface = false;
		SDL_Texture* surface;

	};

}