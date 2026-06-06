#include "BalaEnemiga.h"

BalaEnemiga::BalaEnemiga(sf::Texture& textura)
    : sprite(textura) {

    sprite.setScale({2.f, 2.f});
}

void BalaEnemiga::disparar(sf::Vector2f posicion) {

    if (!disparada) {

        sprite.setPosition(
            posicion.x + 20.f,
            posicion.y + 40.f
        );

        disparada = true;
    }
}

void BalaEnemiga::actualizar() {

    if (disparada) {
        sprite.move({0.f, 8.f});
    }

    if (sprite.getPosition().y > 1200.f) {
        reiniciar();
    }
}

void BalaEnemiga::dibujar(sf::RenderWindow& ventana) {

    if (disparada) {
        ventana.draw(sprite);
    }
}

bool BalaEnemiga::activa() const {
    return disparada;
}

sf::Vector2f BalaEnemiga::obtenerPosicion() const {
    return sprite.getPosition();
}

void BalaEnemiga::reiniciar() {
    disparada = false;
}

sf::FloatRect BalaEnemiga::obtenerBounds() const {

    sf::FloatRect bounds = sprite.getGlobalBounds();

    bounds.left += 1.f;
    bounds.top += 1.f;
    bounds.width -= 1.f;
    bounds.height -= 1.f;

    return bounds;
}