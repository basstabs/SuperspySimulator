//Includes
#include <fstream>
#include <sstream>
#include <filesystem>

//Custom Includes
#include "Helpers.h"
#include "SaveData.h"
#include "Settings.h"
#include "Player.h"

namespace Platformer
{

	int Triangle(int n)
	{

		return (n * (n + 1)) / 2;

	}

	std::string escapeHairBase = "./Assets/Images/Pieces/";
	const std::string SaveData::EscapeHair[5] = {"EscapeBarPixie.png", "EscapeBarSpiky.png" , "EscapeBarPonytail.png", "EscapeBarBob.png", "EscapeBarStraight.png"};

	SaveData* SaveData::singleton = NULL;

	SaveData::SaveData()
	{

		this->reputation = this->maxReputation = this->atRiskRep = 0;

		this->name = "Genny";

		this->hairIndex = this->costumeIndex = 0;
		this->hairColor.r = this->hairColor.g = this->hairColor.b = this->hairColor.a = 255;

	}

	SaveData* SaveData::AccessSaveData()
	{

		if (singleton)
		{

			return singleton;

		}

		singleton = new SaveData();
		return singleton;

	}

	std::vector<std::string> SaveData::Characters()
	{

		std::vector<std::string> characters;

		for (const auto& entry : std::experimental::filesystem::directory_iterator(SAVE_DIRECTORY))
		{

			if (entry.path().extension().string() == ".plSAV")
			{

				std::string name = entry.path().filename().string();
				characters.push_back(name.substr(0, name.find(".")));

			}

		}

		return characters;

	}

	void SaveData::Rescue(std::string name)
	{

		SaveData data;
		data.LoadCharacter(SAVE_DIRECTORY + name + ".plSAV");
		data.defeatedLevel = "";
		data.ExportCharacter();

		singleton->reputation += DEATH_REPUTATION_LOSS;

	}

	std::string SaveData::Defeated()
	{

		std::vector<std::string> characters = SaveData::Characters();

		for (int i = 0; i < characters.size(); i++)
		{

			SaveData data;
			data.LoadCharacter(SAVE_DIRECTORY + characters[i] + ".plSAV");

			if (data.defeatedLevel == singleton->CurrentLevel().shorthand)
			{

				return data.Name();

			}

		}

		return "";

	}

	void SaveData::LoadSaveData(std::string fileName)
	{

		SaveData* data = SaveData::AccessSaveData();

		data->LoadCharacter(fileName);
		data->LoadProgress();

	}

	void SaveData::LoadCharacter(std::string fileName)
	{

		std::ifstream file(fileName);

		std::string line;
		std::getline(file, this->name);
		std::getline(file, line);

		std::istringstream input(line);
		
		input >> this->hairIndex;
		input >> this->costumeIndex;

		int r, g, b;

		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> r;
		input >> g;
		input >> b;

		this->hairColor.r = r;
		this->hairColor.g = g;
		this->hairColor.b = b;
		
		std::getline(file, line);
		input.clear();
		input.str(line);

		input >> this->healthBonus;
		input >> this->invulnBonus;
		input >> this->escapeBonus;

		std::getline(file, this->currentWeapon);

		std::getline(file, this->defeatedLevel);

		file.close();

	}

