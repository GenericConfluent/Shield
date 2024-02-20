#include <algorithm>
#include <ctime>
#include "PowerUps.hpp"
#include "Collision.hpp"

namespace shield {
    const int PowerUp::TIMESTOP = 0;
    const int PowerUp::INFINITESHIELD = 1;
    const int PowerUp::BOMB = 2;

    PowerUpManager::PowerUpManager(GameDataRef data, sf::Sprite & player): _data(data){
        _buffSpawnClock.restart();
        _playerRef = &player;
    }
    
    void PowerUpManager::f_CheckCollisionsWithPlayer(){
        for (int i = 0; i < _onFieldPowerUps.size(); i++) {
            if(PixelPerfectTest(_onFieldPowerUps[i].second, *_playerRef)){
                std::pair<bool, int> exists = f_CheckPowerUpExists(_onFieldPowerUps[i].first);
                if(exists.first){
                    _currentBuffs[exists.second].second.restart();
                } else {
                    _currentBuffs.push_back(std::make_pair(_onFieldPowerUps[i].first, sf::Clock()));
                    _currentBuffs.back().second.restart();
                }
                _onFieldPowerUps.erase(_onFieldPowerUps.begin()+i);
                f_ApplyBuffs();
            }
        }
    }
    
    sf::Vector2f PowerUpManager::f_GetRandomPositionOnWindow(sf::Vector2f position){
        const int border = 75;
        sf::Vector2f newPosition;
        
        std::random_device rd;  //Will be used to obtain a seed for the
        //random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded
        // with rd()
        std::uniform_int_distribution<> disx(border, (int)(_data->window.getSize().x) - border);
        std::uniform_int_distribution<> disy(border, (int)(_data->window.getSize().y) - border);
        
        do {
            
            newPosition = sf::Vector2f(disx(gen), 0);
            newPosition = sf::Vector2f(newPosition.x, disy(gen));
        } while (!(sqrt(pow((newPosition.x-position.x), 2)+pow((newPosition.y-position.y), 2)) > 200));
        
        return newPosition;
    }
    
    std::pair<bool, int> PowerUpManager::f_CheckPowerUpExists(int powerUp){
        for (int i = 0; i < _currentBuffs.size(); i++) {
            if(_currentBuffs[i].first == powerUp){
                return std::pair<bool, int>(true, i);
            }
        }
        return std::pair<bool, int>(false, 0);
    }
    
    void PowerUpManager::f_ApplyBuffs(){
        for (int i = 0; i < _currentBuffs.size(); i++) {
            switch (_currentBuffs[i].first) {
                case PowerUp::TIMESTOP:{
                    _slowMotion = 15.0;
                    _data->backgroundMusic.setPitch(0.5);
                    break;
                }
                    
                case PowerUp::INFINITESHIELD:{
                    _infiniteShield = true;
                    break;
                }
                    
                case PowerUp::BOMB:{
                    _bomb = true;
                    break;
                }
                    
                default:
                    break;
            }
        }
    }
    
    void PowerUpManager::f_RemoveBuffs(int buffType){
        switch (buffType) {
            case PowerUp::TIMESTOP:{
                _slowMotion = 1.0;
                _data->backgroundMusic.setPitch(1);
                break;
            }
                
            case PowerUp::INFINITESHIELD:{
                _infiniteShield = false;
                break;
            }
            
            case PowerUp::BOMB:{
                _bomb = false;
                break;
            }
                
            default:
                break;
        }
    }
    
    void PowerUpManager::update(){
        if(_buffSpawnClock.getElapsedTime().asSeconds() >= _buffSpawnFrequency){
            //random number engine
            std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded
            // with rd()
            std::uniform_int_distribution<> dis(0, 2);
            
            switch (dis(gen)) {
                case PowerUp::TIMESTOP:{
                    sf::Sprite sprite;
                    sprite.setTexture(_data->assets.get<sf::Texture>("TimeStop"));
                    sprite.scale(0.099, 0.099);
                    sprite.setPosition(f_GetRandomPositionOnWindow(_playerRef->getPosition()));
                    
                    _onFieldPowerUps.push_back(std::make_pair(PowerUp::TIMESTOP, sprite));
                    break;
                }
                    
                case PowerUp::INFINITESHIELD:{
                    sf::Sprite sprite;
                    sprite.setTexture(_data->assets.get<sf::Texture>("UnlimShield"));
                    sprite.scale(0.099, 0.099);
                    sprite.setPosition(f_GetRandomPositionOnWindow(_playerRef->getPosition()));
                    
                    _onFieldPowerUps.push_back(std::make_pair(PowerUp::INFINITESHIELD, sprite));
                    break;
                }
                    
                case PowerUp::BOMB:{
                    sf::Sprite sprite;
                    sprite.setTexture(_data->assets.get<sf::Texture>("BOMB"));
                    sprite.scale(0.099, 0.099);
                    sprite.setPosition(f_GetRandomPositionOnWindow(_playerRef->getPosition()));
                    
                    _onFieldPowerUps.push_back(std::make_pair(PowerUp::BOMB, sprite));
                    break;
                }
                    
                default:
                    break;
            }
            _buffSpawnClock.restart();
        }
        
        f_CheckCollisionsWithPlayer();
        
        for (int i = 0; i < _currentBuffs.size(); i++) {
            if(_currentBuffs[i].second.getElapsedTime().asSeconds() >= PowerUp::PowerUpDuration[_currentBuffs[i].first]){
                f_RemoveBuffs(_currentBuffs[i].first);
                _currentBuffs.erase(_currentBuffs.begin()+i);
            }
        }
    }

    void PowerUpManager::draw(){
        for (int i = 0; i < _onFieldPowerUps.size(); i++) {
            _data->window.draw(_onFieldPowerUps[i].second);
        }
    }
    
    float PowerUpManager::getGameSpeed(){
        return _slowMotion;
    }
    
    bool PowerUpManager::infiniteShield(){
        return _infiniteShield;
    }
    
    bool PowerUpManager::bomb(){
        return _bomb;
    }
}

