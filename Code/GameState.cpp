//
//  GameState.cpp
//  Simple game
//
//  Created by M Power on 2019-05-08.
//  Copyright © 2019 M Power. All rights reserved.
//
#include <iostream>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <random>
#include "GameState.hpp"
#include "MainMenu.hpp"
#include "Collision.hpp"



namespace mp{
    
//    std::vector<sf::Sprite> playerGhosts;
//    sf::Clock ghostClock;
//    std::vector<unsigned int> ghosttimerdelay;
    
    sf::Vector2f GameState::getRandomPositionOnWindow(sf::Vector2f position){
        const int border = 50;
        sf::Vector2f newPosition;
        
        std::random_device rd;  //Will be used to obtain a seed for the
        //random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded
        // with rd()
        std::uniform_int_distribution<> disx(border, (int)(_data->window.getSize().x));
        std::uniform_int_distribution<> disy(border, (int)(_data->window.getSize().y));
        
        do {
            
            newPosition = sf::Vector2f(disx(gen), 0);
            newPosition = sf::Vector2f(newPosition.x, disy(gen));
        } while (!(sqrt(pow((newPosition.x-position.x), 2)+pow((newPosition.y-position.y), 2)) > 100));
        
        return newPosition;
    }
    
    GameState::GameState(GameDataRef data) : _data(data)
    {
        
    }
    
    void GameState::Init()
    {
        // Create the player
        _player = new Player(_data);
        // Set background
        _background.setTexture(_data->assets.GetTexture("Background"));
        _background.scale(0.5, 0.5);
        _background.setOrigin(_background.getLocalBounds().width/2, _background.getLocalBounds().height/2);
        _background.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
        // Setup the player
        _player->playerSprite.setTexture(_data->assets.GetTexture("Player"));
        _player->playerSprite.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
        
        _player->playerSprite.setOrigin(_player->playerSprite.getLocalBounds().width/2, _player->playerSprite.getLocalBounds().height/2);

        
        _playerGhost = new Ghost(_data);
        
        _scoreDisplay.setFont(_data->assets.GetFont("Saucer"));
        _scoreDisplay.setFillColor(sf::Color(51, 255, 178, 200));
        _scoreDisplay.setString("0000");
        _scoreDisplay.setCharacterSize(300);
        _scoreDisplay.setLetterSpacing(1.4);
        _scoreDisplay.setOrigin(_scoreDisplay.getLocalBounds().width/2, _scoreDisplay.getLocalBounds().height/2);
        _scoreDisplay.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
        
        _energyBar = new ProgressBar(_data);
        
        _boom = new Explosion(_data);
        _boom->reset();
        _playBoom = false;
        
        
        sf::SoundBuffer buffer;
        buffer.loadFromFile((resourcePath() + "Assets/Audio/Sounds/boom.wav"));
        
        _explosionSound.setBuffer(buffer);
        _explosionSound.setVolume(100);
        
    }
    
