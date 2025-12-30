#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>

#define RANKING RankingManager::Instance()

struct RankingEntry
{
    char name[9];
    int score;

    RankingEntry() : score(0)
    {
        for (int i = 0; i < 9; i++)
            name[i] = '\0';
    }

    RankingEntry(std::string playerName, int playerScore) : score(playerScore)
    {
        int length = playerName.length() > 8 ? 8 : playerName.length();
        for (int i = 0; i < length; i++)
            name[i] = playerName[i];
        for (int i = length; i < 9; i++)
            name[i] = '\0';
    }

    std::string GetName() const
    {
        return std::string(name);
    }
};

class RankingManager
{
private:
    RankingManager()
    {
        LoadFromFile();
    }

    ~RankingManager()
    {
        SaveToFile();
    }

    RankingManager(RankingManager&) = delete;
    RankingManager& operator=(const RankingManager&) = delete;

    std::vector<RankingEntry> _rankings;
    const int MAX_ENTRIES = 10;
    const std::string FILENAME = "ranking.dat";

public:
    static RankingManager& Instance()
    {
        static RankingManager instance;
        return instance;
    }

    bool AddScore(std::string playerName, int score)
    {
        RankingEntry newEntry(playerName, score);

        _rankings.push_back(newEntry);

        std::sort(_rankings.begin(), _rankings.end(),
            [](const RankingEntry& a, const RankingEntry& b) {
                return a.score > b.score;
            });

        if (_rankings.size() > MAX_ENTRIES)
        {
            _rankings.resize(MAX_ENTRIES);
        }

        SaveToFile();

        std::cout << "Score added: " << playerName << " - " << score << std::endl;
        return true;
    }

    std::vector<RankingEntry> GetTopScores() const
    {
        return _rankings;
    }

    bool IsTopScore(int score) const
    {
        if (_rankings.size() < MAX_ENTRIES)
            return true;

        return score > _rankings[MAX_ENTRIES - 1].score;
    }

    void SaveToFile()
    {
        std::ofstream file(FILENAME, std::ios::binary);

        if (!file.is_open())
        {
            std::cout << "ERROR: Could not open file for writing: " << FILENAME << std::endl;
            return;
        }

        int count = _rankings.size();
        file.write((char*)&count, sizeof(int));

        for (const RankingEntry& entry : _rankings)
        {
            file.write((char*)&entry, sizeof(RankingEntry));
        }

        file.close();
        std::cout << "Ranking saved to file: " << FILENAME << std::endl;
    }

    void LoadFromFile()
    {
        std::ifstream file(FILENAME, std::ios::binary);

        if (!file.is_open())
        {
            std::cout << "Ranking file not found. Creating new ranking..." << std::endl;
            return;
        }

        _rankings.clear();

        int count = 0;
        file.read((char*)&count, sizeof(int));

        for (int i = 0; i < count; i++)
        {
            RankingEntry entry;
            file.read((char*)&entry, sizeof(RankingEntry));
            _rankings.push_back(entry);
        }

        file.close();
        std::cout << "Ranking loaded from file: " << count << " entries" << std::endl;
    }

    void Reset()
    {
        _rankings.clear();
        SaveToFile();
        std::cout << "Ranking reset!" << std::endl;
    }

    void PrintRanking() const
    {
        std::cout << "=== TOP 10 RANKING ===" << std::endl;
        for (int i = 0; i < _rankings.size(); i++)
        {
            std::cout << (i + 1) << ". " << _rankings[i].GetName()
                << " - " << _rankings[i].score << std::endl;
        }
        std::cout << "======================" << std::endl;
    }
};