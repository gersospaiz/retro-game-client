#include "../Heders/GameOver.h"

GameOver::GameOver() {

    fuente.loadFromFile("assets/fonts/armalite.TTF");

    titulo.setFont(fuente);
    titulo.setString("GAME OVER");
    titulo.setCharacterSize(70);
    titulo.setFillColor(sf::Color::Red);

    titulo.setOrigin(
        titulo.getLocalBounds().width / 2.f,
        titulo.getLocalBounds().height / 2.f
    );

    titulo.setPosition(400.f, 150.f);


    reiniciar.setFont(fuente);
    reiniciar.setString("REINICIAR");
    reiniciar.setCharacterSize(40);
    reiniciar.setFillColor(sf::Color::Yellow);

    reiniciar.setOrigin(
        reiniciar.getLocalBounds().width / 2.f,
        reiniciar.getLocalBounds().height / 2.f
    );

    reiniciar.setPosition(400.f, 320.f);


    salir.setFont(fuente);
    salir.setString("SALIR");
    salir.setCharacterSize(40);
    salir.setFillColor(sf::Color::White);

    salir.setOrigin(
        salir.getLocalBounds().width / 2.f,
        salir.getLocalBounds().height / 2.f
    );

    salir.setPosition(400.f, 400.f);
}

int GameOver::ejecutar(sf::RenderWindow& ventana) {

    bool corriendo = true;
    int resultado = 0;

    while (corriendo && ventana.isOpen()) {

        sf::Event evento;

        while (ventana.pollEvent(evento)) {

            procesarEventos(
                ventana,
                evento,
                corriendo,
                resultado
            );
        }

        dibujar(ventana);
    }

    return resultado;
}

void GameOver::procesarEventos(
    sf::RenderWindow& ventana,
    sf::Event& evento,
    bool& corriendo,
    int& resultado
) {

    if (evento.type == sf::Event::Closed) {
        ventana.close();
    }

    if (evento.type == sf::Event::KeyPressed) {

        if (evento.key.code == sf::Keyboard::Down) {

            opcionSeleccionada = 1;

            reiniciar.setFillColor(sf::Color::White);
            salir.setFillColor(sf::Color::Yellow);
        }

        if (evento.key.code == sf::Keyboard::Up) {

            opcionSeleccionada = 0;

            reiniciar.setFillColor(sf::Color::Yellow);
            salir.setFillColor(sf::Color::White);
        }

        if (evento.key.code == sf::Keyboard::Enter) {

            // REINICIAR
            if (opcionSeleccionada == 0) {
                resultado = 1;
                corriendo = false;
            }

            // SALIR
            if (opcionSeleccionada == 1) {
                resultado = 2;
                corriendo = false;
            }
        }
    }
}

void GameOver::dibujar(sf::RenderWindow& ventana) {

    ventana.clear(sf::Color::Black);

    ventana.draw(titulo);
    ventana.draw(reiniciar);
    ventana.draw(salir);

    ventana.display();
}