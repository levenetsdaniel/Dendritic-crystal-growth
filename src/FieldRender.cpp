#include "FieldRender.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

FieldRenderer::FieldRenderer(std::uint64_t nx, std::uint64_t ny, float scale)
    : nx(nx), ny(ny), scale(scale),
      window(sf::VideoMode({
                 static_cast<unsigned>(nx * scale),
                 static_cast<unsigned>(ny * scale)
             }),
             "Phase Field (Karma-Rappel)") {
    image.resize({static_cast<unsigned>(nx), static_cast<unsigned>(ny)}, sf::Color::Black);
    window.setFramerateLimit(60);
}

bool FieldRenderer::isOpen() const { return window.isOpen(); }

void FieldRenderer::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            window.close();
    }
}

void FieldRenderer::render(const PhaseField &field) {
    for (std::uint64_t i = 0; i < nx; ++i) {
        for (std::uint64_t j = 0; j < ny; ++j) {
            const float phi = std::clamp(field.at(i, j), -1.0f, 1.0f);
            const std::uint8_t c = (phi > 0.0f) ? 255 : 0;
            image.setPixel({static_cast<unsigned>(i), static_cast<unsigned>(j)},
                           sf::Color(c, c, c));
        }
    }

    sf::Texture texture(image);
    sf::Sprite sprite(texture);
    sprite.setScale({scale, scale});

    window.clear();
    window.draw(sprite);
    window.display();
}

const sf::Image& FieldRenderer::getImage() const
{
    return image;
}