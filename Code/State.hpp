//
//  State.hpp
//  Simple game
//
//  Created by M Power on 2019-05-07.
//  Copyright © 2019 M Power. All rights reserved.
//
#pragma once
#include <stdio.h>


namespace mp {
    class State{
    public:
        virtual void Init() = 0;
        
        virtual void HandleInput() = 0;
        virtual void Update(float dt) = 0;
        virtual void Draw(float dt) = 0;
        
        virtual void Pause() {}
        virtual void Resume() {}
    };
}
