#include "MainMenu.hpp"
#include "GameState.hpp"
#include <iostream>
#include <cmath>

namespace shield {
    MainMenuState::MainMenuState(GameDataRef data) : _data(data) {}
    
    void MainMenuState::init() {
        // Load textures
        _data->assets.load<sf::Texture>("Background", "assets/Images/Background.png");
        _data->assets.load<sf::Texture>("MBackground", "assets/Images/MenuBackground.jpg");
        _data->assets.load<sf::Texture>("Player", "assets/Images/Player.png");
        _data->assets.load<sf::Texture>("Enemy", "assets/Images/Enemy.png");
        // Load fonts
        _data->assets.load<sf::Font>("Saucer", "assets/Fonts/SpaceMono.ttf");
        _data->assets.load<sf::Font>("Orbiter", "assets/Fonts/earthorbiter.ttf");
        _data->assets.load<sf::Font>("SpaceAge", "assets/Fonts/space age.ttf");
        
        // Set up the GUI
        sf::Vector2f lineSize(float(_data->window.getSize().x)/2.895, 5.0);
        
        title.setFont(*_data->assets.get<sf::Font>("SpaceAge"));
        title.setString("Shield");
        title.setCharacterSize(150);
        title.setPosition((_data->window.getSize().x / 2.0) - title.getLocalBounds().width / 2.0, 10.0);
        
        line.setSize(lineSize);
        line.setPosition(0.0, _data->window.getSize().y/1.5);
        line.setFillColor(sf::Color(255,255,255));
        
        heading.setFont(*_data->assets.get<sf::Font>("Orbiter"));
        heading.setString("Play Game(G)");
        heading.setCharacterSize(50);
        heading.setOrigin(heading.getLocalBounds().width, 0.0);
        heading.setPosition(line.getPosition().x + line.getLocalBounds().width, line.getPosition().y - 55);
        
        aboutline.setSize(lineSize);
        aboutline.setPosition(0.0, line.getPosition().y+100);
        aboutline.setFillColor(sf::Color(255,255,255));
        
        aboutheading.setFont(*_data->assets.get<sf::Font>("Orbiter"));
        aboutheading.setString("Information(A)");
        aboutheading.setCharacterSize(50);
        aboutheading.setOrigin(aboutheading.getLocalBounds().width, 0.0);
        aboutheading.setPosition(aboutline.getPosition().x + aboutline.getLocalBounds().width, aboutline.getPosition().y - 55);
        
        leaveline.setSize(lineSize);
        leaveline.setPosition(0.0, aboutline.getPosition().y+100);
        leaveline.setFillColor(sf::Color(255,255,255));
        
        leaveheading.setFont(*_data->assets.get<sf::Font>("Orbiter"));
        leaveheading.setString("Leave App(Esc)");
        leaveheading.setCharacterSize(50);
        leaveheading.setOrigin(aboutheading.getLocalBounds().width, 0.0);
        leaveheading.setPosition(leaveline.getPosition().x + leaveline.getLocalBounds().width, leaveline.getPosition().y - 55);
        
        background.setTexture(*_data->assets.get<sf::Texture>("MBackground"));
        background.setScale(0.55, 0.55);
        background.setOrigin(background.getLocalBounds().width/2, background.getLocalBounds().height/2);
        background.setPosition(_data->window.getSize().x / 2.0, _data->window.getSize().y / 2.0);
        
        Out.titlePos = sf::Vector2f((_data->window.getSize().x / 2.0) - title.getLocalBounds().width / 2.0, -500);
        Out.linePos = sf::Vector2f(-(line.getLocalBounds().width) - 20, _data->window.getSize().y/1.5);
    }
    
    void MainMenuState::handle_input() {
        sf::Event event;
        
        while (this->_data->window.pollEvent(event)) {
            if (sf::Event::Closed == event.type) {
                this->_data->window.close();
            }
            if (sf::Event::KeyPressed == event.type) {
                
            }
        }
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::G)){
            transition = true;
        }
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !(sf::Keyboard::isKeyPressed(sf::Keyboard::G))){
            transition = true;
        }
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && !(sf::Keyboard::isKeyPressed(sf::Keyboard::G)) && !(sf::Keyboard::isKeyPressed(sf::Keyboard::A))){
            _data->window.close();
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::G)){
            heading.setFillColor(sf::Color(51, 255, 178));
            line.setFillColor(sf::Color(51, 255, 178));
            // _data->machine.AddState(StateRef(new GameState(_data)), true);
        } else {
            heading.setFillColor(sf::Color(255,255,255));
            line.setFillColor(sf::Color(255,255,255));
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !(sf::Keyboard::isKeyPressed(sf::Keyboard::G))){
            aboutheading.setFillColor(sf::Color(51, 255, 178));
            aboutline.setFillColor(sf::Color(51, 255, 178));
        } else {
            aboutheading.setFillColor(sf::Color(255,255,255));
            aboutline.setFillColor(sf::Color(255,255,255));
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && !(sf::Keyboard::isKeyPressed(sf::Keyboard::G)) && !(sf::Keyboard::isKeyPressed(sf::Keyboard::A))){
            leaveheading.setFillColor(sf::Color(51, 255, 178));
            leaveline.setFillColor(sf::Color(51, 255, 178));
            _data->window.close();
        } else {
            leaveheading.setFillColor(sf::Color(255,255,255));
            leaveline.setFillColor(sf::Color(255,255,255));
        }
    }
    
    void MainMenuState::update(float dt)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(this->_data->window);
        sf::Vector2f world_mousePos = this->_data->window.mapPixelToCoords(mousePos, _data->window.getView());
        
        this->_data->cursor.setPosition(world_mousePos);
        
        if (transition) {
            line.move(-_transitionSpeed, 0.0);
            title.move(0.0, -_transitionSpeed);
            heading.setPosition(line.getPosition().x + line.getLocalBounds().width, heading.getPosition().y);
            aboutheading.setPosition(line.getPosition().x + line.getLocalBounds().width, aboutheading.getPosition().y);
            aboutline.setPosition(line.getPosition().x, aboutline.getPosition().y);
        }
        
        if(title.getPosition().y <= -300 && line.getPosition().x <= -500){
            transition = false;
            this->_data->machine.AddState(StateRef(new GameState(_data)), true);
            std::cout << "\nChange to game state\n";
        }
    }
    
    void MainMenuState::draw(float dt) {
        this->_data->window.clear( sf::Color(55,55,55) );
        
        this->_data->window.draw(background);
        this->_data->window.draw(title);
        this->_data->window.draw(line);
        this->_data->window.draw(heading);
        this->_data->window.draw(aboutline);
        this->_data->window.draw(aboutheading);
        this->_data->window.draw(leaveline);
        this->_data->window.draw(leaveheading);
        
        this->_data->window.draw(this->_data->cursor);
        this->_data->window.display();
    }
}

