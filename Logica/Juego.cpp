#include "Juego.h"
#include "Configuracion.h"
#include "Utilidades.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "json.hpp"
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

        // ASTEROIDE
        if (i % 3 == 0) {

            enemigos.push_back(
                new Enemigo(
                    texturaAsteroide,
                    anchoVentana,
                    2.0f
                )
            );
        }

        // ENEMIGO 1
        else if (i % 3 == 1) {

            enemigos.push_back(
                new Enemigo(
                    texturaEnemigo1,
                    anchoVentana,
                    5.0f
                )
            );
        }

        // ENEMIGO 2
        else {

            enemigos.push_back(
                new Enemigo(
                    texturaEnemigo2,
                    anchoVentana,
                    7.0f
                )
            );
        }
    }

    for (int i = 0; i < 20; i++) {
        balasEnemigas.push_back(
            new BalaEnemiga(texturaBalaEnemiga)
        );
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

    if (!texturaEnemigo1.loadFromFile("assets/images/enemigo1.png")) {
        std::cout << "Error al cargar enemigo 1\n";
    }

    if (!texturaEnemigo2.loadFromFile("assets/images/enemigo2.png")) {
        std::cout << "Error al cargar enemigo 2\n";
    }

    if (!texturaAsteroide.loadFromFile("assets/images/asteroide.png")) {
        std::cout << "Error al cargar asteroide\n";
    }

    if (!texturaExplosion.loadFromFile("assets/images/explosion.png")) {
        std::cout << "Error al cargar explosion\n";
    }

    if (!fuente.loadFromFile("assets/fonts/comicbd.ttf")) {
        std::cout << "Error al cargar fuente\n";
    }

    if (!fuenteGameOver.loadFromFile("assets/fonts/armalite.TTF")) {
        std::cout << "Error al cargar fuente Game Over\n";
    }

    if (!musicaFondo.openFromFile("assets/audios/background_music.ogg")) {
        std::cout << "Error al cargar musica\n";
    }

    if (!bufferDisparo.loadFromFile("assets/audios/disparo.wav")) {
        std::cout << "Error al cargar sonido de disparo\n";
    }

    if (!bufferExplosion.loadFromFile("assets/audios/explosion.wav")) {
        std::cout << "Error al cargar sonido de explosion\n";
    }

    if (!texturaBalaEnemiga.loadFromFile("assets/images/balaenemiga.png")) {
        std::cout << "Error bala enemiga\n";
    }


    sonidoExplosion.setBuffer(bufferExplosion);

    sonidoDisparo.setBuffer(bufferDisparo);
}

