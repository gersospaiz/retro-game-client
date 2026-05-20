#pragma once

#include <SFML/Graphics.hpp>

class Enemigo {
public:
    Enemigo(sf::Texture& textura);

    void actualizar();
    void dibujar(sf::RenderWindow& ventana);
    void reiniciar();

    sf::Vector2f obtenerPosicion() const;

private:
    sf::Sprite sprite;
    float cambioX;
};