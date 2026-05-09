#ifndef ENEMIGO_H
#define ENEMIGO_H

class Enemigo {
private:
    int x, y;
    int velocidad;
    bool activo;

    float angulo;
    float amplitud;

public:
    Enemigo(int startX, int startY);

    void mover();
    void dibujar();
    void destruir();

    int getX();
    int getY();
    bool estaActivo();
};

#endif
