#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "../lib/constantes.h"
#include "../lib/punto.h"
#include "../lib/formas.cpp"
#include "../lib/glm.h"
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

Punto centroPlaneta, centroObj1, centroObj2, centroCono;

modelo modeloActual;
GLMmodel* pmodel = NULL;

float light_position[] = {0,0,1,0};

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
    float diffuse[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_LIGHT0);

    glEnable(GL_NORMALIZE);
    camaraTPR = Punto(2.39f, 2.015f, 349.0f);
    centroPlaneta = Punto(50.0f, 40.0f, 60.0f);
    centroObj1 = Punto(43.0f, 68.0f, 60.0f);
    centroObj2 = Punto(5.0f, 50.0f, 10.0f);

    centroCono = Punto(centroPlaneta.x, centroPlaneta.y + 25, centroPlaneta.z);
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
        /* Mover la luz */
        /*case 'x':
            light_position[0] += 1;
            printf("%f\n", light_position[0]);
            break;
        case 'y':
            light_position[1] += 1;
            break;
        case 'z':
            light_position[2] += 1;
            break;
        */
        case 'w':
        case 'W':
            camaraTPR.y += 0.01;
            actualizarOrientacion();
            break;
        // A, izquierda
        case 'a':
        case 'A':
            camaraTPR.x += 0.01;
            actualizarOrientacion();
            break;
        // S, abajo
        case 's':
        case 'S':
            camaraTPR.y -= 0.01;
            actualizarOrientacion();
            break;
        // D, derecha
        case 'd':
        case 'D':
            camaraTPR.x -= 0.01;
            actualizarOrientacion();
            break;
        // E, dolly in
        case 'e':
        case 'E':
            camaraTPR.z -= VELOCIDAD_ZOOM;
            actualizarOrientacion();
            break;
        // Q, dolly out
        case 'q':
        case 'Q':
            camaraTPR.z += VELOCIDAD_ZOOM;
            actualizarOrientacion();
            break;
        case '1':
            modeloActual = planeta;
            break;
        case '2':
            modeloActual = objeto1;
            break;
        case '3':
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

void dibujarPlaneta(float x, float y, float z) {
    glPushMatrix();
    glColor3ub(0,238,0);
    esfera(x, y, z, 15);

    glColor3ub(255,69,0);
    anillo(x, y, z, 1, 25, 50);

    glPopMatrix();
}

void dibujarFranja(float x, float y, float z) {
    glBegin(GL_TRIANGLE_STRIP);
    glColor3ub(38, 138, 190);
    glVertex3f(x, y, z);
    glVertex3f(x + 5, y + 20, z);
    glVertex3f(x + 5, y + 30, z);
    glVertex3f(x + 25, y - 30, z - 30);
    glEnd();
}

// Actualiza los posiciones de los objetos si las flechas direccionales son
// presionadas
void revisarFlechas() {
    Punto* aMover;
    switch (modeloActual) {
        case planeta:
            aMover = &centroPlaneta;
            centroCono = Punto(centroPlaneta.x, 
                         centroPlaneta.y + 25, centroPlaneta.z);
            break;
        case objeto1:
            aMover = &centroObj1;
            centroCono = Punto(centroObj1.x, 
                         centroObj1.y + 18, centroObj1.z);
            break;
        case objeto2:
            aMover = &centroObj2;
            centroCono = Punto(centroObj2.x + 10, 
                         centroObj2.y + 40, centroObj2.z);
            break;
    }

    if (estadosFlechas[0])
        aMover->x -= 1.0f;
        centroCono.x -= 1.0f;
    if (estadosFlechas[1])
        aMover->x += 1.0f;
        centroCono.x += 1.0f;
    if (estadosFlechas[2])
        aMover->y += 1.0f;
        centroCono.y += 1.0f;
    if (estadosFlechas[3])
        aMover->y -= 1.0f;
        centroCono.y -= 1.0f;
}

void dibujarCapo(float x, float y, float z) {
 
    glPushMatrix();
    if (!pmodel) {
        char a[] = "../data/al.obj";
        pmodel = glmReadOBJ(a);
        glmUnitize(pmodel);
        glmFacetNormals(pmodel);
        glmVertexNormals(pmodel, 90.0);
    }

    glTranslatef(x, y, z);
    glScalef(7.0f, 9.0f, 7.0f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    //glScalef(8.0f, 10.0f, 8.0f);
    glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();
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

    /* Dibuja la esfera de luz */
    /*glDisable(GL_LIGHTING);
    glColor3ub(255,255,255);
    esfera(light_position[0], light_position[1], light_position[2], 1);
    */

    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    dibujarPlaneta(centroPlaneta.x, centroPlaneta.y, centroPlaneta.z);
    dibujarCapo(centroObj1.x, centroObj1.y, centroObj1.z);

    glDisable(GL_LIGHTING);

    glColor3ub(255,255,102);

    // El cono indica qué objeto está seleccionado para moverse
    cono(centroCono.x, centroCono.y, centroCono.z,
         90.0, 1.0, 0.0, 0.0, 
         2.0, 8.0);

    dibujarFranja(centroObj2.x, centroObj2.y, centroObj2.z);

    glColor3ub(255,255,255);

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

    glutMainLoop();

    return 0;
}
