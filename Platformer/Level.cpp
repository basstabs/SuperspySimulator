//Includes
#include <math.h>
#include <sstream>

//Custom Includes
#include "Helpers.h"
#include "Settings.h"
#include "Level.h"
#include "Platformer.h"
#include "SaveData.h"

#define DATA_DIRECTORY "./Assets/Data/"
#define COLLISION_DIRECTORY "Collision/"
#define MASTER_DIRECTORY "Master/"
#define EVENT_DIRECTORY "Event/"
#define RENDER_DIRECTORY "Render/"
#define IMAGE_DIRECTORY "./Assets/Images/"

namespace Platformer
{

	Level::Level(Platformer* p)
	{

		this->platformer = p;

		this->tileImage = NULL;

	}

	Level::Level(Platformer* p, int w, int h, std::string collision, std::string enemy_master, std::string events) : AABB(0, 0, w, h)
	{

		this->platformer = p;

		this->tileImage = NULL;

		LoadCollision(collision);
		LoadMaster(enemy_master);
		LoadEvents(events);

	}

	Level::~Level()
	{

		std::map<std::string, Enemy*>::iterator it;
		for (it = this->master.begin(); it != this->master.end(); it++)
		{

			if (it->second)
			{

				delete it->second;
				it->second = NULL;

			}

		}

		std::map<std::string, GameEntity*>::iterator iter;
		for (iter = this->breakables.begin(); iter != this->breakables.end(); iter++)
		{

			if (iter->second)
			{

				delete iter->second;
				iter->second = NULL;

			}

		}

		for (int i = 0; i < this->backgroundSheets.size(); i++)
		{

			if (this->backgroundSheets[i])
			{

				delete this->backgroundSheets[i];
				this->backgroundSheets[i] = NULL;

			}

		}

		for (int i = 0; i < this->foregroundSheets.size(); i++)
		{

			if (this->foregroundSheets[i])
			{

				delete this->foregroundSheets[i];
				this->foregroundSheets[i] = NULL;

			}

		}

		for (int i = 0; i < this->spawners.size(); i++)
		{

			if (this->spawners[i])
			{

				delete this->spawners[i];
				this->spawners[i] = NULL;

			}

		}

		for (int i = 0; i < this->triggers.size(); i++)
		{

			if (this->triggers[i])
			{

				delete this->triggers[i];
				this->triggers[i] = NULL;

			}

		}

		if (this->tileImage)
		{

			SDL_DestroyTexture(this->tileImage);
			this->tileImage = NULL;

		}

		if (this->music)
		{

			Mix_HaltMusic();
			Mix_FreeMusic(this->music);
			this->music = NULL;

		}

		if (this->enemyDrop)
		{

			delete this->enemyDrop;
			this->enemyDrop = NULL;

		}

	}

	void Level::LoadLevel(std::string fileName)
	{

		std::string data = DATA_DIRECTORY;
		std::string coll = COLLISION_DIRECTORY;
		std::string mast = MASTER_DIRECTORY;
		std::string evnt = EVENT_DIRECTORY;
		std::string render = RENDER_DIRECTORY;

		std::ifstream file(fileName);
		std::string line;

		std::getline(file, line);
		std::istringstream input(line);

		float w, h;

		input >> w;
		input >> h;
		input >> this->lockX;
		input >> this->lockY;

		this->SetWidth(w);
		this->SetHeight(h);

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numSpawnPoints;
		input >> numSpawnPoints;

		for (int i = 0; i < numSpawnPoints; i++)
		{

			std::getline(file, line);
			input.clear();
			input.str(line);

			float x, y;
			input >> x;
			input >> y;

			this->spawnPoints.push_back(Vector2(x, y));

		}

		std::getline(file, line);
		this->LoadCollision(data + coll + line);

		std::getline(file, line);
		this->LoadMaster(data + mast + line);

		std::getline(file, line);
		this->LoadGround(data + render + line);

		std::getline(file, line);
		this->LoadEvents(data + evnt + line);

		std::getline(file, line);
		this->LoadTiles(data + render + line);

		std::getline(file, line);
		this->music = LoadMusic(Settings::AccessSettings()->MusicVolume(), line);

		this->LoadSpawners(&file);

		std::string type;
		std::getline(file, type);

		std::getline(file, line);
		input.clear();
		input.str(line);

		float x, y;
		input >> x;
		input >> y;
		input >> w;
		input >> h;

		this->enemyDrop = new Item(type, x, y, w, h);

		std::string sheetFile;
		std::string sheetImage;
		std::string thumb;

		std::getline(file, sheetFile);
		std::getline(file, sheetImage);
		std::getline(file, thumb);

		this->enemyDrop->Load(sheetFile, sheetImage, thumb);

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> this->dropChance;

	}

