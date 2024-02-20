//
//  Explosion.cpp
//  Shield
//
//  Created by M Power on 2019-06-26.
//  Copyright © 2019 M Power. All rights reserved.
//

#include "Explosion.hpp"
#include "ResourcePath.hpp"

namespace mp {
    Explosion::Explosion(GameDataRef data) : _data(data){
        _visible = sf::IntRect(0,0,512,512);
        _spriteSheet.setTexture(_data->assets.get<sf::Texture>("Explosion"));
        _spriteSheet.setTextureRect(_visible);
        _spriteSheet.setOrigin(_spriteSheet.getLocalBounds().width/2, _spriteSheet.getLocalBounds().height/2);
    }
    
    void Explosion::tick(){
        if(_visible.left + _visible.width == 4096 && _visible.top + _visible.height == 4096){
            _done = true;
        }
        
        if(!_done){
            if(_visible.left + _visible.width == 4096){
                _visible.left = 0;
                _visible.top += 512;
            } else {
                _visible.left += 512;
            }
        }
        
        _spriteSheet.setTextureRect(_visible);
    }
    
    Explosion& Explosion::operator= (const Explosion &explosion){
        _data = explosion._data;
        _visible = explosion._visible;
        _done = explosion._done;
        _spriteSheet.setTexture(_data->assets.get<sf::Texture>("Explosion"));
        _spriteSheet.setTextureRect(_visible);
        
        // return the existing object so we can chain this operator
        return *this;
    }
    
    void Explosion::setPosition(sf::Vector2f position){
//        _spriteSheet.setPosition(position.x - (_size/2), position.y - (_size/2));
        _spriteSheet.setPosition(position);
    }
    
    bool Explosion::hasPlayed(){
        return _done;
    }
    
    void Explosion::reset(){
        _visible = sf::IntRect(0,0,512,512);
        _spriteSheet.setTextureRect(_visible);
    }
}
