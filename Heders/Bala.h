#pragma once

#include <SFML/Graphics.hpp>

class Bala {
public:
    Bala(sf::Texture& textura);

    void disparar(float posicionJugadorX);
    void actualizar();
    void dibujar(sf::RenderWindow& ventana);
    void reiniciar();

    bool estaDisparada() const;
    sf::Vector2f obtenerPosicion() const;

private:
    sf::Sprite sprite;
    bool disparada = false;
};