#pragma once
#include "WaveManager.h"
#include <string>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

class XMLWaveLoader
{
public:
    static bool LoadWavesFromXML(std::string xmlPath);

private:
    static EnemyType GetEnemyTypeFromID(int enemyID);
    static Vector2 ParsePosition(const char* xStr, const char* yStr);
    static void ParseOptionalAttributes(
        rapidxml::xml_node<char>* posNode,
        EnemySpawnData& data
    );
};