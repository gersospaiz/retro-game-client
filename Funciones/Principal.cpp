#include "../Heders/Juego.h"
#include "../Heders/Login.h"
#include "../Heders/Menu.h"

int main() {
    // Ventana solo para el login
    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Destructor Espacial - Login");

    Login login;
    bool loginExitoso = login.ejecutar(ventana);

    ventana.close(); // Cierra la ventana del login

    if (!loginExitoso) return 0;

    // MENU
    sf::RenderWindow ventanaMenu(
        sf::VideoMode(800, 600),
        "Destructor Espacial - Menu"
    );

    Menu menu;

    bool iniciarJuego = menu.ejecutar(ventanaMenu);

    ventanaMenu.close();

    if (!iniciarJuego)
        return 0;

    // Juego crea su propia ventana internamente
    Juego juego;
    juego.ejecutar();

    return 0;
}