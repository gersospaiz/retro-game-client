#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <array>

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
    sf::Texture texturaFondogame;

    sf::Sprite fondo;
    std::vector<sf::Sprite> iconosVidas;

    sf::Font fuente;
    sf::Font fuenteGameOver;

    sf::Music musicaFondo;

    sf::SoundBuffer bufferDisparo;
    std::array<sf::Sound, 5> sonidosDisparo;

    Jugador* jugador;
    Bala* bala;
    std::vector<Enemigo*> enemigos;

    int puntuacion = 0;
    int vidas = 3; // conteo de las vidas
    bool juegoTerminado = false;

    void cargarRecursos();
    void procesarEventos();
    void actualizar();
    void dibujar();

    void mostrarPuntuacion();
    void mostrarGameOver();

    void reiniciar();
};