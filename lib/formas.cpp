#include "formas.h"

void esfera(float x, float y, float z, float radius) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glutSolidSphere(radius, 100, 100);
    glPopMatrix();
}

void anillo(float x, float y, float z,
        float innerRadius, float outerRadius,
        float rotate) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotated(60,1,0,0);
    glutSolidTorus(innerRadius, outerRadius, 100, 100);
    glPopMatrix();
}

void cono(float x, float y, float z, 
          float angle, float rotatex, float rotatey,
          float rotatez, float base, float height) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(angle, rotatex, rotatey, rotatez);
    glutSolidCone(base, height, 100, 100);
    glPopMatrix();
}
