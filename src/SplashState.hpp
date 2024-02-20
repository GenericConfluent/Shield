//
//  SplashState.hpp
//  Simple game
//
//  Created by M Power on 2019-05-07.
//  Copyright © 2019 M Power. All rights reserved.
//

#pragma once
#include <stdio.h>
#include "State.hpp"
#include "Game.hpp"

namespace mp {
    class SplashState : public State {
    public:
        SplashState(GameDataRef data);
        
        void init();
        
        void handle_input();
        void update(float dt);
        void draw(float dt);
        
    private:
        GameDataRef data;
        sf::Clock clock;
        sf::Sprite background;
    };
}

