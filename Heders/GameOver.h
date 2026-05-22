#pragma once
#include <SFML/Graphics.hpp>

class GameOver {
public:
    GameOver();

    int ejecutar(sf::RenderWindow& ventana);

private:
    sf::Font fuente;

    sf::Text titulo;
    sf::Text reiniciar;
    sf::Text salir;

    int opcionSeleccionada = 0;

    void procesarEventos(
        sf::RenderWindow& ventana,
        sf::Event& evento,
        bool& corriendo,
        int& resultado
    );

    void dibujar(sf::RenderWindow& ventana);
};