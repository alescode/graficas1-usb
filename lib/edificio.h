#ifndef EDIFICIO_HH
#define EDIFICIO_HH

class Edificio {
    public:
        Punto pos;
        float altura;
        float profundidad;

        Edificio() {pos = Punto(), altura = 0, profundidad = 0;}

        Edificio(Punto p, float y, float z) {
            pos = p;
            altura = y;
            profundidad = z;
        }
};

#endif

