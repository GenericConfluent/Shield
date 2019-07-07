//
//  PowerUps.hpp
//  Shield
//
//  Created by M Power on 2019-06-27.
//  Copyright © 2019 M Power. All rights reserved.
//
#include <stdio.h>
#include <vector>
#include <SFML/Graphics.hpp>
#include <utility>
#include <random>
#include "Game.hpp"


namespace mp {
    class PowerUp{
    public:
        const static int TIMESTOP = 0;
        const static int INFINITESHIELD = 1;
        const static int BOMB = 2;
        inline constexpr static float PowerUpDuration[] = {8.0, 10.0, 0.5};
    };
    
    class PowerUpManager {
    public:
        PowerUpManager(GameDataRef data, sf::Sprite & player);
        
        
        void update();
        void draw();
        
        float getGameSpeed();
        bool infiniteShield();
        bool bomb();
    private:
        
        float _slowMotion = 1.0;
        bool _infiniteShield = false;
        bool _bomb = false;
        
        sf::Vector2f f_GetRandomPositionOnWindow(sf::Vector2f position);
        std::pair<bool, int> f_CheckPowerUpExists(int powerUp);
        void f_CheckCollisionsWithPlayer();
        void f_ApplyBuffs();
        void f_RemoveBuffs(int buffType);
        
        std::random_device rd;  //Will be used to obtain a seed for the random device

        GameDataRef _data;
        
        sf::Sprite * _playerRef;
        
        float _buffSpawnFrequency= 14.5;
        sf::Clock _buffSpawnClock;
        std::vector<std::pair<int, sf::Sprite>> _onFieldPowerUps;
        std::vector<std::pair<int, sf::Clock>> _currentBuffs;
    };
}
