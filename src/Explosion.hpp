#pragma once
#include "Game.hpp"
#include <SFML/Graphics.hpp>

namespace shield {
    class Explosion{
    public:
        Explosion(GameDataRef data);
        
        void reset();
        void tick();
        
        void setPosition(sf::Vector2f position);
        bool hasPlayed();
        
        Explosion& operator= (const Explosion &explosion);
        
        sf::Sprite _spriteSheet;

    private:
        bool _done = false;
        
        GameDataRef _data;
        
        const int _size = 512;
        sf::Texture texture;
        sf::IntRect _visible;
    };
}

