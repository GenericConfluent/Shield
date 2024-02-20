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
#include "GameState.hpp"
#include "MainMenu.hpp"
#include "Collision.hpp"



namespace mp{
    
//    std::vector<sf::Sprite> playerGhosts;
//    sf::Clock ghostClock;
//    std::vector<unsigned int> ghosttimerdelay;
    GameState::GameState(GameDataRef data) : _data(data) {}
    
    void GameState::init() {
        // Create the player
        _player = new Player(_data);
        // Set background
        _background.setTexture(_data->assets.get<sf::Texture>("Background"));
        _background.scale(0.5, 0.5);
        _background.setOrigin(_background.getLocalBounds().width/2, _background.getLocalBounds().height/2);
        _background.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
        // Setup the player
        _player->playerSprite.setTexture(_data->assets.get<sf::Texture>("Player"));
        _player->playerSprite.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
        
        _player->playerSprite.setOrigin(_player->playerSprite.getLocalBounds().width/2, _player->playerSprite.getLocalBounds().height/2);

        
        _playerGhost = new Ghost(_data);
        
        _scoreDisplay.setFont(_data->assets.get<sf::Font>("Saucer"));
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
        buffer.loadFromFile("assets/Audio/Sounds/boom.wav");
        
        _explosionSound.setBuffer(buffer);
        _explosionSound.setVolume(100);
        
        _powerUpManager = new PowerUpManager(_data, _player->playerSprite);
    }
    
    void GameState::handle_input() {
        sf::Event event;
        
        while (this->_data->window.pollEvent(event)) {
            if (sf::Event::Closed == event.type) {
                this->_data->window.close();
            }
            if (sf::Event::KeyPressed == event.type) {
                
            }
        }
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if(_energy != 0) {
                _player->playerSprite.move((-(cos(_player->playerSprite.getRotation()*3.14159265/180)*-3))*_player->getSpeed(), (sin(_player->playerSprite.getRotation()*3.14159265/180)*3)*_player->getSpeed());
                if(_subTime.getElapsedTime().asMilliseconds() >= 200) {
                    _energy -= 5;
                    _subTime.restart();
                }
            }
            _pressTime.restart();
        }
    }
    
    void GameState::update(float dt) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(this->_data->window);
        sf::Vector2f world_mousePos = this->_data->window.mapPixelToCoords(mousePos, _data->window.getView());
        
        this->_data->cursor.setPosition(world_mousePos);
        
        if(!_playBoom) {
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
                    _waitTime = 3 - float(_score.getElapsedTime().asSeconds()) / float(30 * ceil((double)(_powerUpManager->getGameSpeed()/5.0)));
                }
                
                if (_waitTime > 0.5 && _waitTime <= 1) {
                    _waitTime = 5 - float(_score.getElapsedTime().asSeconds()) / float(100 * ceil((double)(_powerUpManager->getGameSpeed()/5.0)));
                }
                
                Enemy enemy(_data ,_player->playerSprite.getPosition());
                _enemies.push_back(enemy);
                _spawnClock.restart();
            }
            
            if (_powerUpManager->infiniteShield()) {
                _energy = 100;
                _energyBar->setValue(3000);
            } else {
                if (_pressTime.getElapsedTime().asSeconds() >= 2.5 && _energy < 100) {
                    _energy += 5;
                }
                
                _energyBar->setValue(_energy);
            }
            
            std::stringstream stream;
            stream << std::setfill('0') << std::setw(4) << round(_score.getElapsedTime().asSeconds());
            
            _scoreDisplay.setString(stream.str());
            _scoreDisplay.setOrigin(_scoreDisplay.getLocalBounds().width/2, _scoreDisplay.getLocalBounds().height/2+150);
            _scoreDisplay.setPosition(_data->window.getSize().x/2, _data->window.getSize().y/2);
            
            for (int i  = 0; i < _enemies.size(); i++) {
                if((_enemies[i]._enemyBody.getPosition().x > _data->window.getSize().x*1.5 || _enemies[i]._enemyBody.getPosition().x < -400) && (_enemies[i]._enemyBody.getPosition().y > _data->window.getSize().y*1.5 || _enemies[i]._enemyBody.getPosition().y < -400)){
                    _enemies.erase(_enemies.begin() + i);
                } else {
                    _enemies[i].update(_powerUpManager->getGameSpeed(), _score.getElapsedTime().asSeconds());
                }
                
                if(PixelPerfectTest(_enemies[i]._enemyBody, _player->playerSprite)){
                    _playBoom = true;
                    _boom->setPosition(_player->playerSprite.getPosition());
                    _data->backgroundMusic.setPitch(1);
                    _data->backgroundMusic.setVolume(50);
                    _explosionSound.play();
                }
                
                if (_powerUpManager->bomb()) {
                    Explosion newExp(_data);
                    newExp.setPosition(_enemies[i]._enemyBody.getPosition());
                    _enemies.erase(_enemies.begin() + i);
                    _otherExplosions.push_back(newExp);
                }
            }

            for(int i = 0; i < _otherExplosions.size(); i++){
                if (_otherExplosions[i].hasPlayed()) {
                    _otherExplosions.erase(_otherExplosions.begin()+i);
                }
                _otherExplosions[i].tick();
            }

            _powerUpManager->update();
        } else {
            if(_boom->hasPlayed()){
                this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
                _explosionSound.pause();
                _data->backgroundMusic.setVolume(55);
            }
            _boom->tick();
        }
    }
    
    void GameState::draw(float dt)
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
            
            
            for(int i = 0; i < _otherExplosions.size(); i++){
                _data->window.draw(_otherExplosions[i]._spriteSheet);
            }
            
            _powerUpManager->draw();
            
            _energyBar->draw();
        } else {
            _data->window.draw(_boom->_spriteSheet);
        }
        
        
        this->_data->window.draw(this->_data->cursor);
        this->_data->window.display();
    }
}
