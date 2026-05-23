#pragma once

#include <SFML/Graphics.hpp>

class Jugador {
public:
    Jugador(sf::Texture& textura, float anchoVentana, float altoVentana);

    void moverIzquierda();
    void moverDerecha();
    void actualizar();
    void dibujar(sf::RenderWindow& ventana);

    sf::Vector2f obtenerPosicion() const;
    sf::FloatRect obtenerBounds() const;

private:
    sf::Sprite sprite;

};