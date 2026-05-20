#include "../Heders/Juego.h"
#include "../Heders/Configuracion.h"
#include "../Heders/Utilidades.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

Juego::Juego()
    : ventana(sf::VideoMode(ANCHO_PANTALLA, ALTO_PANTALLA), "Destructor Espacial"),
      fondo() {

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    cargarRecursos();

    fondo.setTexture(texturaFondo);

    jugador = new Jugador(texturaJugador);
    bala = new Bala(texturaBala);

    for (int i = 0; i < NUMERO_ENEMIGOS; i++) {
        if (i % 2 == 0) {
            enemigos.push_back(new Enemigo(texturaEnemigo1));
        } else {
            enemigos.push_back(new Enemigo(texturaEnemigo2));
        }
    }

    musicaFondo.setLoop(true);
    musicaFondo.play();

    ventana.setFramerateLimit(120);
}

void Juego::cargarRecursos() {
    if (!texturaFondo.loadFromFile("assets/images/background.png")) {
        std::cout << "Error al cargar fondo\n";
    }

    if (!texturaJugador.loadFromFile("assets/images/space-invaders.png")) {
        std::cout << "Error al cargar jugador\n";
    }

    if (!texturaBala.loadFromFile("assets/images/bullet.png")) {
        std::cout << "Error al cargar bala\n";
    }

    if (!texturaEnemigo1.loadFromFile("assets/images/enemy1.png")) {
        std::cout << "Error al cargar enemigo 1\n";
    }

    if (!texturaEnemigo2.loadFromFile("assets/images/enemy2.png")) {
        std::cout << "Error al cargar enemigo 2\n";
    }

    if (!fuente.loadFromFile("assets/fonts/comicbd.ttf")) {
        std::cout << "Error al cargar fuente\n";
    }

    if (!fuenteGameOver.loadFromFile("assets/fonts/armalite.TTF")) {
        std::cout << "Error al cargar fuente Game Over\n";
    }

    if (!musicaFondo.openFromFile("assets/audios/background_music.wav")) {
        std::cout << "Error al cargar musica\n";
    }
}

void Juego::ejecutar() {
    while (ventana.isOpen()) {
        procesarEventos();

        if (!juegoTerminado) {
            actualizar();
        }

        dibujar();
    }
}

void Juego::procesarEventos() {
    sf::Event evento;

    while (ventana.pollEvent(evento)) {
        if (evento.type == sf::Event::Closed) {
            ventana.close();
        }
    }

    if (!juegoTerminado) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            jugador->moverIzquierda();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            jugador->moverDerecha();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            bala->disparar(jugador->obtenerPosicion().x);
        }
    }
}

void Juego::actualizar() {
    jugador->actualizar();

    for (Enemigo* enemigo : enemigos) {
        if (enemigo->obtenerPosicion().y > 440) {
            juegoTerminado = true;
            return;
        }

        enemigo->actualizar();

        if (bala->estaDisparada() &&
            hayColision(enemigo->obtenerPosicion(), bala->obtenerPosicion())) {

            bala->reiniciar();
            puntuacion++;
            enemigo->reiniciar();
        }
    }

    bala->actualizar();
}

void Juego::dibujar() {
    ventana.clear();

    ventana.draw(fondo);

    for (Enemigo* enemigo : enemigos) {
        enemigo->dibujar(ventana);
    }

    bala->dibujar(ventana);
    jugador->dibujar(ventana);

    mostrarPuntuacion();

    if (juegoTerminado) {
        mostrarGameOver();
    }

    ventana.display();
}

void Juego::mostrarPuntuacion() {
    sf::Text texto;
    texto.setFont(fuente);
    texto.setString("PUNTUACION " + std::to_string(puntuacion));
    texto.setCharacterSize(32);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(10.f, 10.f);

    ventana.draw(texto);
}

void Juego::mostrarGameOver() {
    sf::Text texto;
    texto.setFont(fuenteGameOver);
    texto.setString("JUEGO TERMINADO");
    texto.setCharacterSize(60);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(110.f, 260.f);

    ventana.draw(texto);
}