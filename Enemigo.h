#pragma once

#include <SFML/Graphics.hpp>

class Enemigo {
public:
    Enemigo(sf::Texture& textura, float anchoVentana, float velocidad);
    sf::FloatRect obtenerBounds() const;

    void actualizar();
    void dibujar(sf::RenderWindow& ventana);
    void reiniciar();

    void cambiarVelocidad(float nuevaVelocidad);
    float obtenerVelocidad() const;

    sf::Vector2f obtenerPosicion() const;


private:
    sf::Sprite sprite;
    float cambioX;
    float anchoVentana;
    float velocidad;
};