#include "Enemigo.h"
#include "Configuracion.h"
#include <cstdlib>

Enemigo::Enemigo(sf::Texture& textura) : sprite(textura), cambioX(VELOCIDAD_ENEMIGO) {
    reiniciar();
}

void Enemigo::actualizar() {
    sprite.move({cambioX, 0.f});

    sf::Vector2f posicion = sprite.getPosition();

    if (posicion.x <= 0) {
        cambioX = VELOCIDAD_ENEMIGO;
        sprite.move({0.f, BAJADA_ENEMIGO});
    }

    else if (posicion.x >= 736) {
        cambioX = -VELOCIDAD_ENEMIGO;
        sprite.move({0.f, BAJADA_ENEMIGO});
    }
}

void Enemigo::dibujar(sf::RenderWindow& ventana) {
    ventana.draw(sprite);
}

void Enemigo::reiniciar() {
    float x = static_cast<float>(std::rand() % 737);
    float y = static_cast<float>(std::rand() % 151);

    sprite.setPosition({x, y});
}

sf::Vector2f Enemigo::obtenerPosicion() const {
    return sprite.getPosition();
}