#pragma once

#include <SFML/Graphics.hpp>

class Menu {
public:
    Menu();

    bool ejecutar(sf::RenderWindow& ventana);

private:
    sf::Font fuente;
    sf::Texture texturaFondo;
    sf::Sprite fondo;

    sf::Text titulo;
    sf::Text jugar;
    sf::Text salir;

    int opcionSeleccionada = 0;

    void procesarEventos(
        sf::RenderWindow& ventana,
        sf::Event& evento,
        bool& corriendo,
        bool& iniciarJuego
    );

    void dibujar(sf::RenderWindow& ventana);
};