void Juego::ejecutar() {
    while (ventana.isOpen()) {
        procesarEventos();

        if (estado == JUGANDO) {
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
        if (estado == INGRESANDO_NOMBRE)
            procesarIngresoNombre(evento);

        else if (estado == INGRESANDO_CONTRASENA)
            procesarIngresoContrasena(evento);
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

        // ASTEROIDES aparecen hasta 10 puntos
        if (i % 3 == 0 && puntuacion < 10) {
            continue;
        }

        Enemigo* enemigo = enemigos[i];
        if (enemigo->obtenerPosicion().y > altoVentana - 160) {

            vidas--;

            enemigo->reiniciar();

            if (vidas <= 0) {
                guardarPuntaje();
                estado = GAME_OVER;
                return;
            }
        }

        enemigo->actualizar();

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
            }

        if (puntuacion >= 20) {

            if (std::rand() % 500 == 0) {

                for (BalaEnemiga* balaE : balasEnemigas) {

                    if (!balaE->activa()) {

                        balaE->disparar(
                            enemigo->obtenerPosicion()
                        );

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

            invulnerable = true;
            relojInvulnerable.restart();

            if (vidas <= 0) {

                guardarPuntaje();

                estado = GAME_OVER;

                return;
            }
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
    if (estado == INGRESANDO_NOMBRE) {
        mostrarPantallaNombre();
    }
    else if (estado == INGRESANDO_CONTRASENA) {
        mostrarPantallaContrasena();
    }
    else if (estado == JUGANDO) {
        for (int i = 0; i < enemigos.size(); i++) {

            //despues de de 10 puntos salen asteroides
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
    ventana.draw(textoTokens);

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

    ventana.draw(textoPuntajes);

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

    ventana.draw(textoTitulo);
    ventana.draw(textoJugar);
    ventana.draw(textoSalir);
}

void Juego::procesarMenu(sf::Event evento) {

    if (evento.type == sf::Event::MouseButtonPressed) {

        if (evento.mouseButton.button == sf::Mouse::Left) {

            if (mouseSobreJugar()) {
                estado = JUGANDO;
            }

            if (mouseSobrePuntajes()) {
                estado = PUNTAJES;
            }

            if (mouseSobreSalir()) {
                ventana.close();
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

void Juego::procesarGameOver(sf::Event evento) {

    if (evento.type == sf::Event::MouseButtonPressed) {

        if (evento.mouseButton.button == sf::Mouse::Left) {

            if (mouseSobreReiniciar()) {

                puntuacion = 0;

                bala->reiniciar();

                for (Enemigo* enemigo : enemigos) {
                    enemigo->reiniciar();
                }

                estado = JUGANDO;
            }

            if (mouseSobreMenuPrincipal()) {

                puntuacion = 0;

                bala->reiniciar();

                for (Enemigo* enemigo : enemigos) {
                    enemigo->reiniciar();
                }

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

bool Juego::mouseSobrePuntajes() {

    sf::Vector2i mouse = sf::Mouse::getPosition(ventana);

    return textoPuntajes.getGlobalBounds().contains(
        static_cast<float>(mouse.x),
        static_cast<float>(mouse.y)
    );
}

void Juego::mostrarPuntajes() {

    sf::Text titulo;
    titulo.setFont(fuenteGameOver);
    titulo.setString("PUNTAJES");
    titulo.setCharacterSize(70);
    titulo.setFillColor(sf::Color::White);

    titulo.setPosition(
        anchoVentana / 2.f - titulo.getGlobalBounds().width / 2.f,
        100.f
    );

    ventana.draw(titulo);

    std::ifstream archivo("puntajes.txt");

    std::string linea;

    float y = 250.f;

    while (std::getline(archivo, linea)) {

        sf::Text texto;

        texto.setFont(fuente);
        texto.setString(linea);
        texto.setCharacterSize(40);
        texto.setFillColor(sf::Color::White);

        texto.setPosition(300.f, y);

        ventana.draw(texto);

        y += 60.f;
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
}

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

void Juego::procesarIngresoContrasena(sf::Event evento) {

    // 1. MANEJO DE ESCRITURA (TEXT ENTERED)
    if (evento.type == sf::Event::TextEntered) {
        if (evento.text.unicode == '\b') {
            if (!contrasenaJugador.empty()) {
                contrasenaJugador.pop_back();
            }
        }
        // Eliminamos el '\r' de aquí para que todo el control del Enter lo tenga el bloque de abajo
        else if (evento.text.unicode < 128 && evento.text.unicode != '\r' && contrasenaJugador.size() < 16) {
            char letra = static_cast<char>(evento.text.unicode);
            if (std::isalnum(letra) || letra == '_') {
                contrasenaJugador += letra;
            }
        }
    }

    // 2. MANEJO DEL ENTER (KEY PRESSED)
    if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::Enter) {

        if (contrasenaJugador.size() >= 4) {
            bool ok = false;

            // Si usarAPI es true, consulta. Si es false, pasa directo (Bypass)
            if (usarAPI) {
                mensajeAuth = "VERIFICANDO...";
                ok = llamarLoginAPI(nombreJugador, contrasenaJugador);
            } else {
                ok = false;
            }

            // Procesar el resultado del Login / Bypass
            if (ok) {
                mensajeAuth = "";
                estado = MENU; // O 'JUGANDO', según a dónde quieras mandar al usuario
            } else {
                mensajeAuth = "USUARIO O CONTRASEÑA INCORRECTOS";
                contrasenaJugador = "";
                nombreJugador = "";
                estado = INGRESANDO_NOMBRE; // Regresa a pedir el nombre si falla
            }
        }
    }
}


size_t Juego::escribirRespuesta(void* contenido, size_t size, size_t nmemb, std::string* out) {
    size_t total = size * nmemb;
    out->append((char*)contenido, total);
    return total;
}

bool Juego::llamarLoginAPI(const std::string& usuario, const std::string& contrasena) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string body = "{\"username\":\"" + usuario + "\",\"password\":\"" + contrasena + "\"}";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "x-api-key: B_G5_DESTRUCTOR_ESPACIAL_KEY_2026");
    curl_easy_setopt(curl, CURLOPT_URL, "http://52.55.14.96:3000/api/external/auth/login");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);
    std::string debug = "CURLcode: " + std::to_string(res) + "\nRespuesta: " + respuesta;
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK && httpCode == 200) {
        // Parsear tokens del JSON
        try {
            auto json = nlohmann::json::parse(respuesta);
            saldoTokens = json["usuario"]["saldo_tokens"].get<int>();
            tokenSesion = json["token"].get<std::string>();
        } catch (...) {
            saldoTokens = 0;
        }
        return true;
    }
    return false;
}

bool Juego::llamarIniciarPartidaAPI() {
    if (saldoTokens < costoPartida) {
        mensajeAuth = "TOKENS INSUFICIENTES";
        return false;
    }
    estado = JUGANDO;

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string respuesta;
    std::string body = "{\"costo_partida\":" + std::to_string(costoPartida) + "}";
    std::string authHeader = "Authorization: Bearer " + tokenSesion;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "x-api-key: B_G5_DESTRUCTOR_ESPACIAL_KEY_2026");
    headers = curl_slist_append(headers, authHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, "http://52.55.14.96:3000/api/external/games/start");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode res = curl_easy_perform(curl);
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK && httpCode == 200) {
        try {
            auto json = nlohmann::json::parse(respuesta);
            idPartida = json["id_partida"].get<long long>();
            saldoTokens -= costoPartida;  // descontar localmente
            partidaIniciadaAPI = true;
            partidaFinalizada = false;
        } catch (...) {}
        return true;
    }
    return false;
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

void Juego::reiniciarPartida() {
    puntuacion = 0;
    vidas = 3;
    if (vidas == 0)
    {
        vidas == 3;
    }
    ultimoAumentoVelocidad = 1;
    mostrarExplosion = false;
    invulnerable = false;

    bala->reiniciar();

    for (Enemigo* enemigo : enemigos) {
        enemigo->reiniciar();
    }
}

void Juego::mostrarVidas() {
    sf::Text texto;
    texto.setFont(fuente);
    texto.setString("VIDAS: " + std::to_string(vidas));
    texto.setCharacterSize(32);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(55.f, 55.f);

    ventana.draw(texto);
}
