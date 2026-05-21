#include "../Heders/Juego.h"
#include "../Heders/Login.h"

int main() {
    // Ventana solo para el login
    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Destructor Espacial - Login");

    Login login;
    bool loginExitoso = login.ejecutar(ventana);

    ventana.close(); // Cierra la ventana del login

    if (!loginExitoso) return 0;

    // Juego crea su propia ventana internamente
    Juego juego;
    juego.ejecutar();

    return 0;
}