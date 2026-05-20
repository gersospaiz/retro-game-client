#include "../Heders/Bala.h"
#include "../Heders/Configuracion.h"

Bala::Bala(sf::Texture& textura) : sprite(textura) {
    sprite.setPosition({0.f, 480.f});
}

void Bala::disparar(float posicionJugadorX) {
    if (!disparada) {
        sprite.setPosition({posicionJugadorX + 16.f, 480.f});
        disparada = true;
    }
}

void Bala::actualizar() {
    if (disparada) {
        sprite.move({0.f, -VELOCIDAD_BALA});
    }

    if (sprite.getPosition().y < 0) {
        reiniciar();
    }
}

void Bala::dibujar(sf::RenderWindow& ventana) {
    if (disparada) {
        ventana.draw(sprite);
    }
}

void Bala::reiniciar() {
    sprite.setPosition({0.f, 480.f});
    disparada = false;
}

bool Bala::estaDisparada() const {
    return disparada;
}

sf::Vector2f Bala::obtenerPosicion() const {
    return sprite.getPosition();
}