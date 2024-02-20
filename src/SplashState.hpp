#pragma once
#include "State.hpp"
#include "Game.hpp"

namespace shield {
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