	void SaveData::LoadProgress()
	{

		std::string fileName = SAVE_DIRECTORY;
		fileName += "Progress.plPRG";
		std::ifstream file(fileName);

		std::string line;
		std::getline(file, line);
		std::istringstream input(line);

		input >> this->reputation;
		input >> this->maxReputation;
		this->maxReputation = fmaxf(this->reputation, this->maxReputation);

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numWeapons;
		input >> numWeapons;

		for (int i = 0; i < numWeapons; i++)
		{

			std::getline(file, line);
			input.clear();
			input.str(line);

			std::string weapon;
			int cost;
			bool unlocked;

			input >> weapon;
			input >> cost;
			input >> unlocked;

			this->weaponData[weapon] = std::pair<int, bool>(cost, unlocked);

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numInfo;
		input >> numInfo;

		for (int i = 0; i < numInfo; i++)
		{

			std::string info;
			std::getline(file, info);

			std::getline(file, line);
			input.clear();
			input.str(line);

			int num;
			input >> num;

			this->infoCounts[info] = num;

		}

		std::getline(file, line);
		input.clear();
		input.str(line);

		int numCompletion;
		input >> numCompletion;
		for (int i = 0; i < numCompletion; i++)
		{

			std::string name;
			std::getline(file, name);

			std::getline(file, line);
			input.clear();
			input.str(line);

			int damsel, dossier;
			input >> damsel;
			input >> dossier;

			this->completionRecords[name] = std::pair<int, int>(damsel, dossier);

		}

		file.close();

	}

	int SaveData::GetReputation()
	{

		return this->reputation;

	}

	int SaveData::GetAtRiskReputation()
	{

		return this->atRiskRep;

	}

	int SaveData::GetMaxReputation()
	{
		
		this->maxReputation = fmaxf(this->reputation, this->maxReputation);
		return this->maxReputation;

	}

	void SaveData::AddAtRisk(int r)
	{

		this->atRiskRep += r;

	}

	void SaveData::ClearAtRisk()
	{

		this->atRiskRep = 0;

	}

	void SaveData::ProcessItem(Item* i)
	{

		this->reputation += Settings::AccessSettings()->ReputationByType(i->Type());

		if (i->Type() == "Dossier")
		{

			this->collectedDossiers++;

		}
		else if (i->Type() == "Damsel")
		{

			this->savedDamsels++;

		}

		if (i->Type().find("Info") != std::string::npos)
		{

			this->collectedInfo++;
			this->infoCounts[i->Type()]++;

		}

	}

	int SaveData::NumDamsels()
	{

		return this->savedDamsels;

	}

	int SaveData::NumDossiers()
	{

		return this->collectedDossiers;

	}

	int SaveData::NumInfo()
	{

		return this->collectedInfo;

	}

	void SaveData::ExitLevel(std::string level)
	{

		this->faux.clear();

		this->reputation += this->atRiskRep + this->currentLevel.reputationForCompletion;
		this->atRiskRep = 0;

		std::map<std::string, std::pair<int, int>>::iterator it = this->completionRecords.find(this->currentLevel.shorthand);
		if (it != this->completionRecords.end())
		{

			if (it->second.first > this->savedDamsels || (this->savedDamsels == it->second.first && it->second.second > this->collectedDossiers))
			{

				this->savedDamsels = it->second.first;
				this->collectedDossiers = it->second.second;

			}

		}

		this->completionRecords[this->currentLevel.shorthand] = std::pair<int, int>(this->savedDamsels, this->collectedDossiers);

		this->savedDamsels = this->collectedDossiers = this->collectedInfo = 0;

		this->ExportSaveData();

	}

	void SaveData::GameOver()
	{

		this->faux.clear();

		this->atRiskRep = 0;
		this->reputation = fmaxf(0, this->reputation - DEATH_REPUTATION_LOSS);

		this->defeatedLevel = this->currentLevel.shorthand;

		this->ExportSaveData();

		this->Clear();

	}

	int SaveData::HairIndex()
	{

		return this->hairIndex;

	}

	int SaveData::CostumeIndex()
	{

		return this->costumeIndex;

	}

	SDL_Color SaveData::HairColor()
	{

		return this->hairColor;

	}

	std::string SaveData::Name()
	{

		return this->name;

	}

	void SaveData::SetHairIndex(int i)
	{

		this->hairIndex = i;

	}

	void SaveData::SetCostumeIndex(int i)
	{

		this->costumeIndex = i;

	}

	void SaveData::SetHairColor(SDL_Color c)
	{

		this->hairColor = c;

	}

	void SaveData::SetName(std::string n)
	{

		if (this->name != n)
		{

			std::string fileName = "./Saves/" + this->name + ".plSAV";
			remove(fileName.c_str());

		}

		this->name = n;

	}

	SDL_Texture* SaveData::EscapeHairFileName()
	{

		SDL_Texture* hair = LoadTexture(escapeHairBase + (this->currentLevel.shorthand == "Tutorial" ? "Tropic" : this->currentLevel.shorthand) + SaveData::EscapeHair[this->hairIndex]);
		SDL_SetTextureColorMod(hair, this->hairColor.r, this->hairColor.g, this->hairColor.b);

		return hair;

	}

	void SaveData::ExportSaveData()
	{

		std::string last = SAVE_DIRECTORY;
		last += "LastSave.txt";
		std::ofstream lastSave(last, std::ios::trunc);
		lastSave << this->name;
		lastSave.close();

		this->ExportCharacter();
		this->ExportProgress();

	}

	void SaveData::ExportCharacter()
	{

		std::ofstream file(SAVE_DIRECTORY + this->name + ".plSAV", std::ios::trunc);

		file << this->name << std::endl;
		file << this->HairIndex() << " " << this->CostumeIndex() << std::endl;
		file << (int)this->HairColor().r << " " << (int)this->HairColor().g << " " << (int)this->HairColor().b << std::endl;

		file << this->healthBonus << " " << this->invulnBonus << " " << this->escapeBonus << std::endl;

		file << this->currentWeapon << std::endl;

		file << this->defeatedLevel << std::endl;

		file.close();

	}

	void SaveData::ExportProgress()
	{

		std::string fileName = SAVE_DIRECTORY;
		fileName += "Progress.plPRG";
		std::ofstream file(fileName);

		file << this->GetReputation() << " " << this->GetMaxReputation() << std::endl;

		file << this->weaponData.size() << std::endl;

		std::map<std::string, std::pair<int, bool>>::iterator i;
		for (i = this->weaponData.begin(); i != this->weaponData.end(); i++)
		{

			file << i->first << " " << i->second.first << " " << i->second.second << std::endl;

		}

		file << this->infoCounts.size() << std::endl;

		std::map<std::string, int>::iterator it;
		for (it = this->infoCounts.begin(); it != this->infoCounts.end(); it++)
		{

			file << it->first << std::endl;
			file << it->second << std::endl;

		}
		
		file << this->completionRecords.size() << std::endl;

		std::map<std::string, std::pair<int, int>>::iterator iter;
		for (iter = this->completionRecords.begin(); iter != this->completionRecords.end(); iter++)
		{

			file << iter->first << std::endl;
			file << iter->second.first << " " << iter->second.second << std::endl;

		}

		file.close();

	}

	int SaveData::InfoByType(std::string t)
	{

		return this->infoCounts[t];

	}

	LevelData SaveData::CurrentLevel()
	{

		return this->currentLevel;

	}

	void SaveData::SetCurrentLevel(LevelData d)
	{

		this->currentLevel = d;

	}

	bool SaveData::CompletedLevel(std::string name)
	{

		std::map<std::string, std::pair<int, int>>::iterator it = this->completionRecords.find(name);
		return it != this->completionRecords.end();

	}

	int SaveData::DamselData(std::string name)
	{

		return (this->completionRecords.find(name) == this->completionRecords.end() ? 0 : this->completionRecords[name].first);

	}

	int SaveData::DossierData(std::string name)
	{

		return (this->completionRecords.find(name) == this->completionRecords.end() ? 0 : this->completionRecords[name].second);

	}

	void SaveData::AugmentPlayer(Player* p)
	{

		p->maxHealth += this->healthBonus;
		p->hitInvulnerability += this->invulnBonus * INVULNERABILITY_PER_LEVEL;

		if (this->currentWeapon.size() > 0)
		{

			delete p->weapon;

			std::ifstream weapon("./Assets/Data/Master/" + this->currentWeapon + ".plWPN");
			p->weapon = Weapon::LoadWeapon(weapon, p);
			weapon.close();

		}

	}

	int SaveData::AvailablePoints()
	{

		int weaponPoints = 0;

		std::map<std::string, std::pair<int, bool>>::iterator it;
		for (it = this->weaponData.begin(); it != this->weaponData.end(); it++)
		{

			if (it->second.second)
			{

				weaponPoints += it->second.first;

			}

		}

		return (this->GetMaxReputation() / REPUTATION_LEVEL_SIZE) - (Triangle(this->healthBonus) + Triangle(this->invulnBonus) + Triangle(this->escapeBonus) + weaponPoints);

	}

	int SaveData::HealthBonus()
	{

		return this->healthBonus;

	}

	int SaveData::InvulnBonus()
	{

		return this->invulnBonus;

	}

	int SaveData::EscapeBonus()
	{

		return this->escapeBonus;

	}

	int SaveData::AddHealthBonus(int h)
	{

		return this->healthBonus += h;

	}

	int SaveData::AddInvulnBonus(int i)
	{

		return this->invulnBonus += i;

	}

	int SaveData::AddEscapeBonus(int e)
	{

		return this->escapeBonus += e;

	}

	std::string SaveData::WeaponImageName()
	{

		if (this->currentWeapon.size() > 0)
		{

			return this->currentWeapon + "Weapon.png";

		}

		return "BaseWeapon.png";

	}

	std::string SaveData::CurrentWeapon()
	{

		if (this->currentWeapon.size() > 0)
		{

			return this->currentWeapon;

		}

		return "Pistol";

	}

	void SaveData::SetCurrentWeapon(std::string w)
	{

		this->currentWeapon = w;

		if (w.size() > 0)
		{

			this->weaponData[w].second = true;

		}

	}

	int SaveData::WeaponUnlockCost(std::string w)
	{

		if (this->weaponData[w].second)
		{

			return 0;

		}

		return this->weaponData[w].first;

	}

	void SaveData::Clear()
	{

		this->name = "";

		this->healthBonus = 0;
		this->invulnBonus = 0;
		this->escapeBonus = 0;

		this->currentWeapon = "";
		this->defeatedLevel = "";

	}

	bool SaveData::Exists(std::string name)
	{

		std::ifstream file(SAVE_DIRECTORY + name + ".plSAV");
		return file.good();

	}

	std::string SaveData::DefeatedLevel()
	{

		return this->defeatedLevel;

	}

	bool SaveData::CollectedDamsels()
	{

		std::ifstream file("./Saves/Levels.plLVS");

		std::string line;
		std::getline(file, line);
		std::istringstream input(line);

		int numLevels;
		input >> numLevels;

		for (int i = 0; i < numLevels; i++)
		{

			int numDamsels;
			std::string name;

			std::getline(file, line);
			input.clear();
			input.str(line);

			input >> numDamsels;

			//Read through unnecessary data
			std::getline(file, line);
			
			std::getline(file, name);
			
			if (this->DamselData(name) < numDamsels)
			{

				file.close();
				return false;

			}

			//Read through unnecessary data
			std::getline(file, line);
			std::getline(file, line);
			std::getline(file, line);
		
		}

		file.close();

		return true;

	}

	bool SaveData::Completed()
	{

		std::ifstream file("./Saves/Levels.plLVS");

		std::string line;
		std::getline(file, line);
		std::istringstream input(line);

		int numLevels;
		input >> numLevels;

		for (int i = 0; i < numLevels; i++)
		{

			int numDamsels, numDossiers;
			std::string name;

			std::getline(file, line);
			input.clear();
			input.str(line);

			input >> numDamsels;
			input >> numDossiers;

			//Read through unnecessary data
			std::getline(file, line);

			std::getline(file, name);

			if (this->DamselData(name) < numDamsels || this->DossierData(name) < numDossiers)
			{

				file.close();
				return false;

			}

			//Read through unnecessary data
			std::getline(file, line);
			std::getline(file, line);
			std::getline(file, line);

		}

		file.close();

		return true;

	}

	void SaveData::SetFaux(std::vector<bool> f)
	{

		this->faux = f;

	}

	bool SaveData::Faux(int index)
	{

		if (index >= 0 && index < this->faux.size())
		{

			return this->faux[index];

		}

		return false;

	}

}