    void GameState::HandleInput()
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
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            if(_energy != 0){
                _player->playerSprite.move((-(cos(_player->playerSprite.getRotation()*3.14159265/180)*-3))*_player->getSpeed(), (sin(_player->playerSprite.getRotation()*3.14159265/180)*3)*_player->getSpeed());
                if(_subTime.getElapsedTime().asMilliseconds() >= 200){
                    _energy -= 5;
                    _subTime.restart();
                }
            }
            _pressTime.restart();
        }
        
        
    }
    
    void GameState::Update(float dt)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(this->_data->window);
        sf::Vector2f world_mousePos = this->_data->window.mapPixelToCoords(mousePos, _data->window.getView());
        
        this->_data->cursor.setPosition(world_mousePos);
        
        if(!_playBoom){
            sf::Vector2f playerPos = _player->playerSprite.getPosition();
            
            const float PI = 3.14159265;
            
            float dx = playerPos.x - world_mousePos.x;
            float dy = playerPos.y - world_mousePos.y;
            
            float rotation = (atan2(dy, dx) * 180 / PI) + 180;
            
            _player->playerSprite.setRotation(rotation);
            
            _player->update();
            
            
            
            _playerGhost->update();
            
            //        if(_spawnClock.getElapsedTime().asMilliseconds() > 400){
            //            _playerGhost->spawn(_player->playerSprite.getPosition());
            //        }
            
            
            if(_spawnClock.getElapsedTime().asSeconds() > _waitTime){
                if (_waitTime > 1.0) {
                    _waitTime = 3 - float(_score.getElapsedTime().asSeconds()) / float(30 * ceil((double)(_slowMotion/5.0)));
                }
                
                if (_waitTime > 0.5 && _waitTime <= 1) {
                    _waitTime = 5 - float(_score.getElapsedTime().asSeconds()) / float(100 * ceil((double)(_slowMotion/5.0)));
                }
                
                Enemy enemy(_data ,_player->playerSprite.getPosition());
                _enemies.push_back(enemy);
                _spawnClock.restart();
            }
            
            if (_pressTime.getElapsedTime().asSeconds() >= 2.5 && _energy < 100) {
                _energy += 5;
            }
            
            _energyBar->setValue(_energy);
            
            std::stringstream stream;
            stream << std::setfill('0') << std::setw(4) << round(_score.getElapsedTime().asSeconds());
            
            _scoreDisplay.setString(stream.str());
            _scoreDisplay.setOrigin(_scoreDisplay.getLocalBounds().width/2, _scoreDisplay.getLocalBounds().height/2+150);
            _scoreDisplay.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
            
            for (int i  = 0; i < _enemies.size(); i++) {
                if((_enemies[i]._enemyBody.getPosition().x > _data->window.getSize().x*1.5 || _enemies[i]._enemyBody.getPosition().x < -400) && (_enemies[i]._enemyBody.getPosition().y > _data->window.getSize().y*1.5 || _enemies[i]._enemyBody.getPosition().y < -400)){
                    _enemies.erase(_enemies.begin() + i);
                } else {
                    _enemies[i].update(_slowMotion, _score.getElapsedTime().asSeconds());
                }
                
                if(PixelPerfectTest(_enemies[i]._enemyBody, _player->playerSprite)){
                    _playBoom = true;
                    _boom->setPosition(_player->playerSprite.getPosition());
                    _data->backgroundMusic.setPitch(1);
                    _data->backgroundMusic.setVolume(50);
                    _explosionSound.play();
                }
            }
            
            if (_buffSpawnClock.getElapsedTime().asSeconds() > 14.5) {
                std::random_device rd;  //Will be used to obtain a seed for the
                //random number engine
                std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded
                // with rd()
                std::uniform_int_distribution<> dis(0, 1);
                
                switch (dis(gen)) {
                    case 0:{
                        sf::Sprite sprite;
                        sprite.setTexture(_data->assets.GetTexture("TimeStop"));
                        sprite.scale(0.099, 0.099);
                        sprite.setPosition(getRandomPositionOnWindow(_player->playerSprite.getPosition()));
                        _timeStops.push_back(sprite);
                        break;
                    }
                        

                    default:
                        break;
                }
                _buffSpawnClock.restart();
            }
            
            for (int i = 0; i < _timeStops.size(); i++) {
                if(PixelPerfectTest(_timeStops[i], _player->playerSprite)){
                    _slowMotion = 15.0;
                    _timeStops.erase(_timeStops.begin()+i);
                    _buffDuration.restart();
                    _data->backgroundMusic.setPitch(0.5);
                }
            }
            
            if(_slowMotion > 1.5){
                if(_buffDuration.getElapsedTime().asSeconds() > 8){
                    _slowMotion = 1.0;
                    _data->backgroundMusic.setPitch(1);
                }
            }
            
        } else {
            if(_boom->hasPlayed()){
                this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
                _explosionSound.pause();
                _data->backgroundMusic.setVolume(55);
            }
            _boom->tick();
        }
    }
    
    void GameState::Draw(float dt)
    {
        this->_data->window.clear( sf::Color(55,55,55) );
        this->_data->window.draw(_background);
        
    
        this->_data->window.draw(_scoreDisplay);
        //        this->_playerGhost->draw();
        if(!_playBoom){
            this->_player->draw();
            
            for (int i  = 0; i < _enemies.size(); i++) {
                _enemies[i].draw();
            }
            
            for (int i = 0; i < _timeStops.size(); i++) {
                this->_data->window.draw(_timeStops[i]);
            }
            
            _energyBar->draw();
        } else {
            _boom->draw();
        }
        
        
        this->_data->window.draw(this->_data->cursor);
        this->_data->window.display();
    }
}
