#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>

#include "Jugador.h"
#include "Enemigo.h"
#include "Bala.h"

class Juego {
public:
    Juego();
    void ejecutar();

private:
    sf::RenderWindow ventana;

    sf::Texture texturaFondo;
    sf::Texture texturaJugador;
    sf::Texture texturaBala;
    sf::Texture texturaEnemigo1;
    sf::Texture texturaEnemigo2;

    sf::Sprite fondo;

    sf::Font fuente;
    sf::Font fuenteGameOver;

    sf::Music musicaFondo;

    Jugador* jugador;
    Bala* bala;
    std::vector<Enemigo*> enemigos;

    int puntuacion = 0;
    bool juegoTerminado = false;

    void cargarRecursos();
    void procesarEventos();
    void actualizar();
    void dibujar();

    void mostrarPuntuacion();
    void mostrarGameOver();
};