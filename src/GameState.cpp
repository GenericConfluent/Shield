#include <iostream>
#include <numbers>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include "GameState.hpp"
#include "MainMenu.hpp"
#include "Collision.hpp"
#include "SFML/System/Vector2.hpp"

namespace shield {
//    std::vector<sf::Sprite> playerGhosts;
//    sf::Clock ghostClock;
//    std::vector<unsigned int> ghosttimerdelay;
    constexpr float PI = std::numbers::pi_v<float>;

    GameState::GameState(GameDataRef data) : _data(data), explosions(data) {}
    
    void GameState::init() {
        // Create player
        _player = {std::make_unique<Player>(Player(_data))};
        // Set background
        _background.setTexture(*_data->assets.get<sf::Texture>("Background"));
        _background.scale(0.5, 0.5);
        _background.setOrigin(_background.getLocalBounds().width/2, _background.getLocalBounds().height/2);
        _background.setPosition(_data->window.getSize().x / 2.0, _data->window.getSize().y / 2.0);
        // Setup the player
        _player.value()->playerSprite.setTexture(*_data->assets.get<sf::Texture>("Player"));
        _player.value()->playerSprite.setPosition(_data->window.getSize().x/2.0, _data->window.getSize().y/2.0);
        
        _player.value()->playerSprite.setOrigin(_player.value()->playerSprite.getLocalBounds().width/2, _player.value()->playerSprite.getLocalBounds().height/2);
        
        //_playerGhost = new Ghost(_data);
        
        _scoreDisplay.setFont(*_data->assets.get<sf::Font>("Saucer"));
        _scoreDisplay.setFillColor(sf::Color(51, 255, 178, 200));
        _scoreDisplay.setString("0000");
        _scoreDisplay.setCharacterSize(300);
        _scoreDisplay.setLetterSpacing(1.4);
        _scoreDisplay.setOrigin(_scoreDisplay.getLocalBounds().width/2, _scoreDisplay.getLocalBounds().height/2);
        _scoreDisplay.setPosition(_data->window.getSize().x/2.0, _data->window.getSize().y/2.0);
        
        _energyBar = new ProgressBar(_data);
        
        _boom = new Explosions(_data);
        _boom->reset();
        _playBoom = false;
        
        sf::SoundBuffer buffer;
        buffer.loadFromFile("assets/Audio/Sounds/boom.wav");
        
        _explosionSound.setBuffer(buffer);
        _explosionSound.setVolume(100);
        
        _powerUpManager = new PowerUpManager(_data, _player.value()->playerSprite);
    }
    
    void GameState::handle_input() {
        sf::Event event;
        
        while (this->_data->window.pollEvent(event)) {
            if (sf::Event::Closed == event.type) {
                this->_data->window.close();
            }
        }
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if(_energy > 0) {
                Player& pl = *_player.value().get();
                const float rotation_rad = pl.playerSprite.getRotation() * (PI / 180.0f);

                sf::Vector2f direction;
                direction.x = cos(rotation_rad);
                direction.y = sin(rotation_rad);

                pl.playerSprite.move(direction * pl.getSpeed());
                if(_subTime.getElapsedTime().asMilliseconds() >= 200) {
                    _energy -= 5;
                    _subTime.restart();
                }
            }
            _pressTime.restart();
        }
    }
    
    void GameState::update(float dt) {
        Player& pl = *_player.value().get();

        sf::Vector2i mousePos = sf::Mouse::getPosition(this->_data->window);
        sf::Vector2f world_mousePos = this->_data->window.mapPixelToCoords(mousePos, _data->window.getView());
        
        this->_data->cursor.setPosition(world_mousePos);
        
        if(pl.isDead()) {
            if(explosions.hasPlayed()) {
                this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
                _explosionSound.pause();
                _data->backgroundMusic.setVolume(0);
            }
            explosions.tick();
            return;
        }

        sf::Vector2f playerPos = _player.value()->playerSprite.getPosition();
        
        float dx = playerPos.x - world_mousePos.x;
        float dy = playerPos.y - world_mousePos.y;
        float rotation = (atan2(dy, dx) * 180.0 / PI) + 180.0;
        
        pl.playerSprite.setRotation(rotation);
        pl.update();
        
        //_playerGhost->update();
        
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
            
            Enemy enemy(_data, _player.value()->playerSprite.getPosition());
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
        _scoreDisplay.setPosition(_data->window.getSize().x / 2.0, _data->window.getSize().y / 2.0);
        
        for (std::size_t i = 0; i < _enemies.size(); ++i) {
            if((_enemies[i]._enemyBody.getPosition().x > _data->window.getSize().x*1.5 || _enemies[i]._enemyBody.getPosition().x < -400) && (_enemies[i]._enemyBody.getPosition().y > _data->window.getSize().y*1.5 || _enemies[i]._enemyBody.getPosition().y < -400)){
                _enemies.erase(_enemies.begin() + i);
            } else {
                _enemies[i].update(_powerUpManager->getGameSpeed(), _score.getElapsedTime().asSeconds());
            }
            
            // NOTE: Order matters here, I'm short circuiting for a performance gain.
            if(BoundingBoxTest(_enemies[i]._enemyBody, pl.playerSprite) && PixelPerfectTest(_enemies[i]._enemyBody, pl.playerSprite)) {
                pl.kill();
                explosions.inner.push_back(pl.playerSprite.getPosition());
                explosions.reset();
                _data->backgroundMusic.setPitch(1);
                _data->backgroundMusic.setVolume(0);
                _explosionSound.play();
            }
        }

        if (_powerUpManager->bomb()) {
            if (explosions.hasPlayed()) {
                explosions.reset();
            }
            for (const auto& enemy : _enemies) {
                explosions.inner.push_back(enemy._enemyBody.getPosition());
            }
            _enemies.clear();
        }

        if (explosions.hasPlayed()) {
            explosions.inner.clear();
        } else {
            explosions.tick();
        }

        _powerUpManager->update();
    }
    
    void GameState::draw(float dt) {
        this->_data->window.clear( sf::Color(55,55,55) );
        this->_data->window.draw(_background);
    
        this->_data->window.draw(_scoreDisplay);
        //        this->_playerGhost->draw();
        if(!_player.value()->isDead()) {
            _player.value()->draw();
            
            for (auto& enemy : _enemies) {
                enemy.draw();
            }
            
            _data->window.draw(explosions);
            
            _powerUpManager->draw();
            _energyBar->draw();
        } else {
            _data->window.draw(explosions);
        }
        
        this->_data->window.draw(this->_data->cursor);
        this->_data->window.display();
    }
}

