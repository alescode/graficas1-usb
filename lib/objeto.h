#ifndef OBJETO_HH
#define OBJETO_HH

class Objeto {
    public:
        Punto pos;
        float z_inicial;

        Objeto() {pos = Punto(), z_inicial = 0;}

        Objeto(Punto p, float z) {
            pos = p;
            z_inicial = z;
        }
};

#endif
