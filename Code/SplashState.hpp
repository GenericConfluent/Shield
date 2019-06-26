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
    class SplashState : public State{
    public:
        SplashState(GameDataRef data);
        
        void Init();
        
        void HandleInput();
        void Update(float dt);
        void Draw(float dt);
        
    private:
        GameDataRef _data;
        
        sf::Clock _clock;
        
        sf::Sprite _background;
        
    };
}

