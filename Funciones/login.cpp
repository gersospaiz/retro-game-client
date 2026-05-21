#include "../Heders/Login.h"
#include <iostream>
#include <curl/curl.h>
#include "../Heders/json.hpp"

// ── Helper para recibir respuesta HTTP ────────────────────────────────────
static size_t escribirRespuesta(void* contenido, size_t size, size_t nmemb, std::string* out) {
    out->append((char*)contenido, size * nmemb);
    return size * nmemb;
}

Login::Login() {
    if (!fuente.loadFromFile("assets/fonts/comicbd.ttf"))
        std::cout << "Error al cargar fuente de login\n";
}

bool Login::ejecutar(sf::RenderWindow& ventana) {
    bool corriendo = true;
    bool exitoso   = false;

    while (ventana.isOpen() && corriendo) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            procesarEventos(ventana, evento, corriendo, exitoso);
        }
        dibujar(ventana);
    }

    return exitoso;
}

void Login::procesarEventos(sf::RenderWindow& ventana, sf::Event& evento, bool& corriendo, bool& exitoso) {
    if (evento.type == sf::Event::Closed)
        ventana.close();

    if (evento.type == sf::Event::KeyPressed) {
        // Tab o Enter cambia de campo
        if (evento.key.code == sf::Keyboard::Tab) {
            escribiendoUsuario = !escribiendoUsuario;
        }

        // Backspace borra último carácter
        if (evento.key.code == sf::Keyboard::BackSpace) {
            if (escribiendoUsuario && !usuario.empty())
                usuario.pop_back();
            else if (!escribiendoUsuario && !contrasena.empty())
                contrasena.pop_back();
        }

        // Enter en campo contraseña intenta login
        if (evento.key.code == sf::Keyboard::Enter && !escribiendoUsuario) {
            if (validarConAPI()) {
                exitoso   = true;
                corriendo = false;
            } else {
                loginFallido = true;
                contrasena.clear();
            }
        }
    }

    // Captura texto escrito
    if (evento.type == sf::Event::TextEntered) {
        char c = static_cast<char>(evento.text.unicode);
        if (evento.text.unicode >= 32 && evento.text.unicode < 127) {
            if (escribiendoUsuario)
                usuario += c;
            else
                contrasena += c;
        }
    }
}

void Login::dibujar(sf::RenderWindow& ventana) {
    ventana.clear(sf::Color(20, 20, 40));

    // ── Título ────────────────────────────────────────────────────────────
    sf::Text titulo;
    titulo.setFont(fuente);
    titulo.setString("DESTRUCTOR ESPACIAL");
    titulo.setCharacterSize(36);
    titulo.setFillColor(sf::Color::Cyan);
    titulo.setPosition(160.f, 80.f);
    ventana.draw(titulo);

    // ── Campo Usuario ─────────────────────────────────────────────────────
    sf::Text labelUsuario;
    labelUsuario.setFont(fuente);
    labelUsuario.setString("Usuario:");
    labelUsuario.setCharacterSize(22);
    labelUsuario.setFillColor(sf::Color::White);
    labelUsuario.setPosition(150.f, 200.f);
    ventana.draw(labelUsuario);

    sf::RectangleShape cajaUsuario(sf::Vector2f(400.f, 40.f));
    cajaUsuario.setPosition(150.f, 230.f);
    cajaUsuario.setFillColor(sf::Color(40, 40, 60));
    cajaUsuario.setOutlineThickness(2.f);
    cajaUsuario.setOutlineColor(escribiendoUsuario ? sf::Color::Cyan : sf::Color(100, 100, 120));
    ventana.draw(cajaUsuario);

    sf::Text textoUsuario;
    textoUsuario.setFont(fuente);
    textoUsuario.setString(usuario + (escribiendoUsuario ? "|" : ""));
    textoUsuario.setCharacterSize(20);
    textoUsuario.setFillColor(sf::Color::White);
    textoUsuario.setPosition(158.f, 237.f);
    ventana.draw(textoUsuario);

    // ── Campo Contraseña ──────────────────────────────────────────────────
    sf::Text labelPass;
    labelPass.setFont(fuente);
    labelPass.setString("Contrasena:");
    labelPass.setCharacterSize(22);
    labelPass.setFillColor(sf::Color::White);
    labelPass.setPosition(150.f, 300.f);
    ventana.draw(labelPass);

    sf::RectangleShape cajaPass(sf::Vector2f(400.f, 40.f));
    cajaPass.setPosition(150.f, 330.f);
    cajaPass.setFillColor(sf::Color(40, 40, 60));
    cajaPass.setOutlineThickness(2.f);
    cajaPass.setOutlineColor(!escribiendoUsuario ? sf::Color::Cyan : sf::Color(100, 100, 120));
    ventana.draw(cajaPass);

    std::string textoPass = std::string(contrasena.size(), '*') + (!escribiendoUsuario ? "|" : "");
    sf::Text campoPass;
    campoPass.setFont(fuente);
    campoPass.setString(textoPass);
    campoPass.setCharacterSize(20);
    campoPass.setFillColor(sf::Color::White);
    campoPass.setPosition(158.f, 337.f);
    ventana.draw(campoPass);

    // ── Instrucciones ─────────────────────────────────────────────────────
    sf::Text instrucciones;
    instrucciones.setFont(fuente);
    instrucciones.setString("TAB: cambiar campo    ENTER: iniciar sesion");
    instrucciones.setCharacterSize(16);
    instrucciones.setFillColor(sf::Color(150, 150, 150));
    instrucciones.setPosition(150.f, 390.f);
    ventana.draw(instrucciones);

    // ── Error de login ────────────────────────────────────────────────────
    if (loginFallido) {
        sf::Text error;
        error.setFont(fuente);
        error.setString("Usuario o contrasena incorrectos");
        error.setCharacterSize(20);
        error.setFillColor(sf::Color::Red);
        error.setPosition(150.f, 430.f);
        ventana.draw(error);
    }

    ventana.display();
}

bool Login::validarConAPI() {
    std::cout << "Usuario: '" << usuario << "'\n";
    std::cout << "Contrasena: '" << contrasena << "'\n";
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    // ── Arma el JSON con las credenciales ─────────────────────────────────
    nlohmann::json body;
    body["username"] = usuario;
    body["password"] = contrasena;
    std::string bodyStr = body.dump();

    std::string respuesta;

    // ── Configura la petición POST ────────────────────────────────────────
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "x-api-key: B_G5_DESTRUCTOR_ESPACIAL_KEY_2026");

    curl_easy_setopt(curl, CURLOPT_URL, "http://52.55.14.96:3000/api/external/auth/login");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    std::cout << "HTTP Code: " << httpCode << "\n";
    std::cout << "Respuesta: " << respuesta << "\n";

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cout << "Error de conexion: " << curl_easy_strerror(res) << "\n";
        return false;
    }

    // ── Evalúa la respuesta ───────────────────────────────────────────────
    // Ajusta esto según lo que devuelva tu API
    return httpCode == 200;
}