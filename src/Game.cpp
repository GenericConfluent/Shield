#include "Game.hpp"
#include <SFML/Audio.hpp>
#include "SplashState.hpp"
#include "GameState.hpp"

namespace shield {
    Game::Game(std::string title) {
        _data->window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Fullscreen);
        _data->machine.AddState(StateRef(new SplashState(this->_data)));
        _data->window.setMouseCursorVisible(false);
        _data->assets.load<sf::Texture>("Cursor", "assets/Images/Cursor.png");
        _data->cursor.setTexture(this->_data->assets.get<sf::Texture>("Cursor"));
        _data->cursor.setOrigin(_data->cursor.getLocalBounds().width/2, _data->cursor.getLocalBounds().height/2);
    }
    
    void Game::run() {
        float newTime, frameTime, interpolation;
        
        float currentTime = this->_clock.getElapsedTime().asSeconds();
        float accumulator = 0.0f;
        
        _data->backgroundMusic.openFromFile("assets/Audio/Music/Origin.wav");
        _data->backgroundMusic.play();
        _data->backgroundMusic.setVolume(55);
        _data->backgroundMusic.setLoop(true);
        
        // What's going on in this game loop?
        while (this->_data->window.isOpen()) {
            this->_data->machine.ProcessStateChanges();
            
            newTime = this->_clock.getElapsedTime().asSeconds();
            frameTime = newTime - currentTime;
            
            if (frameTime > 0.25f) {
                frameTime = 0.25f;
            }
            
            currentTime = newTime;
            accumulator += frameTime;
            
            while (accumulator >= dt) {
                this->_data->machine.GetActiveState()->handle_input();
                this->_data->machine.GetActiveState()->update(dt);
                
                accumulator -= dt;
            }
            
            interpolation = accumulator / dt;
            this->_data->machine.GetActiveState()->draw(interpolation);
            
        }
    }
}

