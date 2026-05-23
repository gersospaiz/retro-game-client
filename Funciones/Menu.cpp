#include "../Heders/Menu.h"
#include "iostream"

Menu::Menu() {

    fondo.setTexture(texturaFondo);

    // POSICIÓN
    fondo.setPosition(0.f, 0.f);

    fondo.setScale(
    1244.f / static_cast<float>(texturaFondo.getSize().x),
    700.f / static_cast<float>(texturaFondo.getSize().y)
);
    //---APARATDO DEL TIRULO
    titulo.setFont(fuente);
    titulo.setString("DESTRUCTOR ESPACIAL");
    titulo.setCharacterSize(60);
    titulo.setFillColor(sf::Color::Cyan);
    // CENTRAR TITULO
    titulo.setOrigin(
        titulo.getLocalBounds().width / 2.f,
        titulo.getLocalBounds().height / 2.f
    );

    titulo.setPosition(400.f, 120.f);


    //--- BOTON JUGAR
    jugar.setFont(fuente);
    jugar.setString("JUGAR");
    jugar.setCharacterSize(40);
    jugar.setFillColor(sf::Color::Yellow);

    // CENTRAR JUGAR
    jugar.setOrigin(
        jugar.getLocalBounds().width / 2.f,
        jugar.getLocalBounds().height / 2.f
    );

    jugar.setPosition(400.f, 300.f);




    // BOTON SALIR
    salir.setFont(fuente);
    salir.setString("SALIR");
    salir.setCharacterSize(40);
    salir.setFillColor(sf::Color::White);
    // CENTRAR SALIR
    salir.setOrigin(
        salir.getLocalBounds().width / 2.f,
        salir.getLocalBounds().height / 2.f
    );

    salir.setPosition(400.f, 380.f);
}

bool Menu::ejecutar(sf::RenderWindow& ventana) {

    bool corriendo = true;
    bool iniciarJuego = false;

    while (corriendo && ventana.isOpen()) {

        sf::Event evento;

        while (ventana.pollEvent(evento)) {

            procesarEventos(
                ventana,
                evento,
                corriendo,
                iniciarJuego
                );
        }
        dibujar(ventana);
    }
    return iniciarJuego;

}

void Menu::procesarEventos(
    sf::RenderWindow& ventana,
    sf::Event &evento,
    bool &corriendo,
    bool &iniciarJuego
    ) {
    if (evento.type == sf::Event::Closed) {
        ventana.close();
    }

    if (evento.type == sf::Event::KeyPressed) {

        //BAJAR
        if (evento.key.code == sf::Keyboard::Down) {
            opcionSeleccionada= 1;

            jugar.setFillColor(sf::Color::White);
            salir.setFillColor(sf::Color::Yellow);
        }
            //SUBIR
            if (evento.key.code == sf::Keyboard::Up) {

                opcionSeleccionada = 0;

                jugar.setFillColor(sf::Color::Yellow);
                salir.setFillColor(sf::Color::White);

            }

        //ENTER
        if (evento.key.code == sf::Keyboard::Enter) {

            if (opcionSeleccionada == 0) {
                iniciarJuego = true;
                corriendo = false;
            }
            if (opcionSeleccionada == 1) {
                ventana.close();
            }
        }

    }
}
void Menu::dibujar(sf::RenderWindow& ventana) {

    ventana.clear(sf::Color(10,10,40));
ventana.draw(fondo);
    ventana.draw(titulo);
    ventana.draw(jugar);
    ventana.draw(salir);

    ventana.display();
}


