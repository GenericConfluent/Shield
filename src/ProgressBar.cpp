#include "ProgressBar.hpp"
#include "SFML/Graphics/PrimitiveType.hpp"
#include <array>
#include <cmath>
#include <format>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <numbers>
#include <optional>

namespace shield {
    constexpr float PI = std::numbers::pi_v<float>;
    // NOTE: `width` must account for `radius`, and the height is 2r.
    
    ProgressBar::ProgressBar(GameDataRef data) : data(data) {
        build_geometry();
        // TODO: Error handling.
        shader.loadFromFile("assets/shader/ProgressBar.frag", sf::Shader::Fragment);
        shader.setUniform("width", ProgressBar::WIDTH);
        shader.setUniform("radius", ProgressBar::RADIUS);
        shader.setUniform("borderWidth", 2.0f);
        shader.setUniform("progress", 1.0f);
    }
    
    void ProgressBar::setValue(int percent){
        value = percent;
        shader.setUniform("progress", static_cast<float>(percent) / 100.0f);
    }

    
    void ProgressBar::build_geometry() {
        vertices.clear();
        constexpr size_t round_sides = 20;
        constexpr float theta_delta = PI / static_cast<float>(round_sides);

        const sf::Vector2f center(0.0f, 0.0f);
        vertices.push_back(sf::Vertex(center));

        std::optional<sf::Vector2f> start;

        // i <- [-1, 1]
        for (int i = -1; i <= 1; i += 2) {
            const float side = static_cast<float>(i);
            // This is not relative to the center point yet. Could
            // also think of this as an extent.
            const float radius_center = (ProgressBar::WIDTH * 0.5 - ProgressBar::RADIUS) * side;
            float theta = PI / 2.0;
            if (i == 1) theta *= 3.0;
            for (size_t _ = 0; _ < round_sides; ++_) {
                sf::Vector2f point(radius_center + center.x, center.y);
                // Could use `sincosf` here if you're fine with non-standard.
                point.x += std::cos(theta) * ProgressBar::RADIUS;
                point.y += std::sin(theta) * ProgressBar::RADIUS;
                theta += theta_delta;
                vertices.push_back(sf::Vertex(point));
                // Store the first vertex so we can define the botom edge of the
                // shape.
                if (!start.has_value()) {
                    start = {point};
                }
            }
        }
        vertices.push_back(sf::Vertex(start.value()));
    }

    void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        sf::Shader::bind(&shader);
        // Note: SFML handles filling the faces, but expects that the first vertex
        // be the center of the fan.
        states.transform = this->getTransform();
        target.draw(&vertices[0], vertices.size(), sf::TriangleFan, states);

        sf::Shader::bind(NULL);
    }
}

