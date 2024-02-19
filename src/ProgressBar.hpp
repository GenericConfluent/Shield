//
//  ProgressBar.hpp
//  Shield
//
//  Created by M Power on 2019-06-25.
//  Copyright © 2019 M Power. All rights reserved.
//

#pragma once
#include "Game.hpp"
#include <SFML/Graphics.hpp>

namespace mp {
    class ProgressBar{
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
