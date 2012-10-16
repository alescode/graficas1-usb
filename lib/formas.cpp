#include "lib/formas.h"

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
