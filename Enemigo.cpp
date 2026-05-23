#include "Enemigo.h"
#include "Configuracion.h"
#include <cstdlib>

Enemigo::Enemigo(sf::Texture& textura, float anchoVentana, float velocidad)
    : sprite(textura),
      cambioX(velocidad),
      anchoVentana(anchoVentana),
      velocidad(velocidad) {

    sprite.setScale({2.0f, 2.0f});
    reiniciar();
}

void Enemigo::actualizar() {
    sprite.move({cambioX, 0.f});

    sf::Vector2f posicion = sprite.getPosition();

    if (posicion.x <= 0) {
        cambioX = velocidad;
        sprite.move({0.f, BAJADA_ENEMIGO});
    }

    else if (posicion.x >= anchoVentana - sprite.getGlobalBounds().width) {
        cambioX = -velocidad;
        sprite.move({0.f, BAJADA_ENEMIGO});
    }
}

void Enemigo::dibujar(sf::RenderWindow& ventana) {
    ventana.draw(sprite);
}

void Enemigo::reiniciar() {

    float limiteX = anchoVentana - sprite.getGlobalBounds().width;

    float x = static_cast<float>(std::rand() % static_cast<int>(limiteX));
    float y = static_cast<float>(std::rand() % 180);

    sprite.setPosition({x, y});

    sprite.setPosition({x, y});
}

sf::Vector2f Enemigo::obtenerPosicion() const {
    return sprite.getPosition();
}

void Enemigo::cambiarVelocidad(float nuevaVelocidad) {
    velocidad = nuevaVelocidad;

    if (cambioX > 0) {
        cambioX = velocidad;
    }
    else {
        cambioX = -velocidad;
    }
}

float Enemigo::obtenerVelocidad() const {
    return velocidad;
}

sf::FloatRect Enemigo::obtenerBounds() const {
    return sprite.getGlobalBounds();
}