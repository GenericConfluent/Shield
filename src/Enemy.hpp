#pragma once
#include <SFML/Graphics.hpp>
#include "Game.hpp"
#include "GameState.hpp"

namespace shield {
    class Enemy {
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

