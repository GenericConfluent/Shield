#pragma once
#include "Game.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

namespace shield {
    // The origin on this thing is in the center, not the top left.
    class ProgressBar : public sf::Drawable, public sf::Transformable {
    public:
        ProgressBar(GameDataRef data);
        void setValue(int percent);

        static constexpr float WIDTH = 400.0;
        static constexpr float RADIUS = 20.0;

        //  The shader needs to know the left most point of the bar to 
        //  draw the gradient properly.
        void setPosition(float x, float y) {
            const float height = data->window.getSize().y;
            shader.setUniform("windowPos", sf::Vector2f(x, height - y));
            sf::Transformable::setPosition(x, y);
        }

        void setPosition(const sf::Vector2f& position) {
            setPosition(position.x, position.y);
        }
        
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void build_geometry();

        int value = 100;
        std::vector<sf::Vertex> vertices;
        sf::Shader shader;
        GameDataRef data;
    };
}

