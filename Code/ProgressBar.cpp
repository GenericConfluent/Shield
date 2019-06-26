//
//  ProgressBar.cpp
//  Shield
//
//  Created by M Power on 2019-06-25.
//  Copyright © 2019 M Power. All rights reserved.
//

#include "ProgressBar.hpp"

namespace mp {
    ProgressBar::ProgressBar(GameDataRef data) : _data(data) {
        _bar.setTexture(_data->assets.GetTexture("Bar:100"));
        _bar.setPosition(10.0, 10.0);
        _bar.setScale(0.8, 0.8);
    }
    
    void ProgressBar::setPosition(sf::Vector2f position){
        _bar.setPosition(position);
    }
    
    void ProgressBar::setValue(int percent){
        switch (percent) {
            case 0:
                _bar.setTexture(_data->assets.GetTexture("Bar:0"));
                break;

            case 5:
                _bar.setTexture(_data->assets.GetTexture("Bar:5"));
                break;

            case 10:
                _bar.setTexture(_data->assets.GetTexture("Bar:10"));
                break;

            case 15:
                _bar.setTexture(_data->assets.GetTexture("Bar:15"));
                break;

            case 20:
                _bar.setTexture(_data->assets.GetTexture("Bar:20"));
                break;

            case 25:
                _bar.setTexture(_data->assets.GetTexture("Bar:25"));
                break;

            case 30:
                _bar.setTexture(_data->assets.GetTexture("Bar:30"));
                break;

            case 35:
                _bar.setTexture(_data->assets.GetTexture("Bar:35"));
                break;

            case 40:
                _bar.setTexture(_data->assets.GetTexture("Bar:40"));
                break;

            case 45:
                _bar.setTexture(_data->assets.GetTexture("Bar:45"));
                break;

            case 50:
                _bar.setTexture(_data->assets.GetTexture("Bar:50"));
                break;

            case 55:
                _bar.setTexture(_data->assets.GetTexture("Bar:55"));
                break;

            case 60:
                _bar.setTexture(_data->assets.GetTexture("Bar:60"));
                break;

            case 65:
                _bar.setTexture(_data->assets.GetTexture("Bar:65"));
                break;

            case 70:
                _bar.setTexture(_data->assets.GetTexture("Bar:70"));
                break;

            case 75:
                _bar.setTexture(_data->assets.GetTexture("Bar:75"));
                break;

            case 80:
                _bar.setTexture(_data->assets.GetTexture("Bar:80"));
                break;

            case 85:
                _bar.setTexture(_data->assets.GetTexture("Bar:85"));
                break;

            case 90:
                _bar.setTexture(_data->assets.GetTexture("Bar:90"));
                break;

            case 95:
                _bar.setTexture(_data->assets.GetTexture("Bar:95"));
                break;

            case 100:
                _bar.setTexture(_data->assets.GetTexture("Bar:100"));
                break;

            default:
                break;
        }
    }
    
    void ProgressBar::draw(){
        _data->window.draw(_bar);
    }
}
