//
//  MainMenu.cpp
//  Simple game
//
//  Created by M Power on 2019-05-07.
//  Copyright © 2019 M Power. All rights reserved.
//

#include "MainMenu.hpp"
#include "ResourcePath.hpp"
#include "GameState.hpp"
#include <iostream>
#include <cmath>

namespace mp {
    MainMenuState::MainMenuState(GameDataRef data) : _data(data)
    {
        
    }
    
    void MainMenuState::Init()
    {
        // Load textures
        _data->assets.LoadTexture("Background", (resourcePath() + "Assets/Images/Background.png"));
        _data->assets.LoadTexture("MBackground", (resourcePath() + "Assets/Images/MenuBackground.jpg"));
        _data->assets.LoadTexture("Player", (resourcePath() + "Assets/Images/Player.png"));
        _data->assets.LoadTexture("Enemy", (resourcePath() + "Assets/Images/Enemy.png"));
        // Load fonts
        _data->assets.LoadFont("Saucer", (resourcePath() + "Assets/Fonts/SpaceMono.ttf"));
        _data->assets.LoadFont("Orbiter", (resourcePath() + "Assets/Fonts/earthorbiter.ttf"));
        _data->assets.LoadFont("SpaceAge", (resourcePath() + "Assets/Fonts/space age.ttf"));
        
        // Set up the GUI
        title.setFont(_data->assets.GetFont("SpaceAge"));
        title.setString("Shield");
        title.setCharacterSize(150);
        title.setPosition((_data->window.getSize().x/2) - title.getLocalBounds().width/2, 10.0);
        
        line.setSize(sf::Vector2f(float(_data->window.getSize().x)/1.86, 5.0));
        line.setPosition(0.0, _data->window.getSize().y/1.5);
        line.setFillColor(sf::Color(255,255,255));
        
        heading.setFont(_data->assets.GetFont("Orbiter"));
        heading.setString("Press G and Click");
        heading.setCharacterSize(50);
        heading.setOrigin(heading.getLocalBounds().width, 0.0);
        heading.setPosition(line.getPosition().x + line.getLocalBounds().width, line.getPosition().y - 55);
        
        background.setTexture(_data->assets.GetTexture("MBackground"));
        background.setScale(0.55, 0.55);
        background.setOrigin(background.getLocalBounds().width/2, background.getLocalBounds().height/2);
        background.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
        
        Out.titlePos = sf::Vector2f((_data->window.getSize().x/2) - title.getLocalBounds().width/2, -500);
        Out.linePos = sf::Vector2f(-(line.getLocalBounds().width) - 20, _data->window.getSize().y/1.5);
    }
    
    void MainMenuState::HandleInput()
    {
        sf::Event event;
        
        while (this->_data->window.pollEvent(event))
        {
            if (sf::Event::Closed == event.type)
            {
                this->_data->window.close();
            }
            if (sf::Event::KeyPressed == event.type) {
                
            }
        }
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::G)){
            transition = true;
        }
    }
    
    void MainMenuState::Update(float dt)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(this->_data->window);
        sf::Vector2f world_mousePos = this->_data->window.mapPixelToCoords(mousePos, _data->window.getView());
        
        this->_data->cursor.setPosition(world_mousePos);
        
        if (transition) {
            line.move(-_transitionSpeed, 0.0);
            title.move(0.0, -_transitionSpeed);
            heading.setPosition(line.getPosition().x + line.getLocalBounds().width, heading.getPosition().y);
        }
        
        if(title.getPosition().y <= -300 && line.getPosition().x <= -500){
            transition = false;
            this->_data->machine.AddState(StateRef(new GameState(_data)), true);
            std::cout << "\nChange to game state\n";
        }
        
        
    }
    
    void MainMenuState::Draw(float dt)
    {
        this->_data->window.clear( sf::Color(55,55,55) );
        
        this->_data->window.draw(background);
        this->_data->window.draw(title);
        this->_data->window.draw(line);
        this->_data->window.draw(heading);
        
        this->_data->window.draw(this->_data->cursor);
        this->_data->window.display();
    }
}
