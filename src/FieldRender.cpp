#include "FieldRender.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>


static std::uint8_t to8(float v) {
    return static_cast<std::uint8_t>(std::clamp(v, 0.0f, 1.0f) * 255.0f);
}

static float normPhi(float phi) {
    return 0.5f * (std::clamp(phi, -1.0f, 1.0f) + 1.0f);
}


sf::Color FieldRenderer::paletteGrayscale(float t) {
    const std::uint8_t v = to8(t);
    return sf::Color(v, v, v);
}

sf::Color FieldRenderer::paletteIceBlue(float t) {
    const float r = 0.05f + 0.85f * t * t;
    const float g = 0.10f + 0.80f * std::pow(t, 1.3f);
    const float b = 0.20f + 0.75f * std::pow(t, 0.6f);
    return sf::Color(to8(r), to8(g), to8(b));
}

sf::Color FieldRenderer::paletteFire(float t) {
    float r, g, b;
    if (t < 0.33f) {
        const float k = t / 0.33f;
        r = k;
        g = 0.0f;
        b = 0.0f;
    } else if (t < 0.66f) {
        const float k = (t - 0.33f) / 0.33f;
        r = 1.0f;
        g = k;
        b = 0.0f;
    } else {
        const float k = (t - 0.66f) / 0.34f;
        r = 1.0f;
        g = 1.0f;
        b = k;
    }
    return sf::Color(to8(r), to8(g), to8(b));
}

sf::Color FieldRenderer::paletteViridis(float t) {
    const float r = 0.267f + t * (0.105f + t * (6.225f + t * (-13.954f + t * 9.357f)));
    const float g = 0.005f + t * (1.404f + t * (-1.515f + t * (1.041f + t * -0.232f)));
    const float b = 0.329f + t * (1.385f + t * (-3.143f + t * (3.272f + t * -1.111f)));
    return sf::Color(to8(r), to8(g), to8(b));
}

sf::Color FieldRenderer::palettePlasma(float t) {
    const float r = 0.050f + t * (2.331f + t * (-4.099f + t * (4.555f + t * -1.836f)));
    const float g = 0.029f + t * (-0.291f + t * (1.728f + t * (-1.180f + t * 0.716f)));
    const float b = 0.527f + t * (1.466f + t * (-4.527f + t * (4.491f + t * -1.957f)));
    return sf::Color(to8(r), to8(g), to8(b));
}

sf::Color FieldRenderer::paletteTemperature(float u) {
    const float t = std::clamp((u + 1.0f) * 0.5f, 0.0f, 1.0f);
    const float r = t;
    const float g = 0.3f + 0.4f * (1.0f - std::abs(2.0f * t - 1.0f));
    const float b = 1.0f - t;
    return sf::Color(to8(r), to8(g), to8(b));
}

sf::Color FieldRenderer::pixelColor(float phi, float u) const {
    const float t = normPhi(phi);

    sf::Color base;
    switch (color_mode_) {
        case ColorMode::BINARY: {
            const std::uint8_t c = (phi > 0.0f) ? 255 : 0;
            return sf::Color(c, c, c);
        }
        case ColorMode::GRAYSCALE: base = paletteGrayscale(t);
            break;
        case ColorMode::ICE_BLUE: base = paletteIceBlue(t);
            break;
        case ColorMode::FIRE: base = paletteFire(t);
            break;
        case ColorMode::VIRIDIS: base = paletteViridis(t);
            break;
        case ColorMode::PLASMA: base = palettePlasma(t);
            break;
        case ColorMode::TEMPERATURE: return paletteTemperature(u);
        case ColorMode::DUAL: {
            if (phi > 0.0f) {
                base = paletteIceBlue(t);
            } else {
                base = paletteTemperature(u);
                base.r = static_cast<std::uint8_t>(base.r * 0.6f);
                base.g = static_cast<std::uint8_t>(base.g * 0.6f);
                base.b = static_cast<std::uint8_t>(base.b * 0.7f);
            }
            break;
        }
    }

    if (show_interface_glow_ && color_mode_ != ColorMode::BINARY) {
        const float interface_w = 1.0f - std::abs(phi); // = 0 на полюсах, ~1 в центре
        if (interface_w > 0.05f && interface_w < 0.95f) {
            const float glow = std::pow(interface_w, 0.5f) * 0.35f;
            const int r = std::min(255, int(base.r) + int(glow * 200));
            const int g = std::min(255, int(base.g) + int(glow * 180));
            const int b = std::min(255, int(base.b) + int(glow * 100));
            base = sf::Color(
                static_cast<std::uint8_t>(r),
                static_cast<std::uint8_t>(g),
                static_cast<std::uint8_t>(b));
        }
    }

    return base;
}

