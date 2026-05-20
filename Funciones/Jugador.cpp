#include "../Heders/Jugador.h"
#include "../Heders/Configuracion.h"

Jugador::Jugador(sf::Texture& textura) : sprite(textura) {
    sprite.setPosition({370.f, 470.f});
}

void Jugador::moverIzquierda() {
    sprite.move({-VELOCIDAD_JUGADOR, 0.f});
}

void Jugador::moverDerecha() {
    sprite.move({VELOCIDAD_JUGADOR, 0.f});
}

void Jugador::actualizar() {
    sf::Vector2f posicion = sprite.getPosition();

    if (posicion.x < 0) {
        sprite.setPosition({0.f, posicion.y});
    }

    if (posicion.x > 736) {
        sprite.setPosition({736.f, posicion.y});
    }
}

void Jugador::dibujar(sf::RenderWindow& ventana) {
    ventana.draw(sprite);
}

sf::Vector2f Jugador::obtenerPosicion() const {
    return sprite.getPosition();
}