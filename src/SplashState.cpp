//
//  SplashState.cpp
//  Simple game
//
//  Created by M Power on 2019-05-07.
//  Copyright © 2019 M Power. All rights reserved.
//

#include "SplashState.hpp"

#include "ResourcePath.hpp"
#include "MainMenu.hpp"
#include <bits/utility.h>
#include <cstring>
#include <iostream>
#include <array>
#include <format>

namespace mp {
    SplashState::SplashState(GameDataRef data) : data (data) {}
    
    // Why not in the constructor? Is there a delay between when we make the object and
    // initialize it?
    void SplashState::init(){
        this->data->assets.load<sf::Texture>("SplashState:Logo", "assets/Images/logo.png");

        // NOTE: Formatting could potentially be sped up with constexpr func.
        constexpr auto PB_PATH = "assets/Images/ProgressBar/Bar-{}.png";
        std::string name, file_path;
        name.reserve(7);
        file_path.reserve(strlen(PB_PATH));
        for (std::size_t i = 0; i <= 100; i += 5) {
            name = std::format("Bar:{}", i);
            file_path = std::format(PB_PATH, i);
            this->data->assets.load<sf::Texture>(name, file_path);
        }

        this->data->assets.load<sf::Texture>("Bar:POWER", "assets/Images/ProgressBar/Bar-Eter.png");
        this->data->assets.load<sf::Texture>("Explosion", "assets/Images/explosion.png");
        this->data->assets.load<sf::Texture>("TimeStop", "assets/Images/TimeStop.png");
        this->data->assets.load<sf::Texture>("UnlimShield", "assets/Images/UnlimShield.png");
        this->data->assets.load<sf::Texture>("BOMB", "assets/Images/bomb.png");
        
        background.setTexture(this->data->assets.get<sf::Texture>("SplashState:Logo"));
        
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
