#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "Enemy.hpp"

namespace rat
{
    class GUI;
    class EnemyCodex
    {
    public:
        EnemyCodex();

        void loadFromJson(nlohmann::json& j);

        void initAssetsViaGUI(GUI& gui);

        auto begin() const { return _enemies.begin(); }
        auto end() const { return _enemies.end(); }

        const Enemy* getEnemy(const std::string& nameID);
    private:
        std::unordered_map<std::string, std::unique_ptr<Enemy>> _enemies;

        void _testInit();
        void _addEnemy(const std::string& name, const std::string& path, int hp);

        std::string _iconPaths{"Assets/PrepScreen/Enemies/"};
    };
}