FieldRenderer::FieldRenderer(std::uint64_t nx, std::uint64_t ny, float scale)
    : nx(nx), ny(ny), scale(scale),
      window(sf::VideoMode({
                 static_cast<unsigned>(nx * scale),
                 static_cast<unsigned>(ny * scale)
             }),
             "Phase Field (Karma-Rappel) | 1-7: palette  G: glow  ESC: quit") {
    image.resize({static_cast<unsigned>(nx), static_cast<unsigned>(ny)}, sf::Color::Black);
    window.setFramerateLimit(60);

    std::cout << "\n[Renderer] Controls:\n"
            << "  1 - Binary (black/white)\n"
            << "  2 - Grayscale\n"
            << "  3 - Ice Blue (default)\n"
            << "  4 - Fire (hot metals)\n"
            << "  5 - Viridis\n"
            << "  6 - Plasma\n"
            << "  7 - Temperature field\n"
            << "  8 - Dual (phase + temperature)\n"
            << "  G - Toggle interface glow\n"
            << "  ESC - Close window\n\n";
}

bool FieldRenderer::isOpen() const { return window.isOpen(); }

void FieldRenderer::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto *keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyEvent->code) {
                case sf::Keyboard::Key::Num1: color_mode_ = ColorMode::BINARY;
                    break;
                case sf::Keyboard::Key::Num2: color_mode_ = ColorMode::GRAYSCALE;
                    break;
                case sf::Keyboard::Key::Num3: color_mode_ = ColorMode::ICE_BLUE;
                    break;
                case sf::Keyboard::Key::Num4: color_mode_ = ColorMode::FIRE;
                    break;
                case sf::Keyboard::Key::Num5: color_mode_ = ColorMode::VIRIDIS;
                    break;
                case sf::Keyboard::Key::Num6: color_mode_ = ColorMode::PLASMA;
                    break;
                case sf::Keyboard::Key::Num7: color_mode_ = ColorMode::TEMPERATURE;
                    break;
                case sf::Keyboard::Key::Num8: color_mode_ = ColorMode::DUAL;
                    break;
                case sf::Keyboard::Key::G: toggleInterfaceGlow();
                    break;
                case sf::Keyboard::Key::Escape: window.close();
                    break;
                default: break;
            }
        }
    }
}

void FieldRenderer::render(const PhaseField &field) {
#pragma omp parallel for schedule(static)
    for (std::int64_t i = 0; i < static_cast<std::int64_t>(nx); ++i) {
        for (std::uint64_t j = 0; j < ny; ++j) {
            const float phi = std::clamp(field.at(i, j), -1.0f, 1.0f);
            image.setPixel({static_cast<unsigned>(i), static_cast<unsigned>(j)},
                           pixelColor(phi, 0.0f));
        }
    }

    sf::Texture texture(image);
    sf::Sprite sprite(texture);
    sprite.setScale({scale, scale});

    window.clear(sf::Color(8, 12, 20));
    window.draw(sprite);
    window.display();
}

void FieldRenderer::render(const PhaseField &field, const TemperatureField &tfield) {
#pragma omp parallel for schedule(static)
    for (std::int64_t i = 0; i < static_cast<std::int64_t>(nx); ++i) {
        for (std::uint64_t j = 0; j < ny; ++j) {
            const float phi = std::clamp(field.at(i, j), -1.0f, 1.0f);
            const float u = tfield.at(i, j);
            image.setPixel({static_cast<unsigned>(i), static_cast<unsigned>(j)},
                           pixelColor(phi, u));
        }
    }

    sf::Texture texture(image);
    sf::Sprite sprite(texture);
    sprite.setScale({scale, scale});

    window.clear(sf::Color(8, 12, 20));
    window.draw(sprite);
    window.display();
}

const sf::Image &FieldRenderer::getImage() const {
    return image;
}
