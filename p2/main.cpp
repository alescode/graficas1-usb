#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "../lib/constantes.h"
#include "../lib/punto.h"
#include "../lib/glm.h"

#define VELOCIDAD_MAXIMA 0.125
#define VELOCIDAD_MINIMA 0.015625

typedef enum coordenada {coordenada_x, coordenada_y, coordenada_z};

bool estadosFlechas[4] = {false};

float light_position[] = {0,0,1,0};

Punto camara(0, 0, 100000);
Punto nave(0, 0, camara.z);

float velocidad = VELOCIDAD_MAXIMA;

int profundidad = camara.z;

GLMmodel* pmodel = NULL;

int frames;
int seconds;

using namespace std;

bool paused;

GLint botonMouse; // boton izquierdo del mouse
int mouseX = 0, mouseY = 0; // ultimas coordenadas conocidas del mouse

vector<Punto*>* globulosRojos;

void esfera(float x, float y, float z, float radius) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glutSolidSphere(radius, 3, 3);
    glPopMatrix();
}

void anillo(float x, float y, float z,
        float innerRadius, float outerRadius,
        float rotate) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glutSolidTorus(innerRadius, outerRadius, 10, 10);
    glPopMatrix();
}

void configurarEscena() { 
    float diffuse[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_LIGHT0);

    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_NORMALIZE);
}

// Escucha el estado actual del mouse y actualiza las estructuras de datos
// pertinentes
void mouse(int boton, int estado, int x, int y)
{
    if (boton == GLUT_LEFT_BUTTON)
        cout << "au" << endl;
}

void movimientoMouse(int x, int y)
{
}

void teclas(unsigned char tecla, int x, int y) {
    switch (tecla) {
        // ESC = Salir
        case 27:
            exit(0);
        case 'e':
        case 'E':
            camara.z -= 5;
            break;
        // Q, dolly out
        case 'q':
        case 'Q':
            camara.z += 5;
            break;
        case 'p':
        case 'P':
            paused = !paused;
            break;
        case '+':
            if (velocidad < VELOCIDAD_MAXIMA)
                velocidad *= 2;
            break;
        case '-':
            if (velocidad > VELOCIDAD_MINIMA)
                velocidad /= 2;
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

// Actualiza los posiciones de los objetos si las flechas direccionales son
// presionadas
void revisarFlechas() {
    // izquierda
    if (estadosFlechas[0]) {
        if (nave.x > -0.8)
            nave.x -= 0.01;
    }
    // derecha
    if (estadosFlechas[1]) {
        if (nave.x < 0.8)
            nave.x += 0.01;
    }
    // arriba
    if (estadosFlechas[2]) {
        if (nave.y < 0.6)
            nave.y += 0.01;
    }
    // abajo
    if (estadosFlechas[3]) {
        if (nave.y > -0.6)
            nave.y -= 0.01;
    }
}

void dibujarNave(float x, float y, float z, float scale) {
 
    glPushMatrix();
    if (!pmodel) {
        char a[] = "data/virus.obj";
        pmodel = glmReadOBJ(a);
        glmUnitize(pmodel);
        glmFacetNormals(pmodel);
        glmVertexNormals(pmodel, 90.0);
    }

    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    //glScalef(8.0f, 10.0f, 8.0f);
    glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();
}

/*
void obtenerGlobulosRojos(float z) {
    globulosRojos->push_back(new Punto(1.0f, 0.0f, z));
    globulosRojos->push_back(new Punto(-1.0f, 0.0f, z));
}*/

void obtenerGlobulosRojos(float z) {
    float p = rand()/float(INT_MAX);
    if (p >= 0.9 && p < 0.95) {
        float r_x = 1.6 * rand()/float(INT_MAX) - 0.8;
        float r_y = 1.2 * rand()/float(INT_MAX) - 0.6;
        globulosRojos->push_back(new Punto(r_x, r_y, z));
    }
    else if (p >= 0.95) {
        float r_x = 1.6 * rand()/float(INT_MAX) - 0.8;
        float r_y = 1.2 * rand()/float(INT_MAX) - 0.6;
        globulosRojos->push_back(new Punto(r_x, r_y, z));
        globulosRojos->push_back(new Punto(r_x, r_y, z - 20));
    }
}

void display() {
    if (paused)
        return;
    frames += 1;
    seconds = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    //if (seconds > 0)
    //    cout << frames / seconds << endl;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.361, 0.027, 0.0,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gluLookAt(camara.x, camara.y, camara.z,
              0.0, 0.0, 0.0, // mirando hacia (0, 0, 0)
              0.0, 1.0, 0.0); // view up

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

    revisarFlechas();

    glBegin(GL_LINES);
    glVertex3f(nave.x, nave.y, camara.z);
    glVertex3f(nave.x, nave.y, camara.z - 20);
    glEnd();

    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    camara.z -= velocidad;


    if (!(frames % int(10 * VELOCIDAD_MAXIMA / velocidad))) {
        obtenerGlobulosRojos(camara.z - 40);
    }

    vector<Punto*>::iterator it;

    glColor3ub(227, 14, 16);
    for (it = globulosRojos->begin(); it < globulosRojos->end(); ++it) {
        anillo((*it)->x, (*it)->y, (*it)->z, 0.1, 0.3, 1);
    }

    glColor3ub(252, 238, 113);
    dibujarNave(nave.x, nave.y, nave.z, 0.2);
    //esfera(nave.x, nave.y, nave.z, 0.05);

    glColor3ub(255, 0, 0);
    nave.z = camara.z - 2;

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
    glutInitWindowSize (1024, 768); 
    glutInitWindowPosition(0, 0);
    glutCreateWindow ("Proyecto 1");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    configurarEscena();

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutReshapeFunc(cambioventana);

    glutKeyboardFunc(teclas);
    glutSpecialFunc(teclasEspeciales);
    glutSpecialUpFunc(teclasEspecialesSoltar);
    glutMouseFunc(mouse);
    glutMotionFunc(movimientoMouse);

    globulosRojos = new vector<Punto*>;

    srand(time(NULL));
    glutMainLoop();

    return 0;
}
