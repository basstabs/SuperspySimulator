//Includes
#include <sstream>

//Custom Includes
#include "ResultsScreen.h"
#include "Settings.h"
#include "Helpers.h"
#include "UI.h"
#include "StatsMenu.h"

namespace Platformer
{

	ResultsScreen::ResultsScreen(int damsels, int dossiers, int info, int bonus)
	{

		this->numDamsels = damsels;
		this->numDossiers = dossiers;
		this->numInfo = info;
		this->levelBonus = bonus;

	}

	ResultsScreen::~ResultsScreen()
	{

		if (this->stageEffect)
		{

			Mix_FreeChunk(this->stageEffect);
			this->stageEffect = NULL;

		}

	}

	void ResultsScreen::Initialize(int argc, char* argv[])
	{

		this->startTime = GetTime();

	}

	void ResultsScreen::LoadContent(int argc, char* argv[])
	{

		this->MenuFactory("./Assets/Data/Menu/Results.plMNU", Settings::AccessSettings()->MusicVolume());

		this->stageEffect = LoadSound(Settings::AccessSettings()->SoundVolume(), "./Assets/Sounds/Effects/Shoot.wav");

	}

	void ResultsScreen::Update(float deltaTime)
	{

		this->MenuState::Update(deltaTime);

		if (this->stage < RESULTS_MAX_STAGE)
		{

			if (this->counter > 0)
			{

				this->counter += RESULTS_TIME_STEP * deltaTime;

				if (this->counter > this->CounterTarget())
				{

					this->counter = 0;

					this->startTime = GetTime();

				}

			}

			if (this->counter == 0 && GetTime() - this->startTime >= RESULTS_TIME_STEP)
			{

				this->stage++;

				PlaySound(this->stageEffect, &AABB(1, 1, 1, 1), 0);

				if (this->stage < RESULTS_MAX_STAGE)
				{

					if (this->stage % 2 == 1)
					{

						this->counter += RESULTS_TIME_STEP * deltaTime;

					}

					this->startTime = GetTime();

				}

			}

		}

		if (this->inputs["Finish"]->Value())
		{

			if (this->stage < RESULTS_MAX_STAGE)
			{

				PlaySound(this->stageEffect, &AABB(1, 1, 1, 1), 0);

				this->stage = RESULTS_MAX_STAGE;

			}
			else
			{

				UI::AccessUI()->PlayConfirmEffect();

				StatsMenu* stats = new StatsMenu();
				stats->Initialize(0, NULL);
				stats->LoadContent(0, NULL);
				stats->SetRunning(true);

				engine.RegisterState(stats);

				this->SetRunning(false);

			}

		}

		PumpSounds(Vector2());

	}

	float ResultsScreen::CounterTarget()
	{

		if (this->stage == 1)
		{

			return this->levelBonus;

		}
		else if (this->stage == 3)
		{

			return this->numDamsels * Settings::AccessSettings()->ReputationByType("Damsel");

		}
		else if (this->stage == 5)
		{

			return this->numDossiers * Settings::AccessSettings()->ReputationByType("Dossier");

		}
		else
		{

			return this->numInfo * Settings::AccessSettings()->ReputationByType("Info");

		}
		
	}

