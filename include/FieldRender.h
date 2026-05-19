#ifndef CRYSTAL_FIELDRENDER_H
#define CRYSTAL_FIELDRENDER_H

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "PhaseField.h"

class FieldRenderer {
private:
    std::uint64_t nx, ny;
    float scale;

    sf::RenderWindow window;
    sf::Image image;

public:
    FieldRenderer(std::uint64_t nx, std::uint64_t ny, float scale = 0.5f);

    bool isOpen() const;

    void handleEvents();

    void render(const PhaseField &field);
};

#endif //CRYSTAL_FIELDRENDER_H
