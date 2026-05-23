#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "BalaEnemiga.h"
#include <string>
#include "Jugador.h"
#include "Enemigo.h"
#include "Bala.h"
#include <fstream>
#include <curl/curl.h>
#include <string>


enum EstadoJuego {
    INGRESANDO_NOMBRE,
    INGRESANDO_CONTRASENA,
    MENU,
    JUGANDO,
    PAUSA,
    PUNTAJES,
    GAME_OVER
};



class Juego {
public:
    Juego();
    void ejecutar();

private:
    bool autenticando = false;
    std::string mensajeAuth = "";

    bool llamarLoginAPI(const std::string& , const std::string& contrasena);
    static size_t escribirRespuesta(void* contenido, size_t size, size_t nmemb, std::string* out);


    int ultimoAumentoVelocidad = 0;

    int vidas = 3;

    void mostrarVidas();

    sf::RenderWindow ventana;

    sf::Texture texturaFondo;

    sf::Sprite fondo2;
    sf::Texture texturaJugador;
    sf::Texture texturaBala;
    sf::Texture texturaEnemigo1;
    sf::Texture texturaEnemigo2;
    sf::Texture texturaAsteroide;

    sf::Texture texturaExplosion;
    sf::Sprite spriteExplosion;

    bool mostrarExplosion = false;

    sf::Clock relojExplosion;

    sf::Sprite fondo;


    sf::Texture texturaBalaEnemiga;

    std::vector<BalaEnemiga*> balasEnemigas;

    bool invulnerable = false;
    sf::Clock relojInvulnerable;

    sf::Font fuente;
    sf::Font fuenteGameOver;

    sf::Music musicaFondo;

    sf::SoundBuffer bufferDisparo;
    sf::Sound sonidoDisparo;

    sf::SoundBuffer bufferExplosion;
    sf::Sound sonidoExplosion;

    EstadoJuego estado = INGRESANDO_NOMBRE;

    sf::Text textoNombre;
    sf::Text textoInputNombre;
    std::string nombreJugador = "";

    void mostrarPantallaNombre();
    void procesarIngresoNombre(sf::Event evento);

    sf::Text textoTitulo;
    sf::Text textoJugar;
    sf::Text textoSalir;


    void mostrarMenu();
    void procesarMenu(sf::Event evento);

    sf::Text textoReiniciar;
    sf::Text textoMenuPrincipal;

    bool mouseSobreReiniciar();
    bool mouseSobreMenuPrincipal();

    void procesarGameOver(sf::Event evento);

    sf::Text textoPuntajes;

    bool mouseSobrePuntajes();

    void mostrarPuntajes();
    void procesarPuntajes(sf::Event evento);

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

    void guardarPuntaje();

    float anchoVentana;
    float altoVentana;

    bool mouseSobreJugar();
    bool mouseSobreSalir();

    float posicionFondoY = 0.f;

    void mostrarPausa();
    void procesarPausa(sf::Event evento);
    void reiniciarPartida();

    std::string contrasenaJugador;
    std::string contrasenaIngresada;
    bool enPantallaContrasena = false;
    sf::Text textoContrasena;
    sf::Text textoInputContrasena;

    void mostrarPantallaContrasena();
    void procesarIngresoContrasena(sf::Event evento);

};