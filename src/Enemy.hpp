//
//  Enemy.hpp
//  Simple game
//
//  Created by M Power on 2019-05-11.
//  Copyright © 2019 M Power. All rights reserved.
//

#pragma once


#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include "GameState.hpp"

namespace mp {
    
    class Enemy{
    public:
        Enemy(GameDataRef data, sf::Vector2f position);
        ~Enemy();
        
        void draw();
        void update(float _slowMotion, float _time);
        bool isDead();
        
        sf::Sprite _enemyBody;
        
    private:
        GameDataRef _data;
    };
}

