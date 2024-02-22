#include "ProgressBar.hpp"
#include <format>

namespace shield {
    ProgressBar::ProgressBar(GameDataRef data) : _data(data) {
        _bar.setTexture(*_data->assets.get<sf::Texture>("Bar:100"));
        _bar.setPosition(10.0, 10.0);
        _bar.setScale(0.8, 0.8);
    }
    
    void ProgressBar::setPosition(sf::Vector2f position){
        _bar.setPosition(position);
    }
    
    void ProgressBar::setValue(int percent){
        std::string id;
        if (percent <= 100 && percent % 5 == 0) {
            id = std::format("Bar:{}", percent) ;
        } else if (percent == 3000) {
            id = "Bar:POWER";
        } else {
            return;
        }
        _bar.setTexture(*_data->assets.get<sf::Texture>(id));
    }
    
    void ProgressBar::draw(){
        _data->window.draw(_bar);
    }
}

