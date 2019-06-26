//
//  Enemy.cpp
//  Simple game
//
//  Created by M Power on 2019-05-11.
//  Copyright © 2019 M Power. All rights reserved.
//

#include "Enemy.hpp"
#include "ResourcePath.hpp"
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <random>

namespace mp {
    Enemy::Enemy(GameDataRef data, sf::Vector2f position) : _data(data)
    {
        const int border = 50;
        _enemyBody.setTexture(_data->assets.GetTexture("Enemy"));
        _enemyBody.scale(0.15, 0.15);
        _enemyBody.setOrigin(_enemyBody.getLocalBounds().width/2, _enemyBody.getLocalBounds().height/2);
        
        std::random_device rd;  //Will be used to obtain a seed for the
        //random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded
        // with rd()
        std::uniform_int_distribution<> disx(border, (int)(_data->window.getSize().x));
        std::uniform_int_distribution<> disy(border, (int)(_data->window.getSize().y));
        
        do {

            _enemyBody.setPosition(disx(gen), 0);
            _enemyBody.setPosition(_enemyBody.getPosition().x, disy(gen));
        } while (!(sqrt(pow((_enemyBody.getPosition().x-position.x), 2)+pow((_enemyBody.getPosition().y-position.y), 2)) > 700));
        
        const float PI = 3.14159265;
        
        float dx = _enemyBody.getPosition().x - position.x;
        float dy = _enemyBody.getPosition().y - position.y;
        
        float rotation = (atan2(dy, dx) * 180 / PI) + 180;
        
        _enemyBody.setRotation(rotation);
    }
    
    Enemy::~Enemy()
    {
    }
    
    void Enemy::draw()
    {
        _data->window.draw(_enemyBody);
    }
    
    void Enemy::update(float _slowMotion, float _time){
        float speed = float((_time/20)+1)/_slowMotion;
        _enemyBody.move(speed*(-(cos(_enemyBody.getRotation()*3.14159265/180)*-1)), speed*(sin(_enemyBody.getRotation()*3.14159265/180)*1));
    }
    
    bool Enemy::isDead(){
        
    }
}
