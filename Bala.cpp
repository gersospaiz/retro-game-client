#include "Bala.h"
#include "Configuracion.h"

Bala::Bala(sf::Texture& textura) : sprite(textura) {
    sprite.setPosition({0.f, 1000.f});
}

void Bala::disparar(float posicionJugadorX, float posicionJugadorY) {   //CENTRAR BALA
    if (!disparada) {
        sprite.setPosition({
            posicionJugadorX + 95.f, //MOVER
            posicionJugadorY + 10.f // SUBIR O BAJAR
        });

        disparada = true;
    }
}

void Bala::actualizar() {
    if (disparada) {
        sprite.move({0.f, -VELOCIDAD_BALA});
    }

    if (sprite.getPosition().y < 0) {  // BALA SALE DE PANTALLA
        reiniciar();
    }
}

void Bala::dibujar(sf::RenderWindow& ventana) {
    if (disparada) {
        ventana.draw(sprite);
    }
}

void Bala::reiniciar() {
    sprite.setPosition({0.f, 1000.f});
    disparada = false;
}

bool Bala::estaDisparada() const {
    return disparada;
}

sf::Vector2f Bala::obtenerPosicion() const {
    return sprite.getPosition();
}
sf::FloatRect Bala::obtenerBounds() const {
    return sprite.getGlobalBounds();
}