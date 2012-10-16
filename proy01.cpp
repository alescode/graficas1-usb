#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "lib/constantes.h"
#include "lib/punto.h"
#include "lib/formas.cpp"
#include "lib/glm.h"
/* Idea para coordenadas esféricas y utilización del mouse para mover el mundo:
 * Alexandri Zavodni
 * http://www.nd.edu/~pbui/teaching/cse40166.f10/examples/ex_15/main.cpp
 */

typedef enum coordenada {coordenada_x, coordenada_y, coordenada_z};

typedef enum modelo {planeta, objeto1, objeto2};

GLint botonMouse; // boton izquierdo del mouse
int mouseX = 0, mouseY = 0; // ultimas coordenadas conocidas del mouse

float VELOCIDAD_ZOOM = 2.5;

Punto camaraXYZ, camaraTPR;
// coordenadas cartesianas y esfericas (theta-pi-R)

bool estadosFlechas[4] = {false};
// vector booleano que determina si se estan presionando las teclas
// izquierda, derecha, arriba, abajo

Punto centroPlaneta, centroObj1, centroObj2;

modelo modeloActual;
GLMmodel* pmodel = NULL;

void actualizarObjeto() {
    centroPlaneta.x += 1.0f;
    centroPlaneta.y += 1.0f;
    centroPlaneta.z += 1.0f;
}

void actualizarOrientacion() {
    // Asegura que Phi este entre 0 y pi
    if (camaraTPR.y <= 0)
        camaraTPR.y = 0.001;
    if (camaraTPR.y >= M_PI)
        camaraTPR.y = M_PI-0.001;
    // Asegura que el radio no sea muy pequeño
    if (camaraTPR.z < 5.0) {
        camaraTPR.z = 5.0;
    }

    camaraXYZ.x = camaraTPR.z * sinf(camaraTPR.x) * sinf(camaraTPR.y);
    camaraXYZ.z = camaraTPR.z * -cosf(camaraTPR.x) * sinf(camaraTPR.y);
    camaraXYZ.y = camaraTPR.z * -cosf(camaraTPR.y);
}

void configurarEscena() { 
    float position[] = {0,0,1,0};
    float diffuse[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_LIGHT0);

    camaraTPR = Punto(2.39f, 2.015f, 349.0f);
    centroPlaneta = Punto(10.0f, 30.0f, 40.0f);
    actualizarOrientacion();
}

// Escucha el estado actual del mouse y actualiza las estructuras de datos
// pertinentes
void mouse(int boton, int estado, int x, int y)
{
    if (boton == GLUT_LEFT_BUTTON)
        botonMouse = estado;

    if (boton == RUEDA_ARRIBA) {
        camaraTPR.z += 0.5;
        actualizarOrientacion();
    }

    if (boton == RUEDA_ABAJO) {
        camaraTPR.z -= 0.5;
        actualizarOrientacion();
    }

    mouseX = x;
    mouseY = y;
}

void movimientoMouse(int x, int y)
{
    if (botonMouse == GLUT_DOWN) {
        camaraTPR.x += (x - mouseX) * 0.005;
        camaraTPR.y += (y - mouseY) * 0.005;

        actualizarOrientacion();
    }
    mouseX = x;
    mouseY = y;
}

void teclas(unsigned char tecla, int x, int y) {
    switch (tecla) {
        // ESC = Salir
        case 27:
            exit(0);
        // W, arriba
        case 119:
        case 87:
            camaraTPR.y += 0.01;
            actualizarOrientacion();
            break;
        // A, izquierda
        case 97:
        case 65:
            camaraTPR.x += 0.01;
            actualizarOrientacion();
            break;
        // S, abajo
        case 115:
        case 83:
            camaraTPR.y -= 0.01;
            actualizarOrientacion();
            break;
        // D, derecha
        case 100:
        case 68:
            camaraTPR.x -= 0.01;
            actualizarOrientacion();
            break;
        // E, dolly in
        case 69:
        case 101:
            camaraTPR.z -= VELOCIDAD_ZOOM;
            actualizarOrientacion();
            break;
        // Q, dolly out
        case 81:
        case 113:
            camaraTPR.z += VELOCIDAD_ZOOM;
            actualizarOrientacion();
            break;
        case 49:
            modeloActual = planeta;
            break;
        case 50:
            modeloActual = objeto1;
            break;
        case 51:
            modeloActual = objeto2;
            break;
    }
}