	void ResultsScreen::Render(float deltaTime)
	{

		this->MenuState::Render(deltaTime);

		if (this->stage >= 1)
		{

			SDL_Color color;
			color.r = color.b = 0;
			color.g = color.a = 255;

			std::ostringstream parser;
			parser << (int)(stage == 1 && this->counter > 0 ? this->counter : this->levelBonus);

			SDL_Texture* textSurface = CreateFontTexture(this->font, color, parser.str());

			SDL_Rect rect;
			rect.x = this->text[1].pos.X() + 256;
			rect.y = this->text[1].pos.Y();
			SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

			SDL_RenderCopy(renderer, textSurface, NULL, &rect);

			SDL_DestroyTexture(textSurface);

			if (this->stage >= 2)
			{

				parser.clear();
				parser.str("");
				parser << "x" << this->numDamsels;

				textSurface = CreateFontTexture(this->font, color, parser.str());

				SDL_Rect rect;
				rect.x = this->text[2].pos.X() + 160;
				rect.y = this->text[2].pos.Y();
				SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

				SDL_RenderCopy(renderer, textSurface, NULL, &rect);

				SDL_DestroyTexture(textSurface);

				if (this->stage >= 3)
				{

					parser.clear();
					parser.str("");
					parser << (int)(stage == 3 && this->counter > 0 ? this->counter : this->numDamsels * Settings::AccessSettings()->ReputationByType("Damsel"));

					textSurface = CreateFontTexture(this->font, color, parser.str());

					SDL_Rect rect;
					rect.x = this->text[2].pos.X() + 256;
					rect.y = this->text[2].pos.Y();
					SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

					SDL_RenderCopy(renderer, textSurface, NULL, &rect);

					SDL_DestroyTexture(textSurface);

					if (this->stage >= 4)
					{

						parser.clear();
						parser.str("");
						parser << "x" << this->numDossiers;

						textSurface = CreateFontTexture(this->font, color, parser.str());

						SDL_Rect rect;
						rect.x = this->text[3].pos.X() + 160;
						rect.y = this->text[3].pos.Y();
						SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

						SDL_RenderCopy(renderer, textSurface, NULL, &rect);

						SDL_DestroyTexture(textSurface);

						if (this->stage >= 5)
						{

							parser.clear();
							parser.str("");
							parser << (int)(stage == 5 && this->counter > 0 ? this->counter : this->numDossiers * Settings::AccessSettings()->ReputationByType("Dossier"));

							textSurface = CreateFontTexture(this->font, color, parser.str());

							SDL_Rect rect;
							rect.x = this->text[3].pos.X() + 256;
							rect.y = this->text[3].pos.Y();
							SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

							SDL_RenderCopy(renderer, textSurface, NULL, &rect);

							SDL_DestroyTexture(textSurface);

							if (this->stage >= 6)
							{

								parser.clear();
								parser.str("");
								parser << "x" << this->numInfo;

								textSurface = CreateFontTexture(this->font, color, parser.str());

								SDL_Rect rect;
								rect.x = this->text[4].pos.X() + 160;
								rect.y = this->text[4].pos.Y();
								SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

								SDL_RenderCopy(renderer, textSurface, NULL, &rect);

								SDL_DestroyTexture(textSurface);

								if (this->stage >= 7)
								{

									parser.clear();
									parser.str("");
									parser << (int)(stage == 7 && this->counter > 0 ? this->counter : this->numInfo * Settings::AccessSettings()->ReputationByType("Info"));

									textSurface = CreateFontTexture(this->font, color, parser.str());

									SDL_Rect rect;
									rect.x = this->text[4].pos.X() + 256;
									rect.y = this->text[4].pos.Y();
									SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

									SDL_RenderCopy(renderer, textSurface, NULL, &rect);

									SDL_DestroyTexture(textSurface);

									if (this->stage >= RESULTS_MAX_STAGE)
									{

										parser.clear();
										parser.str("");
										parser << this->levelBonus + (this->numDamsels * Settings::AccessSettings()->ReputationByType("Damsel")) + (this->numDossiers * Settings::AccessSettings()->ReputationByType("Dossier")) + (this->numInfo * Settings::AccessSettings()->ReputationByType("Info"));

										textSurface = CreateFontTexture(this->font, color, parser.str());

										SDL_Rect rect;
										rect.x = this->text[5].pos.X() + 256;
										rect.y = this->text[5].pos.Y();
										SDL_QueryTexture(textSurface, NULL, NULL, &rect.w, &rect.h);

										SDL_RenderCopy(renderer, textSurface, NULL, &rect);

										SDL_DestroyTexture(textSurface);

									}

								}

							}
						}

					}

				}

			}

		}

		UI::AccessUI()->RenderMouse();

	}

	void ResultsScreen::Pause()
	{

	}

	void ResultsScreen::Unpause()
	{

	}

	bool ResultsScreen::UnpauseCondition()
	{

		return true;

	}

	void ResultsScreen::Shutdown()
	{

	}

}