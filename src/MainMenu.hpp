#pragma once

#include <SFML/Graphics.hpp>
#include "State.hpp"
#include "Game.hpp"

namespace shield {
    struct guiTransitionOut {
        sf::Vector2f titlePos;
        sf::Vector2f linePos;
    };
    
    class MainMenuState : public State
    {
    public:
        MainMenuState(GameDataRef data);
        
        void init();
        
        void handle_input();
        void update(float dt);
        void draw(float dt);
        
    private:
        GameDataRef _data;
        
        sf::Text title;
        sf::Text heading;
        sf::RectangleShape line;
        
        sf::Text aboutheading;
        sf::RectangleShape aboutline;
        
        sf::Text leaveheading;
        sf::RectangleShape leaveline;
        
        sf::Sprite background;
        
        guiTransitionOut Out;
        float _transitionSpeed = 15.0;
        bool transition = false;
    };
}

