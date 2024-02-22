#pragma once
#include <SFML/Graphics.hpp>
#include <utility>
#include "Game.hpp"

namespace shield {
    class Player {
    public:
        Player(GameDataRef data);
        ~Player();
        
        void draw();
        void update();
        float getSpeed();
        void attack();
        void kill();
        bool isDead();
        sf::Sprite playerSprite;
        char state;
        
    private:
        GameDataRef _data;
        float _playerSpeed = 9.0;
        bool dead = false;
    };
    
    class Ghost {
    public:
        Ghost( GameDataRef data);
        void spawn(sf::Vector2f position);
        void update();
        void draw();
        
    private:
        GameDataRef _data;
        sf::Clock _timer;
        std::vector<std::pair<sf::Sprite, float>> _ghosts;
        
        const int _baselineFade = 200; // Must be divisible by the fadeing tick
        float fadingTick = 25;
    };
}

