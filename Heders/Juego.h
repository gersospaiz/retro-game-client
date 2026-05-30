#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <string>
#include <fstream>

#include "Jugador.h"
#include "Enemigo.h"
#include "Bala.h"
#include "BalaEnemiga.h"

#include "ApiClient.h"
#include "GameApiConfig.h"

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


    ApiClient apiClient{
        GameApiConfig::BASE_URL,
        GameApiConfig::API_KEY,
        GameApiConfig::CODIGO_JUEGO
    };

    UsuarioApi usuarioApi;
    PartidaApi partidaApi;

    bool partidaIniciadaAPI = false;
    bool partidaFinalizadaAPI = false;

    int ultimoScoreReportado = 0;
    sf::Clock relojPartidaApi;

    int saldoTokens = 0;
    std::string mensajeAuth = "";

    sf::Text textoTokens;


    int ultimoAumentoVelocidad = 0;
    int vidas = 3;
    int puntuacion = 0;

    bool juegoTerminado = false;
    bool invulnerable = false;

    sf::Clock relojInvulnerable;

    EstadoJuego estado = INGRESANDO_NOMBRE;


    sf::RenderWindow ventana;

    float anchoVentana;
    float altoVentana;
    float posicionFondoY = 0.f;


    sf::Texture texturaFondo;
    sf::Sprite fondo;
    sf::Sprite fondo2;


    sf::Texture texturaJugador;
    sf::Texture texturaBala;
    sf::Texture texturaEnemigo1;
    sf::Texture texturaEnemigo2;
    sf::Texture texturaAsteroide;
    sf::Texture texturaExplosion;
    sf::Texture texturaBalaEnemiga;

    sf::Sprite spriteExplosion;

    bool mostrarExplosion = false;

    sf::Clock relojExplosion;


    Jugador* jugador;
    Bala* bala;

    std::vector<Enemigo*> enemigos;
    std::vector<BalaEnemiga*> balasEnemigas;

    sf::Font fuente;
    sf::Font fuenteGameOver;

    sf::Music musicaFondo;

    sf::SoundBuffer bufferDisparo;
    sf::Sound sonidoDisparo;

    sf::SoundBuffer bufferExplosion;
    sf::Sound sonidoExplosion;


    sf::Text textoNombre;
    sf::Text textoInputNombre;

    std::string nombreJugador = "";

    sf::Text textoContrasena;
    sf::Text textoInputContrasena;

    std::string contrasenaJugador = "";

    sf::Text textoTitulo;
    sf::Text textoJugar;
    sf::Text textoSalir;
    sf::Text textoPuntajes;

    sf::Text textoReiniciar;
    sf::Text textoMenuPrincipal;


    void cargarRecursos();

    void procesarEventos();
    void actualizar();
    void dibujar();


    void mostrarPantallaNombre();
    void procesarIngresoNombre(sf::Event evento);

    void mostrarPantallaContrasena();
    void procesarIngresoContrasena(sf::Event evento);

    void mostrarMenu();
    void procesarMenu(sf::Event evento);


    void mostrarPuntajes();
    void procesarPuntajes(sf::Event evento);


    void mostrarGameOver();
    void procesarGameOver(sf::Event evento);


    void mostrarPausa();
    void procesarPausa(sf::Event evento);


    void mostrarPuntuacion();
    void mostrarVidas();

    void guardarPuntaje();
    void reiniciarPartida();

    bool mouseSobreJugar();
    bool mouseSobreSalir();

    bool mouseSobrePuntajes();

    bool mouseSobreReiniciar();
    bool mouseSobreMenuPrincipal();
    bool loginActivo = true;
};