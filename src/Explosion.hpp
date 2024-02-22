#pragma once
#include "Game.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics.hpp>

namespace shield {
    class Explosions : public sf::Sprite {
    public:
        Explosions(GameDataRef data);
        
        void reset();
        void tick();
        
        bool hasPlayed();
        
        Explosions& operator= (const Explosions &explosion);
        std::vector<sf::Vector2f> inner = {};

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        const int _size = 512;
        std::shared_ptr<sf::Texture> texture;
        sf::IntRect _visible;
    };
}

