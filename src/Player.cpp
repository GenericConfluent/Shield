#include "Player.hpp"

namespace shield {
    Player::Player(GameDataRef data) : _data(data) {
        playerSprite.scale(0.2, 0.2);
    }
    
    Player::~Player() {}
    
    void Player::draw() {
        _data->window.draw(playerSprite);
    }
    
    void Player::update(){}
    
    float Player::getSpeed(){
        return _playerSpeed;
    }
    
    void Player::attack() {}
    
    void Player::hit(float damage){
//        this->_health -= damage;
    }
    
    bool Player::isDead() {}
    
    Ghost::Ghost(GameDataRef data) : _data(data) {}
    
    void Ghost::spawn(sf::Vector2f position) {
        sf::Sprite newSprite;
        newSprite.scale(0.2, 0.2);
        newSprite.setTexture(_data->assets.get<sf::Texture>("Player"));
        newSprite.setColor(sf::Color(200,200,200, _baselineFade));
        newSprite.setPosition(position.x, position.y);
        _ghosts.push_back(std::make_pair(newSprite, 0.0));
    }
    
    void Ghost::update() {
        if(_timer.getElapsedTime().asSeconds() >= 1.0){
            for (int i = 0; i < _ghosts.size(); i++) {
                if(_baselineFade-(fadingTick*_ghosts[i].second) < 5.0){// Once the fade has reached a number less than 0 it will delete the ghost
                    _ghosts.erase(_ghosts.begin()+i);
                }
            }
            for (int i = 0; i < _ghosts.size(); i++) {
                _ghosts[i].second += 1.0;
                _ghosts[i].first.setColor(sf::Color(200,200,200, _baselineFade-(fadingTick*_ghosts[i].second)));
            }
        }
    }
    
    void Ghost::draw() {
        for (int i = _ghosts.size(); i > 0; i--) {
            _data->window.draw(_ghosts[i-1].first);
        }
    }
}
