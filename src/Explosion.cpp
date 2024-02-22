#include "Explosion.hpp"
#include <iostream>

namespace shield {
    Explosions::Explosions(GameDataRef data) {
        _visible = sf::IntRect(0,0,512,512);
        texture = data->assets.get<sf::Texture>("Explosion");
        setTexture(*texture);
        setTextureRect(_visible);
        setOrigin(getLocalBounds().width / 2.0, getLocalBounds().height / 2.0);
    }
    
    void Explosions::tick() {
        if(!hasPlayed()){
            if(_visible.left + _visible.width == 4096){
                _visible.left = 0;
                _visible.top += 512;
            } else {
                _visible.left += 512;
            }
        }
        
        setTextureRect(_visible);
    }
    
    Explosions& Explosions::operator= (const Explosions &explosion) {
        texture = explosion.texture;
        _visible = explosion._visible;
        setTexture(*texture);
        setTextureRect(_visible);
        
        return *this;
    }
    
    inline bool Explosions::hasPlayed() {
        return _visible.left + _visible.width == 4096 && _visible.top + _visible.height == 4096;
    }
    
    void Explosions::reset() {
        _visible = sf::IntRect(0,0,512,512);
        setTextureRect(_visible);
    }

    void Explosions::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::Transformable tf;
        for (const sf::Vector2f& position : inner) {
            tf.setPosition(position);
            states.transform = tf.getTransform();
            target.draw(static_cast<sf::Sprite>(*this), states);
        }
    }
}
