#ifndef CRYSTAL_FIELDRENDER_H
#define CRYSTAL_FIELDRENDER_H

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "PhaseField.h"
#include "TemperatureField.h"

class FieldRenderer {
public:
    // Режимы визуализации
    enum class ColorMode {
        BINARY = 0,
        GRAYSCALE = 1,
        ICE_BLUE = 2,
        FIRE = 3,
        VIRIDIS = 4,
        PLASMA = 5,
        TEMPERATURE = 6,
        DUAL = 7
    };

private:
    std::uint64_t nx, ny;
    float scale;
    ColorMode color_mode_ = ColorMode::ICE_BLUE;
    bool show_interface_glow_ = true;

    sf::RenderWindow window;
    sf::Image image;
    sf::Font font_;
    bool has_font_ = false;

    static sf::Color paletteGrayscale(float t);

    static sf::Color paletteIceBlue(float t);

    static sf::Color paletteFire(float t);

    static sf::Color paletteViridis(float t);

    static sf::Color palettePlasma(float t);

    static sf::Color paletteTemperature(float u);

    sf::Color pixelColor(float phi, float u) const;

public:
    FieldRenderer(std::uint64_t nx, std::uint64_t ny, float scale = 0.5f);

    bool isOpen() const;

    void handleEvents();

    void render(const PhaseField &field);

    void render(const PhaseField &field, const TemperatureField &tfield);

    const sf::Image &getImage() const;

    void setColorMode(ColorMode mode) { color_mode_ = mode; }
    ColorMode getColorMode() const { return color_mode_; }
    void toggleInterfaceGlow() { show_interface_glow_ = !show_interface_glow_; }
};

#endif // CRYSTAL_FIELDRENDER_H
