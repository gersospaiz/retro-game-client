#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Login {
public:
    Login();
    bool ejecutar(sf::RenderWindow& ventana);  // Retorna true si login exitoso

private:
    sf::Font fuente;

    std::string usuario;
    std::string contrasena;
    bool escribiendoUsuario = true;  // false = escribiendo contraseña
    bool loginFallido = false;
    bool mostrarContrasena = false;

    void procesarEventos(sf::RenderWindow& ventana, sf::Event& evento, bool& corriendo, bool& exitoso);
    void dibujar(sf::RenderWindow& ventana);
    bool validarConAPI();
};