//
//  InputManager.hpp
//  Simple game
//
//  Created by M Power on 2019-05-07.
//  Copyright © 2019 M Power. All rights reserved.
//

#pragma once
#include <stdio.h>
#include <SFML/Graphics.hpp>

namespace mp {
    class InputManager
    {
    public:
        InputManager() {}
        ~InputManager() {}
        
        bool IsSpriteClicked(sf::Sprite object, sf::Mouse::Button button, sf::RenderWindow &window);
        
        sf::Vector2i GetMousePosition(sf::RenderWindow &window);
    };
}
