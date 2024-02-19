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
        // Dear me from 5 years ago: for loop. Look it up. Better yet: math.
        this->_data->assets.LoadTexture("SplashState:Logo", "assets/Images/logo.png");
        this->_data->assets.LoadTexture("Bar:0", "assets/Images/ProgressBar/Bar-0.png");
        this->_data->assets.LoadTexture("Bar:5", "assets/Images/ProgressBar/Bar-5.png");
        this->_data->assets.LoadTexture("Bar:10", "assets/Images/ProgressBar/Bar-10.png");
        this->_data->assets.LoadTexture("Bar:15", "assets/Images/ProgressBar/Bar-15.png");
        this->_data->assets.LoadTexture("Bar:20", "assets/Images/ProgressBar/Bar-20.png");
        this->_data->assets.LoadTexture("Bar:25", "assets/Images/ProgressBar/Bar-25.png");
        this->_data->assets.LoadTexture("Bar:30", "assets/Images/ProgressBar/Bar-30.png");
        this->_data->assets.LoadTexture("Bar:35", "assets/Images/ProgressBar/Bar-35.png");
        this->_data->assets.LoadTexture("Bar:40", "assets/Images/ProgressBar/Bar-40.png");
        this->_data->assets.LoadTexture("Bar:45", "assets/Images/ProgressBar/Bar-45.png");
        this->_data->assets.LoadTexture("Bar:50", "assets/Images/ProgressBar/Bar-50.png");
        this->_data->assets.LoadTexture("Bar:55", "assets/Images/ProgressBar/Bar-55.png");
        this->_data->assets.LoadTexture("Bar:60", "assets/Images/ProgressBar/Bar-60.png");
        this->_data->assets.LoadTexture("Bar:65", "assets/Images/ProgressBar/Bar-65.png");
        this->_data->assets.LoadTexture("Bar:70", "assets/Images/ProgressBar/Bar-70.png");
        this->_data->assets.LoadTexture("Bar:75", "assets/Images/ProgressBar/Bar-75.png");
        this->_data->assets.LoadTexture("Bar:80", "assets/Images/ProgressBar/Bar-80.png");
        this->_data->assets.LoadTexture("Bar:85", "assets/Images/ProgressBar/Bar-85.png");
        this->_data->assets.LoadTexture("Bar:90", "assets/Images/ProgressBar/Bar-90.png");
        this->_data->assets.LoadTexture("Bar:95", "assets/Images/ProgressBar/Bar-95.png");
        this->_data->assets.LoadTexture("Bar:100", "assets/Images/ProgressBar/Bar-Full.png");
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
