#include "Juego.h"
#include "Configuracion.h"
#include "Utilidades.h"
#include "GameApiConfig.h"
#include "ApiClient.h"
#include <thread>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cctype>

static ApiClient apiClient(
    GameApiConfig::BASE_URL,
    GameApiConfig::API_KEY,
    GameApiConfig::CODIGO_JUEGO
);

static UsuarioApi usuarioApi;
static PartidaApi partidaApi;

static bool partidaIniciadaAPI = false;
static bool partidaFinalizadaAPI = false;

static int ultimoScoreReportado = 0;
static sf::Clock relojPartidaApi;

Juego::Juego()
: ventana(sf::VideoMode::getDesktopMode(),
      "Destructor Espacial",
      sf::Style::Fullscreen),
      fondo() {

    anchoVentana = static_cast<float>(ventana.getSize().x);
    altoVentana = static_cast<float>(ventana.getSize().y);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    cargarRecursos();

    fondo.setTexture(texturaFondo);
    fondo2.setTexture(texturaFondo);

    fondo2.setScale(
        (float)ventana.getSize().x / texturaFondo.getSize().x,
        (float)ventana.getSize().y / texturaFondo.getSize().y
    );

    sf::Vector2u tamanoVentana = ventana.getSize();
    sf::Vector2u tamanoTextura = texturaFondo.getSize();

    fondo.setScale(
        (float)tamanoVentana.x / tamanoTextura.x,
        (float)tamanoVentana.y / tamanoTextura.y
    );

    jugador = new Jugador(texturaJugador, anchoVentana, altoVentana);
    bala = new Bala(texturaBala);

    for (int i = 0; i < NUMERO_ENEMIGOS; i++) {

        if (i % 3 == 0) {
            enemigos.push_back(new Enemigo(texturaAsteroide, anchoVentana, 2.0f));
        }
        else if (i % 3 == 1) {
            enemigos.push_back(new Enemigo(texturaEnemigo1, anchoVentana, 5.0f));
        }
        else {
            enemigos.push_back(new Enemigo(texturaEnemigo2, anchoVentana, 7.0f));
        }
    }

    for (int i = 0; i < 20; i++) {
        balasEnemigas.push_back(new BalaEnemiga(texturaBalaEnemiga));
    }

    musicaFondo.setLoop(true);
    musicaFondo.play();

    ventana.setFramerateLimit(120);
}

// =========================
// RECURSOS
// =========================

void Juego::cargarRecursos() {
    texturaFondo.loadFromFile("assets/images/background.png");
    texturaJugador.loadFromFile("assets/images/space-invaders.png");
    texturaBala.loadFromFile("assets/images/bullet.png");
    texturaEnemigo1.loadFromFile("assets/images/enemigo1.png");
    texturaEnemigo2.loadFromFile("assets/images/enemigo2.png");
    texturaAsteroide.loadFromFile("assets/images/asteroide.png");
    texturaExplosion.loadFromFile("assets/images/explosion.png");
    texturaBalaEnemiga.loadFromFile("assets/images/balaenemiga.png");

    fuente.loadFromFile("assets/fonts/comicbd.ttf");
    fuenteGameOver.loadFromFile("assets/fonts/armalite.TTF");

    musicaFondo.openFromFile("assets/audios/background_music.ogg");
    bufferDisparo.loadFromFile("assets/audios/disparo.wav");
    bufferExplosion.loadFromFile("assets/audios/explosion.wav");

    sonidoExplosion.setBuffer(bufferExplosion);
    sonidoDisparo.setBuffer(bufferDisparo);
}

// =========================
// LOOP PRINCIPAL
// =========================

void Juego::ejecutar() {
    while (ventana.isOpen()) {
        procesarEventos();

        if (estado == JUGANDO) {
            actualizar();
        }

        dibujar();
    }
}

// =========================
// EVENTOS
// =========================