void teclasEspeciales(int tecla, int x, int y)
{
    if (tecla == GLUT_KEY_LEFT)
        estadosFlechas[0] = true;

    if (tecla == GLUT_KEY_RIGHT)
        estadosFlechas[1] = true;

    if (tecla == GLUT_KEY_UP)
        estadosFlechas[2] = true;

    if (tecla == GLUT_KEY_DOWN)
        estadosFlechas[3] = true;
}

void teclasEspecialesSoltar(int tecla, int x, int y)
{
    if (tecla == GLUT_KEY_LEFT)
        estadosFlechas[0] = false;

    if (tecla == GLUT_KEY_RIGHT)
        estadosFlechas[1] = false;

    if (tecla == GLUT_KEY_UP)
        estadosFlechas[2] = false;

    if (tecla == GLUT_KEY_DOWN)
        estadosFlechas[3] = false;
}

void malla(coordenada c) {
    double delta;
    for (int i = 1; i < 20 ; i++) {
        delta = i * 5.0;
        switch (c) {
            case coordenada_x:
                glVertex3f(0.0, delta, 0.0);
                glVertex3f(100.0, delta, 0.0);
                glVertex3f(delta, 0.0, 0.0);
                glVertex3f(delta, 100.0, 0.0);
                break;
            case coordenada_y:
                glVertex3f(0.0, delta, 0.0);
                glVertex3f(0.0, delta, 100.0);
                glVertex3f(0.0, 0.0, delta);
                glVertex3f(0.0, 100.0, delta);
                break;
            case coordenada_z:
                glVertex3f(0.0, 0.0, delta);
                glVertex3f(100.0, 0.0, delta);
                glVertex3f(delta, 0.0, 0.0);
                glVertex3f(delta, 0.0, 100.0);
                break;
        }
    }
}

void dibujarPlaneta() {
    glPushMatrix();
    glColor3ub(0,238,0);
    esfera(centroPlaneta.x, centroPlaneta.y, centroPlaneta.z, 6);

    glColor3ub(255,69,0);
    anillo(centroPlaneta.x, centroPlaneta.y, centroPlaneta.z, 1, 10, 60);
    glPopMatrix();
}

void revisarFlechas() {
    Punto* aMover;
    switch (modeloActual) {
        case planeta:
            aMover = &centroPlaneta;
            break;
        case objeto1:
            aMover = &centroObj1;
            break;
        case objeto2:
            aMover = &centroObj2;
            break;
    }

    if (estadosFlechas[0])
        aMover->x -= 1.0f;
    if (estadosFlechas[1])
        aMover->x += 1.0f;
    if (estadosFlechas[2])
        aMover->y += 1.0f;
    if (estadosFlechas[3])
        aMover->y -= 1.0f;
}

void drawmodel(void)
{
 
    if (!pmodel) {
        pmodel = glmReadOBJ("data/al.obj");
        if (!pmodel) exit(0);
        glmUnitize(pmodel);
        glmFacetNormals(pmodel);
        glmVertexNormals(pmodel, 90.0);
    }
    
    glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
}

void display(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f ,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gluLookAt(camaraXYZ.x, camaraXYZ.y, camaraXYZ.z,
              0.0, 0.0, 0.0, // mirando hacia (0, 0, 0)
              0.0, 1.0, 0.0); // view up

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

    // EJE X
    glBegin(GL_LINES);
    glColor3ub(30,90,90);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(100.0,0.0,0.0);
    malla(coordenada_x);

    // EJE Y
    glColor3ub(90,90,30);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,100.0,0.0);
    malla(coordenada_y);

    // EJE Z
    glColor3ub(90,30,90);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,100.0);
    malla(coordenada_z);
    glEnd();

    revisarFlechas();
    dibujarPlaneta();

       glEnable(GL_LIGHTING);
    glPushMatrix();
    drawmodel();
    glPopMatrix();
        drawmodel();
        glDisable(GL_LIGHTING);

    glFlush();
}

void cambioventana(int w, int h){

    float aspectratio;

    if (h==0)
        h=1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    aspectratio = (float) w / (float) h;

    gluPerspective(35.0f, aspectratio, 1.0, 2000.0);
}


int main(int argc,char** argv) {
    glutInit(&argc,argv);
    glutInitWindowSize (800, 800); 
    glutInitWindowPosition(10, 50);
    glutCreateWindow ("Proyecto 1");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    configurarEscena();

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutReshapeFunc(cambioventana);

    glutKeyboardFunc(teclas);
    glutSpecialFunc(teclasEspeciales);
    glutSpecialUpFunc(teclasEspecialesSoltar);
    glutMouseFunc(mouse);
    glutMotionFunc(movimientoMouse);

    glutMainLoop();

    return 0;
}
