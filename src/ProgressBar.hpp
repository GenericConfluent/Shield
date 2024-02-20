#pragma once
#include "Game.hpp"
#include <SFML/Graphics.hpp>

namespace shield {
    class ProgressBar {
    public:
        ProgressBar(GameDataRef data);
        
        void setValue(int percent);
        void setPosition(sf::Vector2f position);
        void draw();
        
    private:
        GameDataRef _data;
        sf::Sprite _bar;
    };
}

