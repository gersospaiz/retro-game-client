#include "Jugador.h"
#include "Configuracion.h"

Jugador::Jugador(sf::Texture& textura, float anchoVentana, float altoVentana)
    : sprite(textura) {

    sprite.setPosition({
        anchoVentana / 2.f - sprite.getGlobalBounds().width / 2.f,
        altoVentana - sprite.getGlobalBounds().height - 40.f
    });
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

    if (posicion.x > ANCHO_PANTALLA - 64) {
        sprite.setPosition({736.f, posicion.y});
    }
}

void Jugador::dibujar(sf::RenderWindow& ventana) {
    ventana.draw(sprite);
}

sf::Vector2f Jugador::obtenerPosicion() const {
    return sprite.getPosition();
}

sf::FloatRect Jugador::obtenerBounds() const {

    sf::FloatRect bounds = sprite.getGlobalBounds();

    bounds.left += 25.f;
    bounds.top += 20.f;
    bounds.width -= 50.f;
    bounds.height -= 30.f;

    return bounds;
}