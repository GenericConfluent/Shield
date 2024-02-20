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
        void hit(float damage);
        bool isDead();
        sf::Sprite playerSprite;
        char state;
        float s_yvel;
        float s_jumpheight = 13;
        
    private:
        GameDataRef _data;
        
        float _playerSpeed = 3.0;
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

