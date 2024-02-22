#include "Game.hpp"
#include <iostream>

int main() {
    if(!sf::Shader::isAvailable()) {
        std::cerr << "This version of the game requires shader support.\n" << std::endl;
        return 1;
    }
    shield::Game game = shield::Game("Shield");
    game.run();
    return 0;
}
