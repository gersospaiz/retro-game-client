#pragma once

#include <SFML/Graphics.hpp>

class BalaEnemiga {
public:
    BalaEnemiga(sf::Texture& textura);
    sf::FloatRect obtenerBounds() const;

    void disparar(sf::Vector2f posicion);
    void actualizar();
    void dibujar(sf::RenderWindow& ventana);

    bool activa() const;

    sf::Vector2f obtenerPosicion() const;

    void reiniciar();

private:
    sf::Sprite sprite;



    bool disparada = false;
};