	void Level::LoadTiles(std::string fileName)
	{

		std::string base = IMAGE_DIRECTORY;

		std::ifstream file(fileName);

		std::string line;
		std::getline(file, line);

		SDL_Texture* tileset;
		SDL_Rect* tileData;
		int tileWidth, tileHeight;
		int tileSize;
		int* tiles;

		tileset = LoadTexture(base + line);

		std::getline(file, line);
		std::istringstream input(line);

		input >> tileSize;

		//Load Tile Data
		tileWidth = ceil((double)this->Width() / (double)tileSize);
		tileHeight = ceil((double)this->Height() / (double)tileSize);

		int srcW, srcH;
		SDL_QueryTexture(tileset, NULL, NULL, &srcW, &srcH);

		srcW = ceil(srcW / (float)tileSize);
		srcH = ceil(srcH / (float)tileSize);

		tileData = new SDL_Rect[srcW * srcH];

		for (int i = 0; i < srcH; i++)
		{

			for (int j = 0; j < srcW; j++)
			{

				int index = (i * srcW) + j;

				tileData[index].x = tileSize * j;
				tileData[index].y = tileSize * i;
				tileData[index].w = tileSize;
				tileData[index].h = tileSize;

			}

		}

		//Load Tiles
		tiles = new int[tileWidth * tileHeight];

		for (int i = 0; i < tileHeight; i++)
		{

			std::getline(file, line);
			input.clear();
			input.str(line);

			for (int j = 0; j < tileWidth; j++)
			{

				int tile;
				input >> tile;
				tiles[(i * tileWidth) + j] = tile;

			}

		}

		this->tileImage = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->Width(), this->Height());

		SDL_SetRenderTarget(renderer, this->tileImage);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(this->tileImage, SDL_BLENDMODE_BLEND);

		for (int i = 0; i < tileHeight; i++)
		{

			for (int j = 0; j < tileWidth; j++)
			{

				if (tiles[(i * tileWidth) + j] >= 0)
				{

					SDL_Rect src = tileData[tiles[(i * tileWidth) + j]];
					SDL_Rect dest = src;

					dest.x = (j * tileSize);
					dest.y = (i * tileSize);

					SDL_RenderCopyEx(renderer, tileset, &src, &dest, 0, NULL, SDL_FLIP_NONE);

				}

			}

		}

		SDL_SetRenderTarget(renderer, NULL);

		delete tiles;
		tiles = NULL;

		delete tileData;
		tileData = NULL;

		SDL_DestroyTexture(tileset);
		tileset = NULL;

