//
//  Game.cpp
//  Simple game
//
//  Created by M Power on 2019-05-06.
//  Copyright © 2019 M Power. All rights reserved.
//

#include "Game.hpp"
#include <SFML/Audio.hpp>
#include "SplashState.hpp"
#include "ResourcePath.hpp"
#include "GameState.hpp"

namespace mp {
    Game::Game(std::string title){
        _data->window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Fullscreen);
        _data->machine.AddState(StateRef(new SplashState(this->_data)));
        _data->window.setMouseCursorVisible(false);
        _data->assets.LoadTexture("Cursor", (resourcePath() + "Assets/Images/Cursor.png"));
        _data->cursor.setTexture(this->_data->assets.GetTexture("Cursor"));
        _data->cursor.setOrigin(_data->cursor.getLocalBounds().width/2, _data->cursor.getLocalBounds().height/2);
        
        
        this->Run();
    }
    
    void Game::Run()
    {
        float newTime, frameTime, interpolation;
        
        float currentTime = this->_clock.getElapsedTime().asSeconds();
        float accumulator = 0.0f;

        
        _data->backgroundMusic.openFromFile(resourcePath() + "Assets/Audio/Music/Origin.wav");
        _data->backgroundMusic.play();
        _data->backgroundMusic.setVolume(55);
        _data->backgroundMusic.setLoop(true);
        
        while (this->_data->window.isOpen())
        {
            this->_data->machine.ProcessStateChanges();
            
            newTime = this->_clock.getElapsedTime().asSeconds();
            frameTime = newTime - currentTime;
            
            if (frameTime > 0.25f)
            {
                frameTime = 0.25f;
            }
            
            currentTime = newTime;
            accumulator += frameTime;
            
            
            while (accumulator >= dt)
            {
                this->_data->machine.GetActiveState()->HandleInput();
                this->_data->machine.GetActiveState()->Update(dt);
                
                accumulator -= dt;
            }
            
            interpolation = accumulator / dt;
            this->_data->machine.GetActiveState()->Draw(interpolation);
            
        }
    }
    

}

