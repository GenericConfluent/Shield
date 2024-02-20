#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "AssetManager.hpp"
#include "StateMachine.hpp"
#include "InputManager.hpp"

namespace shield {
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
        void run();
        
    private:
        GameDataRef _data = std::make_shared<GameData>();
        
        constexpr static float dt = 1.0f / 60.0f; // 60 fps
        sf::Clock _clock;
    };
}

