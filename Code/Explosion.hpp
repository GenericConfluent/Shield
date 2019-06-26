//
//  Explosion.hpp
//  Shield
//
//  Created by M Power on 2019-06-26.
//  Copyright © 2019 M Power. All rights reserved.
//

#pragma once
#include "Game.hpp"
#include <SFML/Graphics.hpp>

namespace mp {
    class Explosion{
    public:
        Explosion(GameDataRef data);
        
        void reset();
        void tick();
        void draw();
        
        void setPosition(sf::Vector2f position);
        bool hasPlayed();
    private:
        GameDataRef _data;
        
        bool _done = false;
        
        const int _size = 512;
        
        sf::IntRect _visible;
        sf::Sprite _spriteSheet;
    };
    
}
