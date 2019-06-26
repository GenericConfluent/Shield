//
//  GameState.hpp
//  Simple game
//
//  Created by M Power on 2019-05-08.
//  Copyright © 2019 M Power. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "State.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "ResourcePath.hpp"
#include "ProgressBar.hpp"
#include "Explosion.hpp"

namespace mp {
    class Player;
    class Enemy;
    
    enum Status {Menu, Transition, Playing};
    
    typedef std::shared_ptr<GameData> GameDataRef;
    
    struct guiPacket{
        
    };
    
    
    
    class GameState : public State
    {
    public:
        GameState(GameDataRef data);
        
        void Init();
        
        void reInit();
        
        void HandleInput();
        void Update(float dt);
        void Draw(float dt);
        
    private:
        GameDataRef _data;
        
        sf::Music _bgmusic;
        
        sf::Sprite _background;
        
        Player * _player;
        Ghost * _playerGhost;
        
        float _waitTime = 1.0;
        sf::Clock _spawnClock;
        
        sf::Clock _score;
        sf::Text _scoreDisplay;
        ProgressBar * _energyBar;
        int _energy = 100;
        sf::Clock _subTime;
        
        sf::Clock _pressTime;
        
        std::vector<Enemy> _enemies;
        
        Explosion * _boom;
        bool _playBoom = false;
        sf::Sound _explosionSound;
        
        // Game state
        float _slowMotion = 1.0;
        
        
    };
}