void Juego::procesarEventos() {
    sf::Event evento;

    while (ventana.pollEvent(evento)) {

        if (evento.type == sf::Event::Closed) {
            ventana.close();
        }

        if (estado == INGRESANDO_NOMBRE) {
            procesarIngresoNombre(evento);
        }
        else if (estado == INGRESANDO_CONTRASENA) {
            procesarIngresoContrasena(evento);
        }
        else if (estado == MENU) {
            procesarMenu(evento);
        }
        else if (estado == PUNTAJES) {
            procesarPuntajes(evento);
        }
        else if (estado == GAME_OVER) {
            procesarGameOver(evento);
        }
        else if (estado == PAUSA) {
            procesarPausa(evento);
        }

        if (evento.type == sf::Event::KeyPressed &&
            evento.key.code == sf::Keyboard::P) {

            if (estado == JUGANDO) {
                estado = PAUSA;
            }
            else if (estado == PAUSA) {
                estado = JUGANDO;
            }
        }
    }

    if (estado == JUGANDO) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            jugador->moverIzquierda();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            jugador->moverDerecha();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

            if (!bala->estaDisparada()) {
                bala->disparar(
                    jugador->obtenerPosicion().x,
                    jugador->obtenerPosicion().y
                );

                sonidoDisparo.play();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            reiniciarPartida();
        }
    }
}


void Juego::actualizar() {

    if (invulnerable &&
        relojInvulnerable.getElapsedTime().asSeconds() > 1.0f) {
        invulnerable = false;
    }

    posicionFondoY += 0.5f;

    if (posicionFondoY >= altoVentana) {
        posicionFondoY = 0.f;
    }

    fondo.setPosition(0.f, posicionFondoY - altoVentana);
    fondo2.setPosition(0.f, posicionFondoY);

    jugador->actualizar();

    for (int i = 0; i < enemigos.size(); i++) {

        if (i % 3 == 0 && puntuacion < 10) {
            continue;
        }

        Enemigo* enemigo = enemigos[i];

        if (enemigo->obtenerPosicion().y > altoVentana - 160) {

            vidas--;

            if (vidas <= 0) {
                vidas = 0;

                guardarPuntaje();

                if (partidaIniciadaAPI && !partidaFinalizadaAPI) {

                    int tokensGanados = puntuacion / 2;

                    int duracion = static_cast<int>(
                        relojPartidaApi.getElapsedTime().asSeconds()
                    );

                    std::string errorApi;

                    int scoreFinal = puntuacion;
                    int duracionFinal = duracion;
                    int tokensFinales = tokensGanados;
                    long long idPartidaFinal = partidaApi.idPartida;

                                    std::thread([this, idPartidaFinal, scoreFinal, duracionFinal, tokensFinales]() {
                    std::string errorApi;

                    apiClient.finalizarPartida(
                        idPartidaFinal,
                        scoreFinal,
                        1,
                        "LOSE",
                        duracionFinal,
                        tokensFinales,
                        errorApi
                    );
                }).detach();

                    partidaFinalizadaAPI = true;
                    partidaIniciadaAPI = false;
                }

                estado = GAME_OVER;
                return;
            }

            enemigo->reiniciar();
        }

        enemigo->actualizar();

        // COLISION ENEMIGO CONTRA JUGADOR
        if (!invulnerable &&
            enemigo->obtenerBounds().intersects(jugador->obtenerBounds())) {

            vidas--;

            if (vidas <= 0) {
                vidas = 0;

                guardarPuntaje();

                if (partidaIniciadaAPI && !partidaFinalizadaAPI) {

                    int tokensGanados = puntuacion / 2;

                    int duracion = static_cast<int>(
                        relojPartidaApi.getElapsedTime().asSeconds()
                    );

                    std::string errorApi;

                    int scoreFinal = puntuacion;
                    int duracionFinal = duracion;
                    int tokensFinales = tokensGanados;
                    long long idPartidaFinal = partidaApi.idPartida;

                                        std::thread([this, idPartidaFinal, scoreFinal, duracionFinal, tokensFinales]() {
                        std::string errorApi;

                        apiClient.finalizarPartida(
                            idPartidaFinal,
                            scoreFinal,
                            1,
                            "LOSE",
                            duracionFinal,
                            tokensFinales,
                            errorApi
                        );
                    }).detach();

                    partidaFinalizadaAPI = true;
                    partidaIniciadaAPI = false;
                }

                estado = GAME_OVER;
                return;
            }

            invulnerable = true;
            relojInvulnerable.restart();

            enemigo->reiniciar();
        }

        // COLISION BALA DEL JUGADOR CONTRA ENEMIGO
        if (bala->estaDisparada() &&
            enemigo->obtenerBounds().intersects(bala->obtenerBounds())) {

            spriteExplosion.setTexture(texturaExplosion);
            spriteExplosion.setScale({2.0f, 2.0f});
            spriteExplosion.setPosition(enemigo->obtenerPosicion());

            mostrarExplosion = true;
            relojExplosion.restart();

            bala->reiniciar();
            puntuacion++;
            sonidoExplosion.play();
            enemigo->reiniciar();

            // REPORTAR SCORE CADA 50 PUNTOS
            if (partidaIniciadaAPI &&
                puntuacion > 0 &&
                puntuacion % GameApiConfig::REPORTAR_CADA_PUNTOS == 0 &&
                puntuacion != ultimoScoreReportado) {

                std::string errorApi;

                apiClient.reportarScore(
                    partidaApi.idPartida,
                    puntuacion,
                    1,
                    errorApi
                );

                ultimoScoreReportado = puntuacion;
            }
        }

        if (puntuacion >= 20) {

            if (std::rand() % 500 == 0) {

                for (BalaEnemiga* balaE : balasEnemigas) {

                    if (!balaE->activa()) {
                        balaE->disparar(enemigo->obtenerPosicion());
                        break;
                    }
                }
            }
        }
    }

    if (puntuacion % 10 == 0 && puntuacion != ultimoAumentoVelocidad) {

        ultimoAumentoVelocidad = puntuacion;

        for (Enemigo* e : enemigos) {
            e->cambiarVelocidad(e->obtenerVelocidad() + 1.0f);
        }
    }

    for (BalaEnemiga* balaE : balasEnemigas) {

        balaE->actualizar();

        if (!invulnerable &&
            balaE->activa() &&
            jugador->obtenerBounds().intersects(balaE->obtenerBounds())) {

            balaE->reiniciar();

            vidas--;

            if (vidas <= 0) {
                vidas = 0;

                guardarPuntaje();

                if (partidaIniciadaAPI && !partidaFinalizadaAPI) {

                    int tokensGanados = puntuacion / 2;

                    int duracion = static_cast<int>(
                        relojPartidaApi.getElapsedTime().asSeconds()
                    );

                    std::string errorApi;

                    int scoreFinal = puntuacion;
                    int duracionFinal = duracion;
                    int tokensFinales = tokensGanados;
                    long long idPartidaFinal = partidaApi.idPartida;

                    std::thread([this, idPartidaFinal, scoreFinal, duracionFinal, tokensFinales]() {
                        std::string errorApi;

                        apiClient.finalizarPartida(
                            idPartidaFinal,
                            scoreFinal,
                            1,
                            "LOSE",
                            duracionFinal,
                            tokensFinales,
                            errorApi
                        );
                    }).detach();

                    partidaFinalizadaAPI = true;
                    partidaIniciadaAPI = false;
                }

                estado = GAME_OVER;
                return;
            }

            invulnerable = true;
            relojInvulnerable.restart();
        }
    }

    if (mostrarExplosion &&
        relojExplosion.getElapsedTime().asSeconds() > 0.2f) {
        mostrarExplosion = false;
    }

    bala->actualizar();
}

