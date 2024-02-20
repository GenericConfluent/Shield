#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <list>
#include <iterator>
#include <utility>
#include "State.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "ResourcePath.hpp"
#include "ProgressBar.hpp"
#include "Explosion.hpp"
#include "PowerUps.hpp"

namespace shield {
    class Player;
    class Enemy;
    
    enum Status {Menu, Transition, Playing};
    
    typedef std::shared_ptr<GameData> GameDataRef;
    
    class GameState : public State {
    public:
        GameState(GameDataRef data);
        
        void init();
        
        void reInit();
        
        void handle_input();
        void update(float dt);
        void draw(float dt);
        
    private:
        sf::Vector2f getRandomPositionOnWindow(sf::Vector2f position);
        
        GameDataRef _data;
        
        sf::Music _bgmusic;
        sf::Sprite _background;
        
        Player *_player;
        Ghost *_playerGhost;
        
        float _waitTime = 1.0;
        sf::Clock _spawnClock;
        
        sf::Clock _buffDuration;
        
        sf::Clock _score;
        sf::Text _scoreDisplay;
        ProgressBar *_energyBar;
        int _energy = 100;
        sf::Clock _subTime;
        sf::Clock _pressTime;
        
        std::vector<Enemy> _enemies;
        
        Explosion *_boom;
        bool _playBoom = false;
        sf::Sound _explosionSound;
        
        std::vector<Explosion> _otherExplosions;
        
        PowerUpManager *_powerUpManager;
    };
}

