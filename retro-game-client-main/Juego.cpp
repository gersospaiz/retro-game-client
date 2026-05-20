#include "Juego.h"
#include "Configuracion.h"
#include "Utilidades.h"

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

    bool menu = true;

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/comicbd.ttf"))
    {
        std::cout << "Error al cargar fuente del menu" << std::endl;
    }

    // FONDO DEL MENÚ
    sf::Texture fondoTexture;
    if (!fondoTexture.loadFromFile("assets/images/background.png"))
    {
        std::cout << "Error al cargar fondo del menu" << std::endl;
    }

    sf::Sprite fondoSprite;
    fondoSprite.setTexture(fondoTexture);

    sf::Vector2u size = ventana.getSize();

    //  TITULO
    sf::Text titulo;
    titulo.setFont(font);
    titulo.setString("DESTRUCTOR ESPACIAL");
    titulo.setCharacterSize(40);
    titulo.setFillColor(sf::Color(0, 255, 255));

    sf::FloatRect titleBounds = titulo.getLocalBounds();
    titulo.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                     titleBounds.top + titleBounds.height / 2.0f);
    titulo.setPosition(size.x / 2.0f, 120);

    //  TEXTO START
    sf::Text iniciar;
    iniciar.setFont(font);
    iniciar.setString("PRESS ENTER TO START");
    iniciar.setCharacterSize(20);
    iniciar.setFillColor(sf::Color::Green);

    sf::FloatRect textBounds = iniciar.getLocalBounds();
    iniciar.setOrigin(textBounds.left + textBounds.width / 2.0f,
                      textBounds.top + textBounds.height / 2.0f);
    iniciar.setPosition(size.x / 2.0f, 280);

    sf::Clock blinkClock;
    bool showText = true;

    while (ventana.isOpen()) {

        if (menu)
        {
            sf::Event event;

            while (ventana.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    ventana.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                menu = false;
            }

            //  parpadeo retro
            if (blinkClock.getElapsedTime().asMilliseconds() > 500)
            {
                showText = !showText;
                blinkClock.restart();
            }

            ventana.clear();

            //  fondo primero
            ventana.draw(fondoSprite);

            //  textos encima
            ventana.draw(titulo);

            if (showText)
                ventana.draw(iniciar);

            ventana.display();

            continue;
        }

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


    //  CONTROLES DEL JUEGO

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


    // REINICIO (GAME OVER)

    if (juegoTerminado && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {

        juegoTerminado = false;
        puntuacion = 0;

        bala->reiniciar();

        for (auto& enemigo : enemigos)
            enemigo->reiniciar();
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

    static bool mostrar = true;
    static sf::Clock reloj;

    //  efecto parpadeo (cada 500ms)
    if (reloj.getElapsedTime().asMilliseconds() > 500) {
        mostrar = !mostrar;
        reloj.restart();
    }

    sf::Text texto;
    texto.setFont(fuenteGameOver);
    texto.setString("GAME OVER");
    texto.setCharacterSize(60);
    texto.setFillColor(sf::Color::White);

    sf::Text mensaje;
    mensaje.setFont(fuente);
    mensaje.setString("PRESIONA ENTER PARA JUGAR OTRA VEZ");
    mensaje.setCharacterSize(25);
    mensaje.setFillColor(sf::Color::Green);


    // CENTRAR GAME OVER

    sf::FloatRect bounds = texto.getLocalBounds();
    texto.setOrigin(bounds.left + bounds.width / 2.0f,
                    bounds.top + bounds.height / 2.0f);
    texto.setPosition(ANCHO_PANTALLA / 2.0f, ALTO_PANTALLA / 2.0f - 40);


    // CENTRAR MENSAJE

    sf::FloatRect bounds2 = mensaje.getLocalBounds();
    mensaje.setOrigin(bounds2.left + bounds2.width / 2.0f,
                      bounds2.top + bounds2.height / 2.0f);
    mensaje.setPosition(ANCHO_PANTALLA / 2.0f, ALTO_PANTALLA / 2.0f + 40);


    // DIBUJO CON PARPADEO

    if (mostrar) {
        ventana.draw(texto);
        ventana.draw(mensaje);
    }
}