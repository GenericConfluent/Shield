#include "SplashState.hpp"
#include "MainMenu.hpp"
#include <bits/utility.h>
#include <cstring>
#include <iostream>
#include <array>
#include <format>

namespace shield {
    SplashState::SplashState(GameDataRef data) : data (data) {}
    
    void SplashState::init(){
        this->data->assets.load<sf::Texture>("SplashState:Logo", "assets/Images/logo.png");

        this->data->assets.load<sf::Texture>("Explosion", "assets/Images/explosion.png");
        this->data->assets.load<sf::Texture>("TimeStop", "assets/Images/TimeStop.png");
        this->data->assets.load<sf::Texture>("UnlimShield", "assets/Images/UnlimShield.png");
        this->data->assets.load<sf::Texture>("BOMB", "assets/Images/bomb.png");
        
        background.setTexture(*this->data->assets.get<sf::Texture>("SplashState:Logo"));
        
        sf::Vector2f windowSize(this->data->window.getSize());
        background.setOrigin(background.getLocalBounds().width/2,background.getLocalBounds().height/2);
        background.setPosition((float)(windowSize.x/2), (float)(windowSize.y/2));
    }
    
    void SplashState::handle_input(){
        sf::Event event;
        while (this->data->window.pollEvent(event)) {
            if (sf::Event::Closed == event.type || 
                (sf::Event::KeyPressed == event.type && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            ) {
                this->data->window.close();
            }
        }
    }
    
    void SplashState::update(float dt){
        if (this->clock.getElapsedTime().asSeconds() > 2.0) {
            this->data->machine.AddState(StateRef(new MainMenuState(data)), true);
        }
    }
    
    void SplashState::draw(float dt){
        this->data->window.clear(sf::Color(50,50,50));
        this->data->window.draw(this->background);
        this->data->window.display();
    }
}

