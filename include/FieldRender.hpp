#ifndef CRYSTAL_FIELDRENDER_H
#define CRYSTAL_FIELDRENDER_H

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "PhaseField.h"

class FieldRenderer {
private:
    std::uint64_t nx, ny;
    int scale;

    sf::RenderWindow window;
    sf::Image image;

public:
    FieldRenderer(std::uint64_t nx, std::uint64_t ny, int scale = 4)
        : nx(nx), ny(ny), scale(scale),
          window(sf::VideoMode({(unsigned)(nx * scale),
                                (unsigned)(ny * scale)}),
                 "Phase Field")
    {
        image.resize({(unsigned)nx, (unsigned)ny}, sf::Color::Black);
        window.setFramerateLimit(60);
    }

    bool isOpen() const {
        return window.isOpen();
    }

    void handleEvents() {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
    }

    void render(const PhaseField& field) {

        // 🔹 просто красим пиксели
        for (std::uint64_t i = 0; i < nx; ++i) {
            for (std::uint64_t j = 0; j < ny; ++j) {

                double phi = std::clamp(field.at(i, j), 0.0, 1.0);

                std::uint8_t c = static_cast<std::uint8_t>(phi * 255);

                image.setPixel(
                    {(unsigned)i, (unsigned)j},
                    sf::Color(c, c, c)   // grayscale
                );
            }
        }

        // 🔥 создаём texture заново (самый надёжный способ)
        sf::Texture texture(image);
        sf::Sprite sprite(texture);

        sprite.setScale({(float)scale, (float)scale});

        window.clear();
        window.draw(sprite);
        window.display();
    }
};

#endif //CRYSTAL_FIELDRENDER_H