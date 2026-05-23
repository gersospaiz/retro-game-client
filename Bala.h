#pragma once

#include <SFML/Graphics.hpp>

class Bala {
public:
    Bala(sf::Texture& textura);

    void disparar(float posicionJugadorX, float posicionJugadorY);
    void actualizar();
    void dibujar(sf::RenderWindow& ventana);
    void reiniciar();

    bool estaDisparada() const;
    sf::Vector2f obtenerPosicion() const;
    sf::FloatRect obtenerBounds() const;

private:
    sf::Sprite sprite;
    bool disparada = false;
};