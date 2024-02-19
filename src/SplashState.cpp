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
    SplashState::SplashState(GameDataRef data) : _data (data) {}
    
    // Why not in the constructor? Is there a delay between when we make the object and
    // initialize it?
    void SplashState::Init(){
        // Dear me from 5 years ago: for loop. Look it up. Better yet: math.
        // I'll fix this properly later.
        
        this->_data->assets.LoadTexture("SplashState:Logo", "assets/Images/logo.png");

        constexpr auto PB_PATH = "assets/Images/ProgressBar/Bar-{}.png";
        std::string name, file_path;
        name.reserve(7);
        file_path.reserve(strlen(PB_PATH));
        for (std::size_t i = 0; i <= 100; i += 5) {
            name = std::format("Bar:{}", i);
            file_path = std::format(PB_PATH, i);
            this->_data->assets.LoadTexture(name, file_path);
        }

        this->_data->assets.LoadTexture("Bar:POWER", "assets/Images/ProgressBar/Bar-Eter.png");
        this->_data->assets.LoadTexture("Explosion", "assets/Images/explosion.png");
        this->_data->assets.LoadTexture("TimeStop", "assets/Images/TimeStop.png");
        this->_data->assets.LoadTexture("UnlimShield", "assets/Images/UnlimShield.png");
        this->_data->assets.LoadTexture("BOMB", "assets/Images/bomb.png");
        
        _background.setTexture(this->_data->assets.GetTexture("SplashState:Logo"));
        
        sf::Image icon;
        icon.loadFromFile("assets/Images/Player.png"); // File/Image/Pixel
        _data->window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        
        sf::Vector2f windowSize(this->_data->window.getSize());
        _background.setOrigin(_background.getLocalBounds().width/2,_background.getLocalBounds().height/2);
        _background.setPosition((float)(windowSize.x/2), (float)(windowSize.y/2));
        std::cout << windowSize.x << ", " << windowSize.y;
    }
    
    void SplashState::HandleInput(){
        sf::Event event;
        
        while (this->_data->window.pollEvent(event))
        {
            if (sf::Event::Closed == event.type)
            {
                this->_data->window.close();
            }
            
            if(sf::Event::KeyPressed == event.type){
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                    this->_data->window.close();
                }
            }
        }
    }
    
    void SplashState::Update(float dt){
        if (this->_clock.getElapsedTime().asSeconds() > 2.0) {
            this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
        }
    }
    
    void SplashState::Draw(float dt){
        this->_data->window.clear(sf::Color(50,50,50));
        this->_data->window.draw( this->_background );
        this->_data->window.display();
    }
}