		file.close();

	}

	void Level::LoadCollision(std::string fileName)
	{

		this->collision.LoadCollisionMap(fileName);

	}

	void Level::LoadMaster(std::string fileName)
	{

		std::ifstream file(fileName);

		std::string line;
		std::getline(file, line);

		std::stringstream input(line);

		int numMasterEnemies;
		input >> numMasterEnemies;

		for (int i = 0; i < numMasterEnemies; i++)
		{

			std::getline(file, line);
			this->master[line] = Enemy::EnemyFactory(&file, line, this);

		}

		file.close();

	}

	void Level::LoadEvents(std::string fileName)
	{

		std::ifstream file(fileName);

		std::string line;
		std::getline(file, line);

		std::stringstream input(line);

		int numEvents;
		input >> numEvents;

		std::string name;
		std::getline(file, name);

		for (int i = 0; i < numEvents; i++)
		{

			bool load = !this->platformer->Blocked(name);

			bool nameChanged = false;

			if (load || name.find("Lock") != std::string::npos)
			{

				std::string type;
				std::getline(file, type);

				if (type == "spike")
				{

					std::getline(file, line);
					input.clear();
					input.str(line);

					int damage;
					input >> damage;

					std::getline(file, line);
					input.clear();
					input.str(line);

					int x, y, w, h;
					input >> x;
					input >> y;
					input >> w;
					input >> h;

					Attack* spike = new Attack(NULL, this, damage, x, y, w, h);
					this->platformer->RegisterAttack(spike);

				}
				else if (type == "item")
				{

					std::getline(file, type);

					std::getline(file, line);
					input.clear();
					input.str(line);

					float x, y, w, h;
					input >> x;
					input >> y;
					input >> w;
					input >> h;

					Item* newItem = new Item(type, x, y, w, h);

					std::string sheetFile;
					std::string sheetImage;
					std::string thumb;

					std::getline(file, sheetFile);
					std::getline(file, sheetImage);
					std::getline(file, thumb);

					std::getline(file, line);
					input.clear();
					input.str(line);

					bool block;

					input >> block;

					if (newItem->Type() == "CorpseRun")
					{

						std::string name = SaveData::AccessSaveData()->Defeated();
						sheetImage = sheetImage + name + ".png";

						thumb = name;

					}
					
					newItem->Load(sheetFile, sheetImage, thumb);
					newItem->SetBlock(name, block);

					if (newItem->Type() != "CorpseRun" || thumb.size() > 0)
					{

						if (newItem->Type() == "CorpseRun")
						{

							newItem->SetType("CorpseRun-" + thumb);

						}

						this->platformer->AddItem(newItem);

					}
					else
					{

						delete newItem;
						newItem = NULL;

					}

				}
				else if (type == "trigger")
				{

					if (!load)
					{

						std::getline(file, line);
						std::getline(file, line);

					}

					this->triggers.push_back(Trigger::TriggerFactory(file, this, name));

					if (!load)
					{

						std::getline(file, line);
						input.clear();
						input.str(line);

						bool fore;
						int imageIndex, locIndex;

						input >> fore;
						input >> imageIndex;
						input >> locIndex;

						this->RemoveGround(fore, imageIndex, locIndex);

						while (std::getline(file, name) && name.substr(0, 2) != "N-")
						{

							// Do nothing, this is just to get rid the unloaded event data

						}

						nameChanged = true;

					}

				}
				else if (type == "breakable")
				{

					std::string code;
					std::getline(file, code);

					std::getline(file, line);

					if (this->breakables.find(line) == this->breakables.end())
					{

						Breakable* breakableMaster = new Breakable(code);
						breakableMaster->SetLevel(this);
						breakableMaster->LoadEntity(std::ifstream(line), true);

						this->breakables[line] = breakableMaster;

					}

					Breakable* breakable = new Breakable(code);
					breakable->SetLevel(this);
					this->breakables[line]->DataCopy(breakable);

					std::getline(file, line);
					input.clear();
					input.str(line);

					int x, y, d;
					input >> x;
					input >> y;
					input >> d;

					breakable->SetX(x);
					breakable->SetY(y);
					breakable->SetDirection(d);

					breakable->PlayAnimation(0, true);

					this->platformer->RegisterEntity(breakable);
					this->collision.Register(code, breakable);

				}

				if (!nameChanged)
				{

					std::getline(file, name);

				}

			}
			else
			{

				while (std::getline(file, name) && name.substr(0, 2) != "N-")
				{

					// Do nothing, this is just to get rid the unloaded event data

				}

			}

		}

	}

	void Level::LoadGround(std::string fileName)
	{

		std::ifstream file(fileName);

		std::string line;
		std::getline(file, line);
		std::istringstream input(line);

		int numBackSheets;
		input >> numBackSheets;

		for (int i = 0; i < numBackSheets; i++)
		{

			std::string sheetFile;
			std::string imageFile;

			std::getline(file, sheetFile);
			std::getline(file, imageFile);

			Spritesheet* sheet = new Spritesheet();
			sheet->LoadSpritesheet(sheetFile);
			sheet->LoadSurface(imageFile);

			this->backgroundSheets.push_back(sheet);
			this->backgroundLocations.push_back(std::vector<Vector2>());

			std::getline(file, line);
			input.clear();
			input.str(line);

			int numLocations;
			input >> numLocations;

			for (int j = 0; j < numLocations; j++)
			{

				std::getline(file, line);
				input.clear();
				input.str(line);

				float x, y;
				input >> x;
				input >> y;

				this->backgroundLocations[i].push_back(Vector2(x, y));

			}

			this->backgroundSheets[i]->PlayAnimation(0, true);

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numForeSheets;
		input >> numForeSheets;

		for (int i = 0; i < numForeSheets; i++)
		{

			std::string sheetFile;
			std::string imageFile;

			std::getline(file, sheetFile);
			std::getline(file, imageFile);

			Spritesheet* sheet = new Spritesheet();
			sheet->LoadSpritesheet(sheetFile);
			sheet->LoadSurface(imageFile);

			this->foregroundSheets.push_back(sheet);
			this->foregroundLocations.push_back(std::vector<Vector2>());

			std::getline(file, line);
			input.clear();
			input.str(line);

			int numLocations;
			input >> numLocations;

			for (int j = 0; j < numLocations; j++)
			{

				std::getline(file, line);
				input.clear();
				input.str(line);

				float x, y;
				input >> x;
				input >> y;

				this->foregroundLocations[i].push_back(Vector2(x, y));

			}

			this->foregroundSheets[i]->PlayAnimation(0, true);

		}

		file.close();

	}

	void Level::LoadSpawners(std::ifstream* file)
	{

		int numSpawners;

		std::string line;
		std::getline(*file, line);
		std::istringstream input(line);
	
		input >> numSpawners;

		for (int i = 0; i < numSpawners; i++)
		{

			this->spawners.push_back(Spawner::SpawnerFactory(file));

		}

	}

	void Level::Start()
	{

		Mix_PlayMusic(this->music, -1);

	}

	void Level::Update(std::vector<GameEntity*>& entities, std::vector<Attack*>& attacks, float deltaTime)
	{

		this->collision.Update(entities, deltaTime);

		for (int i = 0; i < this->backgroundSheets.size(); i++)
		{

			this->backgroundSheets[i]->Update(deltaTime);

		}

		for (int i = 0; i < this->foregroundSheets.size(); i++)
		{

			this->foregroundSheets[i]->Update(deltaTime);

		}


		for (int i = 0; i < this->spawners.size(); i++)
		{

			this->spawners[i]->Update(this->master, entities, attacks, deltaTime);

		}

		for (int i = 0; i < this->triggers.size(); i++)
		{

			this->triggers[i]->Update((Player*)entities[0], deltaTime);

		}

		for (int i = 0; i < this->spawnLocations.size(); i++)
		{

			Enemy::Spawn(this->spawnLocations[i].second, this->spawnLocations[i].first.second, this->spawnLocations[i].first.first, this->master, entities, attacks, deltaTime);

		}

		this->spawnLocations.clear();

	}

	Vector2 Level::SpawnPoint(Vector2 point)
	{

		if (this->spawnPoints.size() <= 0)
		{

			return Vector2();

		}

		float minDistance = (this->spawnPoints[0] - point).LengthSquared();
		int minIndex = 0;

		for (int i = 1; i < this->spawnPoints.size(); i++)
		{

			float distance = (this->spawnPoints[i] - point).LengthSquared();
			if (distance < minDistance)
			{

				minDistance = distance;
				minIndex = i;

			}

		}

		return this->spawnPoints[minIndex];

	}

	Vector2 Level::SpawnIndex(int index)
	{

		int i = fmaxf(fminf(index, this->spawnPoints.size() - 1), 0);

		return this->spawnPoints[i];

	}

	void Level::SpawnEnemy(Vector2 loc, std::string code, std::string type)
	{

		this->spawnLocations.push_back(std::pair<std::pair<std::string, std::string>, Vector2>(std::pair<std::string, std::string>(code, type), loc));

	}

	void Level::Pause()
	{

		for (int i = 0; i < this->backgroundSheets.size(); i++)
		{

			this->backgroundSheets[i]->Pause();

		}

		for (int i = 0; i < this->foregroundSheets.size(); i++)
		{

			this->foregroundSheets[i]->Pause();

		}

		for (int i = 0; i < this->spawners.size(); i++)
		{

			this->spawners[i]->Pause();

		}

	}

	void Level::Unpause()
	{

		for (int i = 0; i < this->backgroundSheets.size(); i++)
		{

			this->backgroundSheets[i]->Unpause();

		}

		for (int i = 0; i < this->foregroundSheets.size(); i++)
		{

			this->foregroundSheets[i]->Unpause();

		}

		for (int i = 0; i < this->spawners.size(); i++)
		{

			this->spawners[i]->Unpause();

		}

	}

	int Level::LockX()
	{

		return this->lockX;

	}

	int Level::LockY()
	{

		return this->lockY;

	}

	CollisionMap* Level::Collision()
	{

		return &this->collision;

	}

	Platformer* Level::GetPlatformer()
	{

		return this->platformer;

	}

	void Level::DrawBackground(Vector2 world, float deltaTime)
	{

		if (this->tileImage)
		{

			SDL_Rect dest;
			dest.x = -world.X();
			dest.y = -world.Y();
			dest.w = this->Width();
			dest.h = this->Height();

			SDL_RenderCopyEx(renderer, this->tileImage, NULL, &dest, 0, NULL, SDL_FLIP_NONE);

		}

		for (int i = 0; i < this->backgroundSheets.size(); i++)
		{

			if (this->backgroundSheets[i])
			{

				for (int j = 0; j < this->backgroundLocations[i].size(); j++)
				{

					this->backgroundSheets[i]->Render(this->backgroundLocations[i][j] - Vector2((int)world.X(), (int)world.Y()), false);

				}

			}

		}

		this->collision.Draw(world, deltaTime);

	}

	void Level::DrawForeground(Vector2 world, float deltaTime)
	{

		for (int i = 0; i < this->foregroundSheets.size(); i++)
		{

			if (this->foregroundSheets[i])
			{

				for (int j = 0; j < this->foregroundLocations[i].size(); j++)
				{

					this->foregroundSheets[i]->Render(this->foregroundLocations[i][j] - Vector2((int)world.X(), (int)world.Y()), false);

				}

			}

		}

	}

	void Level::DrawUI(UI* ui, float deltaTime)
	{

		for (int i = 0; i < this->triggers.size(); i++)
		{

			this->triggers[i]->Render(ui, deltaTime);

		}

	}

	void Level::DebugDraw(Vector2 world, float deltaTime)
	{

		this->collision.DebugDraw(world, deltaTime);

		for (int i = 0; i < this->triggers.size(); i++)
		{

			FillAABB(this->triggers[i], world, 200, 200, 0, 32);

		}

	}

	void Level::AddTrigger(Trigger* t)
	{

		this->triggers.push_back(t);

	}

	void Level::Block(std::string n)
	{

		this->platformer->RegisterBlock(n);

	}

	void Level::RemoveGround(bool fore, int image, int location)
	{

		if (image >= 0 && location >= 0)
		{

			if (fore)
			{

				this->foregroundLocations[image].erase(this->foregroundLocations[image].begin() + location);

			}
			else
			{

				this->backgroundLocations[image].erase(this->backgroundLocations[image].begin() + location);

			}

		}

	}

	void Level::EnemyDied(Vector2 loc)
	{

		int roll = RandomBoundedInteger(1, 100);
		if (roll <= this->dropChance)
		{

			this->platformer->AddItem(this->enemyDrop->Clone(loc));
			
		}

	}

	void Level::Refill(int i)
	{

		for (int j = 0; j < i; j++)
		{

			this->platformer->AddToInventory(this->enemyDrop->Clone(Vector2()));

		}

	}

}
