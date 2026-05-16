#pragma once

#include <SFML/Graphics.hpp>

class Jugador {
public:
    Jugador(sf::Texture& textura);

    void moverIzquierda();
    void moverDerecha();
    void actualizar();
    void dibujar(sf::RenderWindow& ventana);

    sf::Vector2f obtenerPosicion() const;

private:
    sf::Sprite sprite;
};