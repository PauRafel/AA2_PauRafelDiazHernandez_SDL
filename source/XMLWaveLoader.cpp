#include "XMLWaveLoader.h"
#include "RenderManager.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool XMLWaveLoader::LoadWavesFromXML(std::string xmlPath)
{
    std::cout << "========================================" << std::endl;
    std::cout << "Loading waves from XML: " << xmlPath << std::endl;
    std::cout << "========================================" << std::endl;

    try
    {
        std::ifstream file(xmlPath);
        if (!file.is_open())
        {
            std::cerr << "ERROR: Could not open XML file: " << xmlPath << std::endl;
            return false;
        }

        std::string xmlContent((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        file.close();

        if (xmlContent.empty())
        {
            std::cerr << "ERROR: XML file is empty: " << xmlPath << std::endl;
            return false;
        }

        rapidxml::xml_document<> doc;
        doc.parse<0>(&xmlContent[0]);

        auto* levelNode = doc.first_node("level");
        if (!levelNode)
        {
            std::cerr << "ERROR: No <level> root node found in XML" << std::endl;
            return false;
        }

        int waveNumber = 1;
        int totalEnemiesLoaded = 0;

        for (auto* waveNode = levelNode->first_node("wave");
            waveNode != nullptr;
            waveNode = waveNode->next_sibling("wave"))
        {
            Wave* wave = new Wave(waveNumber);
            int enemiesInThisWave = 0;

            auto* isBossAttr = waveNode->first_attribute("is_boss");
            bool isBossWave = (isBossAttr && std::string(isBossAttr->value()) == "true");

            if (isBossWave)
            {
                std::cout << "Wave " << waveNumber << " [BOSS WAVE]" << std::endl;
            }
            else
            {
                std::cout << "Wave " << waveNumber << std::endl;
            }

            for (auto* enemyNode = waveNode->first_node("enemy");
                enemyNode != nullptr;
                enemyNode = enemyNode->next_sibling("enemy"))
            {
                auto* idAttr = enemyNode->first_attribute("id");
                auto* countAttr = enemyNode->first_attribute("count");

                if (!idAttr || !countAttr)
                {
                    std::cerr << "  WARNING: Enemy node missing 'id' or 'count' attribute, skipping..." << std::endl;
                    continue;
                }

                int enemyID = std::stoi(idAttr->value());
                int enemyCount = std::stoi(countAttr->value());

                EnemyType enemyType = GetEnemyTypeFromID(enemyID);

                float spawnDelayIncrement = 0.0f;
                auto* delayNode = enemyNode->first_node("spawn_delay");
                if (delayNode && delayNode->value())
                {
                    spawnDelayIncrement = std::stof(delayNode->value());
                }

                auto* positionsNode = enemyNode->first_node("positions");
                if (!positionsNode)
                {
                    std::cerr << "  WARNING: No <positions> node found for enemy ID "
                        << enemyID << ", skipping..." << std::endl;
                    continue;
                }

                std::vector<EnemySpawnData> enemySpawns;

                int positionIndex = 0;
                for (auto* posNode = positionsNode->first_node("position");
                    posNode != nullptr;
                    posNode = posNode->next_sibling("position"))
                {
                    auto* xAttr = posNode->first_attribute("x");
                    auto* yAttr = posNode->first_attribute("y");

                    if (!xAttr || !yAttr)
                    {
                        std::cerr << "  WARNING: Position node missing x or y attribute, skipping..." << std::endl;
                        continue;
                    }

                    EnemySpawnData spawnData;
                    spawnData.type = enemyType;
                    spawnData.spawnPosition = ParsePosition(xAttr->value(), yAttr->value());

                    spawnData.spawnDelay = positionIndex * spawnDelayIncrement;

                    ParseOptionalAttributes(posNode, spawnData);

                    enemySpawns.push_back(spawnData);

                    positionIndex++;
                }

                for (const auto& spawnData : enemySpawns)
                {
                    wave->AddEnemy(spawnData);
                    enemiesInThisWave++;
                    totalEnemiesLoaded++;
                }

                std::cout << "  - Enemy ID " << enemyID
                    << " (" << enemySpawns.size() << " spawns, delay: "
                    << spawnDelayIncrement << "s)" << std::endl;
            }

            WAVE_MANAGER._waves.push_back(wave);

            std::cout << "  Total enemies in wave: " << enemiesInThisWave << std::endl;
            std::cout << std::endl;

            waveNumber++;
        }

        std::cout << "========================================" << std::endl;
        std::cout << "XML LOAD SUCCESS!" << std::endl;
        std::cout << "Total waves loaded: " << (waveNumber - 1) << std::endl;
        std::cout << "Total enemies loaded: " << totalEnemiesLoaded << std::endl;
        std::cout << "========================================" << std::endl;

        return true;
    }
    catch (const rapidxml::parse_error& e)
    {
        std::cerr << "========================================" << std::endl;
        std::cerr << "ERROR: RapidXML parse error!" << std::endl;
        std::cerr << "What: " << e.what() << std::endl;
        std::cerr << "Where: " << e.where<char>() << std::endl;
        std::cerr << "========================================" << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "========================================" << std::endl;
        std::cerr << "ERROR: Exception during XML parsing!" << std::endl;
        std::cerr << "What: " << e.what() << std::endl;
        std::cerr << "========================================" << std::endl;
        return false;
    }
}

EnemyType XMLWaveLoader::GetEnemyTypeFromID(int enemyID)
{
    switch (enemyID)
    {
    case 0: return ENEMY_BUBBLE;
    case 1: return ENEMY_HORIZONTAL_MEDUSA;
    case 2: return ENEMY_KILLER_WHALE;
    case 3: return ENEMY_CIRCLER;
    case 4: return ENEMY_VERTICAL_MEDUSA;
    case 5: return ENEMY_BEHOLDER;
    case 6: return ENEMY_CHOMPER;
    case 7: return ENEMY_AMOEBA;
    case 8: return ENEMY_BIO_TITAN_BOSS;

    case 9:  return ENEMY_TORPEDO;
    case 10: return ENEMY_TURBO_CHAINSAW;
    case 11: return ENEMY_ROBO_KRABS;
    case 12: return ENEMY_NUKE;
    case 13: return ENEMY_MISSILE;
    case 14: return ENEMY_DANIELS;
    case 15: return ENEMY_UFO;
    case 16: return ENEMY_ANNOYER;
    case 17: return ENEMY_ANGRYGONS;
    case 18: return ENEMY_SPACE_BOSS;

    default:
        std::cerr << "WARNING: Unknown enemy ID " << enemyID
            << " - defaulting to ENEMY_BUBBLE" << std::endl;
        return ENEMY_BUBBLE; 
    }
}

Vector2 XMLWaveLoader::ParsePosition(const char* xStr, const char* yStr)
{
    try
    {
        float x = std::stof(xStr);
        float y = std::stof(yStr);
        return Vector2(x, y);
    }
    catch (const std::exception& e)
    {
        std::cerr << "WARNING: Error parsing position (" << xStr << ", " << yStr
            << ") - using default (0, 0)" << std::endl;
        return Vector2(0.0f, 0.0f);
    }
}

void XMLWaveLoader::ParseOptionalAttributes(
    rapidxml::xml_node<char>* posNode,
    EnemySpawnData& data)
{
    auto* onCeilingAttr = posNode->first_attribute("on_ceiling");
    if (onCeilingAttr)
    {
        std::string value = onCeilingAttr->value();
        data.onCeiling = (value == "true" || value == "1");
    }
    else
    {
        data.onCeiling = false;
    }

    auto* startAngleAttr = posNode->first_attribute("start_angle");
    if (startAngleAttr)
    {
        try
        {
            data.startAngle = std::stof(startAngleAttr->value());
        }
        catch (const std::exception& e)
        {
            std::cerr << "WARNING: Invalid start_angle value, using 0.0" << std::endl;
            data.startAngle = 0.0f;
        }
    }
    else
    {
        data.startAngle = 0.0f;
    }

    data.playerPos = nullptr;
}