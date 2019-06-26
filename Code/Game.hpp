//
//  Game.hpp
//  Simple game
//
//  Created by M Power on 2019-05-06.
//  Copyright © 2019 M Power. All rights reserved.
//
#pragma once

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AssetManager.hpp"
#include "StateMachine.hpp"
#include "InputManager.hpp"

namespace  mp{
    struct GameData{
        sf::RenderWindow window;
        sf::Sprite cursor;
        AssetManager assets;
        StateMachine machine;
        InputManager input;
        sf::Music backgroundMusic;
    };

    typedef std::shared_ptr<GameData> GameDataRef;
    class Game{
    public:
        Game(std::string title);
        
    private:
        GameDataRef _data = std::make_shared<GameData>();
        
        
        const float dt = 1.0f / 60.0f;// Update runs at 60 frames per second
        sf::Clock _clock;
        
        void Run();
    };
}

