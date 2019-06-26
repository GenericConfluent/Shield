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
#include <iostream>

namespace mp {
    SplashState::SplashState(GameDataRef data) : _data (data) {
        
    }
    
    void SplashState::Init(){
        this->_data->assets.LoadTexture("SplashState:Logo", (resourcePath() + "Assets/Images/logo.png"));
        
        _background.setTexture(this->_data->assets.GetTexture("SplashState:Logo"));
        
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
        if (this->_clock.getElapsedTime().asSeconds() > 2.0)
        {
            // Switch To Main Menu
            this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
        }
    }
    
    void SplashState::Draw(float dt){
        this->_data->window.clear(sf::Color(50,50,50));
        
        this->_data->window.draw( this->_background );
        
        this->_data->window.display();
    }
}