void Juego::dibujar() {
    ventana.clear();

    ventana.draw(fondo);
    ventana.draw(fondo2);

    if (estado == MENU) {
        mostrarMenu();
    }
    else if (estado == INGRESANDO_NOMBRE) {
        mostrarPantallaNombre();
    }
    else if (estado == INGRESANDO_CONTRASENA) {
        mostrarPantallaContrasena();
    }
    else if (estado == JUGANDO) {

        for (int i = 0; i < enemigos.size(); i++) {

            if (i % 3 == 0 && puntuacion < 10) {
                continue;
            }

            enemigos[i]->dibujar(ventana);
        }

        for (BalaEnemiga* balaE : balasEnemigas) {
            balaE->dibujar(ventana);
        }

        if (mostrarExplosion) {
            ventana.draw(spriteExplosion);
        }

        bala->dibujar(ventana);
        jugador->dibujar(ventana);

        mostrarPuntuacion();
        mostrarVidas();
    }
    else if (estado == PUNTAJES) {
        mostrarPuntajes();
    }
    else if (estado == PAUSA) {
        mostrarPausa();
    }
    else if (estado == GAME_OVER) {
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

    textoTokens.setFont(fuente);
    textoTokens.setString("Tokens: " + std::to_string(saldoTokens));
    textoTokens.setCharacterSize(25);
    textoTokens.setFillColor(sf::Color::Yellow);
    textoTokens.setPosition(10.f, 100.f);

    ventana.draw(texto);
    ventana.draw(textoTokens);
}

void Juego::mostrarVidas() {

    if (vidas < 0) {
        vidas = 0;
    }

    sf::Text texto;
    texto.setFont(fuente);
    texto.setString("VIDAS: " + std::to_string(vidas));
    texto.setCharacterSize(32);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(10.f, 55.f);

    ventana.draw(texto);
}

void Juego::mostrarGameOver() {

    sf::Text texto;
    texto.setFont(fuenteGameOver);
    texto.setString("JUEGO TERMINADO");
    texto.setCharacterSize(70);
    texto.setFillColor(sf::Color::White);

    texto.setPosition(
        anchoVentana / 2.f - texto.getGlobalBounds().width / 2.f,
        220.f
    );

    ventana.draw(texto);

    textoReiniciar.setFont(fuente);
    textoReiniciar.setString("JUGAR OTRA VEZ");
    textoReiniciar.setCharacterSize(45);

    if (mouseSobreReiniciar()) {
        textoReiniciar.setFillColor(sf::Color::Green);
    }
    else {
        textoReiniciar.setFillColor(sf::Color::White);
    }

    textoReiniciar.setPosition(
        anchoVentana / 2.f - textoReiniciar.getGlobalBounds().width / 2.f,
        400.f
    );

    ventana.draw(textoReiniciar);

    textoMenuPrincipal.setFont(fuente);
    textoMenuPrincipal.setString("MENU PRINCIPAL");
    textoMenuPrincipal.setCharacterSize(45);

    if (mouseSobreMenuPrincipal()) {
        textoMenuPrincipal.setFillColor(sf::Color::Yellow);
    }
    else {
        textoMenuPrincipal.setFillColor(sf::Color::White);
    }

    textoMenuPrincipal.setPosition(
        anchoVentana / 2.f - textoMenuPrincipal.getGlobalBounds().width / 2.f,
        500.f
    );

    ventana.draw(textoMenuPrincipal);
}

void Juego::mostrarMenu() {
    textoTitulo.setFont(fuenteGameOver);
    textoTitulo.setString("DESTRUCTOR ESPACIAL");
    textoTitulo.setCharacterSize(70);
    textoTitulo.setFillColor(sf::Color::White);
    textoTitulo.setPosition(
        anchoVentana / 2.f - textoTitulo.getGlobalBounds().width / 2.f,
        180.f
    );

    textoJugar.setFont(fuente);
    textoJugar.setString("JUGAR");
    textoJugar.setCharacterSize(45);

    if (mouseSobreJugar()) {
        textoJugar.setFillColor(sf::Color::Yellow);
    }
    else {
        textoJugar.setFillColor(sf::Color::White);
    }

    textoJugar.setPosition(
        anchoVentana / 2.f - textoJugar.getGlobalBounds().width / 2.f,
        350.f
    );

    textoSalir.setFont(fuente);
    textoSalir.setString("SALIR");
    textoSalir.setCharacterSize(45);

    if (mouseSobreSalir()) {
        textoSalir.setFillColor(sf::Color::Red);
    }
    else {
        textoSalir.setFillColor(sf::Color::White);
    }

    textoSalir.setPosition(
        anchoVentana / 2.f - textoSalir.getGlobalBounds().width / 2.f,
        430.f
    );

    textoPuntajes.setFont(fuente);
    textoPuntajes.setString("PUNTAJES");
    textoPuntajes.setCharacterSize(45);

    if (mouseSobrePuntajes()) {
        textoPuntajes.setFillColor(sf::Color::Cyan);
    }
    else {
        textoPuntajes.setFillColor(sf::Color::White);
    }

    textoPuntajes.setPosition(
        anchoVentana / 2.f - textoPuntajes.getGlobalBounds().width / 2.f,
        520.f
    );
    textoCambiarUsuario.setFont(fuente);
    textoCambiarUsuario.setString("CAMBIAR USUARIO");
    textoCambiarUsuario.setCharacterSize(45);
    textoCambiarUsuario.setFillColor(sf::Color::White);

    textoCambiarUsuario.setPosition(
        anchoVentana / 2.f - textoCambiarUsuario.getGlobalBounds().width / 2.f,
        610.f
    );

    ventana.draw(textoCambiarUsuario);

    if (!mensajeAuth.empty()) {
        sf::Text mensaje;
        mensaje.setFont(fuente);
        mensaje.setString(mensajeAuth);
        mensaje.setCharacterSize(28);
        mensaje.setFillColor(sf::Color::Red);
        mensaje.setPosition(
            anchoVentana / 2.f - mensaje.getGlobalBounds().width / 2.f,
            610.f
        );
        ventana.draw(mensaje);
    }

    ventana.draw(textoTitulo);
    ventana.draw(textoJugar);
    ventana.draw(textoSalir);
    ventana.draw(textoPuntajes);
}

void Juego::procesarMenu(sf::Event evento) {

    if (evento.type == sf::Event::MouseButtonPressed) {

        if (evento.mouseButton.button == sf::Mouse::Left) {

            if (mouseSobreJugar()) {

                // MODO INVITADO: entra sin usar API
                if (!loginActivo) {
                    partidaIniciadaAPI = false;
                    partidaFinalizadaAPI = false;

                    reiniciarPartida();
                    estado = JUGANDO;
                    return;
                }

                // MODO LOGIN REAL: usa API
                std::string errorApi;

                bool ok = apiClient.iniciarPartida(
                    partidaApi,
                    errorApi,
                    GameApiConfig::VERSION_JUEGO,
                    GameApiConfig::COSTO_PARTIDA
                );

                if (ok) {

                    std::cout << "Saldo antes: "
                              << partidaApi.saldoAntes
                              << std::endl;

                    std::cout << "Saldo despues: "
                              << partidaApi.saldoDespues
                              << std::endl;

                    saldoTokens = partidaApi.saldoDespues;

                    partidaIniciadaAPI = true;
                    partidaFinalizadaAPI = false;
                    ultimoScoreReportado = 0;

                    relojPartidaApi.restart();

                    reiniciarPartida();
                    estado = JUGANDO;
                }
                else {
                    std::cout << "ERROR AL INICIAR PARTIDA: "
                              << errorApi
                              << std::endl;

                    mensajeAuth = errorApi;
                }
            }

            if (mouseSobrePuntajes()) {
                estado = PUNTAJES;
            }

            if (mouseSobreSalir()) {
                ventana.close();
            }
            if (mouseSobreCambiarUsuario()) {

                apiClient.cerrarSesion();

                nombreJugador = "";
                contrasenaJugador = "";
                saldoTokens = 0;
                mensajeAuth = "";

                partidaIniciadaAPI = false;
                partidaFinalizadaAPI = false;

                estado = INGRESANDO_NOMBRE;
            }
        }
    }
}


bool Juego::mouseSobreJugar() {
    sf::Vector2i mouse = sf::Mouse::getPosition(ventana);

    return textoJugar.getGlobalBounds().contains(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );
}

bool Juego::mouseSobreSalir() {
    sf::Vector2i mouse = sf::Mouse::getPosition(ventana);

    return textoSalir.getGlobalBounds().contains(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );
}
bool Juego::mouseSobreCambiarUsuario() {
    sf::Vector2i mouse = sf::Mouse::getPosition(ventana);

    return textoCambiarUsuario.getGlobalBounds().contains(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );
}

bool Juego::mouseSobreReiniciar() {
    sf::Vector2i mouse = sf::Mouse::getPosition(ventana);

    return textoReiniciar.getGlobalBounds().contains(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );
}

bool Juego::mouseSobreMenuPrincipal() {
    sf::Vector2i mouse = sf::Mouse::getPosition(ventana);

    return textoMenuPrincipal.getGlobalBounds().contains(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );
}

bool Juego::mouseSobrePuntajes() {
    sf::Vector2i mouse = sf::Mouse::getPosition(ventana);

    return textoPuntajes.getGlobalBounds().contains(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );
}

void Juego::procesarGameOver(sf::Event evento) {

    if (evento.type == sf::Event::MouseButtonPressed) {

        if (evento.mouseButton.button == sf::Mouse::Left) {

            if (mouseSobreReiniciar()) {
                reiniciarPartida();
                estado = MENU;
            }

            if (mouseSobreMenuPrincipal()) {
                reiniciarPartida();
                estado = MENU;
            }
        }
    }
}


void Juego::guardarPuntaje() {

    std::ofstream archivo("puntajes.txt", std::ios::app);

    if (archivo.is_open()) {

        archivo << nombreJugador
                << " - "
                << puntuacion
                << std::endl;

        archivo.close();
    }
}

void Juego::mostrarPuntajes() {

    sf::Text titulo;
    titulo.setFont(fuenteGameOver);
    titulo.setString("RANKING API");
    titulo.setCharacterSize(70);
    titulo.setFillColor(sf::Color::White);

    titulo.setPosition(
        anchoVentana / 2.f - titulo.getGlobalBounds().width / 2.f,
        100.f
    );

    ventana.draw(titulo);

    std::vector<RankingItem> ranking;
    std::string errorApi;

    bool ok = apiClient.consultarRanking(ranking, errorApi);

    float y = 230.f;

    if (!ok) {
        sf::Text error;
        error.setFont(fuente);
        error.setString("Error ranking: " + errorApi);
        error.setCharacterSize(30);
        error.setFillColor(sf::Color::Red);
        error.setPosition(200.f, y);

        ventana.draw(error);
        return;
    }

    for (int i = 0; i < ranking.size() && i < 10; i++) {

        sf::Text texto;
        texto.setFont(fuente);

        texto.setString(
            std::to_string(i + 1) + ". " +
            ranking[i].username +
            " - Score: " +
            std::to_string(ranking[i].bestScore)
        );

        texto.setCharacterSize(35);
        texto.setFillColor(sf::Color::White);
        texto.setPosition(250.f, y);

        ventana.draw(texto);

        y += 55.f;
    }
}

void Juego::procesarPuntajes(sf::Event evento) {

    if (evento.type == sf::Event::KeyPressed) {

        if (evento.key.code == sf::Keyboard::Escape) {
            estado = MENU;
        }
    }
}

void Juego::mostrarPantallaNombre() {

    textoNombre.setFont(fuenteGameOver);
    textoNombre.setString("INGRESA TU NOMBRE");
    textoNombre.setCharacterSize(60);
    textoNombre.setFillColor(sf::Color::White);

    textoNombre.setPosition(
        anchoVentana / 2.f - textoNombre.getGlobalBounds().width / 2.f,
        220.f
    );

    ventana.draw(textoNombre);

    textoInputNombre.setFont(fuente);
    textoInputNombre.setString(nombreJugador + "_");
    textoInputNombre.setCharacterSize(50);
    textoInputNombre.setFillColor(sf::Color::Cyan);

    textoInputNombre.setPosition(
        anchoVentana / 2.f - textoInputNombre.getGlobalBounds().width / 2.f,
        380.f
    );

    ventana.draw(textoInputNombre);
}

void Juego::procesarIngresoNombre(sf::Event evento) {

    if (evento.type == sf::Event::TextEntered) {

        if (evento.text.unicode == '\b') {

            if (!nombreJugador.empty()) {
                nombreJugador.pop_back();
            }
        }
        else if (evento.text.unicode < 128 &&
                 nombreJugador.size() < 12) {

            char letra = static_cast<char>(evento.text.unicode);

            if (std::isalnum(letra) || letra == ' ') {
                nombreJugador += letra;
            }
        }
    }

    if (evento.type == sf::Event::KeyPressed) {

        if (evento.key.code == sf::Keyboard::Enter &&
            !nombreJugador.empty()) {

            estado = INGRESANDO_CONTRASENA;
        }
    }
}


// LOGIN CONTRASENA

void Juego::mostrarPantallaContrasena() {

    textoContrasena.setFont(fuenteGameOver);
    textoContrasena.setString("INGRESA TU CONTRASENA");
    textoContrasena.setCharacterSize(60);
    textoContrasena.setFillColor(sf::Color::White);

    textoContrasena.setPosition(
        anchoVentana / 2.f - textoContrasena.getGlobalBounds().width / 2.f,
        220.f
    );

    ventana.draw(textoContrasena);

    std::string asteriscos(contrasenaJugador.size(), '*');

    textoInputContrasena.setFont(fuente);
    textoInputContrasena.setString(asteriscos + "_");
    textoInputContrasena.setCharacterSize(50);
    textoInputContrasena.setFillColor(sf::Color::Cyan);

    textoInputContrasena.setPosition(
        anchoVentana / 2.f - textoInputContrasena.getGlobalBounds().width / 2.f,
        380.f
    );

    ventana.draw(textoInputContrasena);

    if (!mensajeAuth.empty()) {
        sf::Text mensaje;
        mensaje.setFont(fuente);
        mensaje.setString(mensajeAuth);
        mensaje.setCharacterSize(28);
        mensaje.setFillColor(sf::Color::Red);

        mensaje.setPosition(
            anchoVentana / 2.f - mensaje.getGlobalBounds().width / 2.f,
            470.f
        );

        ventana.draw(mensaje);
    }
}

void Juego::procesarIngresoContrasena(sf::Event evento) {

    if (evento.type == sf::Event::TextEntered) {

        if (evento.text.unicode == '\b') {

            if (!contrasenaJugador.empty()) {
                contrasenaJugador.pop_back();
            }
        }
        else if (evento.text.unicode < 128 &&
                 evento.text.unicode != '\r' &&
                 contrasenaJugador.size() < 16) {

            char letra = static_cast<char>(evento.text.unicode);

            if (std::isalnum(letra) || letra == '_') {
                contrasenaJugador += letra;
            }
        }
    }

    if (evento.type == sf::Event::KeyPressed &&
        evento.key.code == sf::Keyboard::Enter) {

        if (contrasenaJugador.size() >= 4)
        {
            std::string errorApi;

            if (contrasenaJugador.size() >= 4 || !loginActivo) {

                bool ok = false;
                std::string errorApi;

                if (loginActivo) {
                    ok = apiClient.loginJugador(
                        nombreJugador,
                        contrasenaJugador,
                        usuarioApi,
                        errorApi
                    );

                    if (ok) {
                        saldoTokens = usuarioApi.saldoTokens;
                    }
                }
                else {
                    ok = true;
                    nombreJugador = "INVITADO";
                    saldoTokens = 999;
                }

                if (ok) {
                    mensajeAuth = "";
                    estado = MENU;
                }
                else {
                    mensajeAuth = "Usuario o contrasena incorrectos";
                    contrasenaJugador = "";
                }
            }
            else
            {
                mensajeAuth = "LA CONTRASENA DEBE TENER MINIMO 4 CARACTERES";
            }
        }
    }
}


// PAUSA

void Juego::mostrarPausa() {
    ventana.draw(fondo);
    ventana.draw(fondo2);

    sf::Text texto;
    texto.setFont(fuenteGameOver);
    texto.setString("PAUSA");
    texto.setCharacterSize(80);
    texto.setFillColor(sf::Color::White);

    texto.setPosition(
        anchoVentana / 2.f - texto.getGlobalBounds().width / 2.f,
        250.f
    );

    ventana.draw(texto);

    sf::Text instrucciones;
    instrucciones.setFont(fuente);
    instrucciones.setString("P = CONTINUAR   |   R = REINICIAR   |   ESC = MENU");
    instrucciones.setCharacterSize(35);
    instrucciones.setFillColor(sf::Color::White);

    instrucciones.setPosition(
        anchoVentana / 2.f - instrucciones.getGlobalBounds().width / 2.f,
        390.f
    );

    ventana.draw(instrucciones);
}

void Juego::procesarPausa(sf::Event evento) {

    if (evento.type == sf::Event::KeyPressed) {

        if (evento.key.code == sf::Keyboard::R) {
            reiniciarPartida();
            estado = JUGANDO;
        }

        if (evento.key.code == sf::Keyboard::Escape) {
            reiniciarPartida();
            estado = MENU;
        }
    }
}


// REINICIAR PARTIDA

void Juego::reiniciarPartida() {

    puntuacion = 0;
    vidas = 3;
    ultimoAumentoVelocidad = 0;
    mostrarExplosion = false;
    invulnerable = false;

    bala->reiniciar();

    for (int i = 0; i < enemigos.size(); i++) {

        if (i % 3 == 0) {
            enemigos[i]->cambiarVelocidad(2.0f);
        }
        else if (i % 3 == 1) {
            enemigos[i]->cambiarVelocidad(5.0f);
        }
        else {
            enemigos[i]->cambiarVelocidad(7.0f);
        }

        enemigos[i]->reiniciar();
    }
}