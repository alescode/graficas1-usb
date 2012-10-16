#ifndef FORMAS_HH
#define FORMAS_HH

void esfera(float x, float y, float z, float radius);
void anillo(float x, float y, float z,
                  float innerRadius, float outerRadius,
                  float rotate);
void cono(float x, float y, float z, 
          float angle, float rotatex, float rotatey,
          float rotatez, float base, float height);

#endif
