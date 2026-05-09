#include "../include/enemigo.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

Enemigo::Enemigo(int startX, int startY) {
    x = startX;
    y = startY;
    velocidad = 2;
    activo = true;

    angulo = rand() % 360;     
    amplitud = 20 + rand() % 30; 
}

void Enemigo::mover() {
    y += velocidad; // baja

    angulo += 0.1; 

    x += (int)(sin(angulo) * amplitud * 0.1);

    // límites de pantalla (ajusta si necesitas)
    if (x < 0) x = 0;
    if (x > 800) x = 800;
}

void Enemigo::dibujar() {
    if (activo) {
        cout << "Enemigo en: (" << x << ", " << y << ")" << endl;
    }
}

void Enemigo::destruir() {
    activo = false;
}

int Enemigo::getX() {
    return x;
}

int Enemigo::getY() {
    return y;
}

bool Enemigo::estaActivo() {
    return activo;
}
