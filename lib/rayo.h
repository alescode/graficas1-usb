#ifndef RAYO_HH
#define RAYO_HH

class Rayo {
    public:
        Punto pos;
        float z_inicial;

        Rayo() {pos = Punto(), z_inicial = 0;}

        Rayo(Punto p, float z) {
            pos = p;
            z_inicial = z;
        }
};

#endif
