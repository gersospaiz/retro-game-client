#pragma once

#include <cmath>
#include <SFML/System.hpp>

inline bool hayColision(sf::Vector2f posicionEnemigo, sf::Vector2f posicionBala) {
    float distancia = std::sqrt(
        std::pow(posicionEnemigo.x - posicionBala.x, 2) +
        std::pow(posicionEnemigo.y - posicionBala.y, 2)
    );

    return distancia < 27;
}