#pragma once

//Includes
#include <SDL.h>
#include <string>

//Custom Includes
#include "Items.h"

#define REPUTATION_LEVEL_SIZE 1000
#define DEATH_REPUTATION_LOSS 1000

#define SAVE_DIRECTORY "./Saves/"

#define INVULNERABILITY_PER_LEVEL 150

namespace Platformer
{

	class Player;

	struct LevelData
	{

		int numDamsels;
		int numDossiers;

		int reputationForCompletion;

		std::string name;
		std::string shorthand;
		std::string startLevel;
		int startSpawn;

	};

	class SaveData
	{

	//Methods

	public:

		static SaveData* AccessSaveData();
		static void LoadSaveData(std::string fileName);

		static std::vector<std::string> Characters();
		static void Rescue(std::string name);
		static bool Exists(std::string name);

		std::string Defeated();

		int GetReputation();
		int GetAtRiskReputation();
		int GetMaxReputation();

		void AddAtRisk(int r);
		void ClearAtRisk();

		void ProcessItem(Item* i);
		void ExitLevel(std::string level);
		void GameOver();

		int HairIndex();
		int CostumeIndex();
		SDL_Color HairColor();
		std::string Name();

		void SetHairIndex(int i);
		void SetCostumeIndex(int c);
		void SetHairColor(SDL_Color c);
		void SetName(std::string n);

		SDL_Texture* EscapeHairFileName();

		void ExportSaveData();

		int InfoByType(std::string t);

		LevelData CurrentLevel();
		void SetCurrentLevel(LevelData d);

		std::string DefeatedLevel();

		bool CompletedLevel(std::string name);
		int DamselData(std::string name);
		int DossierData(std::string name);

		int NumDamsels();
		int NumDossiers();
		int NumInfo();

		void AugmentPlayer(Player* p);

		int AvailablePoints();
		int HealthBonus();
		int InvulnBonus();
		int EscapeBonus();

		int AddHealthBonus(int h);
		int AddInvulnBonus(int i);
		int AddEscapeBonus(int e);

		std::string WeaponImageName();
		std::string CurrentWeapon();
		void SetCurrentWeapon(std::string w);

		void Clear();

		int WeaponUnlockCost(std::string w);

	protected:

		SaveData();

		void LoadCharacter(std::string fileName);
		void LoadProgress();

		void ExportCharacter();
		void ExportProgress();

	//Data

	public:

	protected:

		const static std::string EscapeHair[5];
		static SaveData* singleton;

		int reputation = 0;
		int atRiskRep = 0;
		int maxReputation = 0;

		int savedDamsels = 0;
		int collectedDossiers = 0;
		int collectedInfo = 0;

		std::string name;

		int hairIndex;
		int costumeIndex;
		SDL_Color hairColor;

		std::map<std::string, int> infoCounts;
		std::map<std::string, std::pair<int, int>> completionRecords;

		LevelData currentLevel;
		std::string defeatedLevel;

		int healthBonus = 0;
		int invulnBonus = 0;
		int escapeBonus = 0;

		std::string currentWeapon;

		std::map<std::string, std::pair<int, bool>> weaponData;

	};

}