#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <DromeAudio/AudioContext.h>
#include <DromeAudio/AudioDriver.h>
#include <DromeAudio/Exception.h>
#else
#include <GL/glut.h>
#endif

#include "../lib/constantes.h"
#include "../lib/punto.h"
#include "../lib/rayo.h"
#include "../lib/glm.h"

#define VELOCIDAD_MAXIMA 0.125
#define VELOCIDAD_MINIMA 0.015625

typedef enum coordenada {coordenada_x, coordenada_y, coordenada_z};

bool estadosFlechas[4] = {false};

Punto camara(0, 0, 100000);
Punto nave(0, 0, camara.z);

float velocidad = VELOCIDAD_MAXIMA;

int profundidad = camara.z;

GLMmodel* virus = NULL;
GLMmodel* globulo_blanco = NULL;

int frames;
int seconds;

bool paused;

GLint botonMouse; // boton izquierdo del mouse
int mouseX = 0, mouseY = 0; // ultimas coordenadas conocidas del mouse

using namespace std;
using namespace DromeAudio;

vector<Punto*>* globulosRojos;
vector<Punto*>* globulosBlancos;

vector<Rayo*>* rayos;

char string_globulo_blanco[] = "data/ghost.obj";
char string_virus[] = "data/virus.obj";

SoundEmitterPtr emitter_disparo;
AudioContext* context_disparo;

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
    glutSolidTorus(innerRadius, outerRadius, 12, 12);
    glPopMatrix();
}

void sonido(string archivo) {
    SoundPtr sound;
    try {
        sound = Sound::create(archivo.c_str());
    } catch(Exception ex) {
        fprintf(stderr, "Couldn't open");
    }
    AudioDriver *driver;
    try {
        driver = AudioDriver::create();
    } catch(Exception ex) {
        fprintf(stderr, "Audio driver initialization failed\n");
    }

    // create context
    AudioContext *context = new AudioContext(driver->getSampleRate());
    driver->setAudioContext(context);

    // create sound emitter and loop until it's done playing
    SoundEmitterPtr emitter = context->playSound(sound);
    emitter->setLoop(false);
}

void configurarEscena() { 
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);
    //glShadeModel(GL_FLAT);

    // Luz ambiental
    float light_position[] = {0,1,0,0};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLineWidth(3);
}

// Escucha el estado actual del mouse y actualiza las estructuras de datos
// pertinentes
void mouse(int boton, int estado, int x, int y)
{
    if (boton == GLUT_LEFT_BUTTON) {
        rayos->push_back(new Rayo(Punto(nave.x, nave.y, nave.z), nave.z));
    }
}

void movimientoMouse(int x, int y)
{
}

void teclasSoltar(unsigned char tecla, int x, int y)
{
    switch (tecla) {
        case 'w':
        case 'W':
            estadosFlechas[2] = false;
            break;
        case 's':
        case 'S':
            estadosFlechas[3] = false;
            break;
        case 'a':
        case 'A':
            estadosFlechas[0] = false;
            break;
        case 'd':
        case 'D':
            estadosFlechas[1] = false;
            break;
    }
}


void teclas(unsigned char tecla, int x, int y) {
    switch (tecla) {
        // ESC = Salir
        case 27:
            exit(0);
        case 'w':
        case 'W':
            estadosFlechas[2] = true;
            break;
        case 's':
        case 'S':
            estadosFlechas[3] = true;
            break;
        case 'a':
        case 'A':
            estadosFlechas[0] = true;
            break;
        case 'd':
        case 'D':
            estadosFlechas[1] = true;
            break;
        /*
        case 'e':
        case 'E':
            camara.z -= 5;
            break;
        // Q, dolly out
        case 'q':
        case 'Q':
            camara.z += 5;
            break;
            */
        case 'p':
        case 'P':
            paused = !paused;
            break;
        case '+':
            if (paused) {
                paused = false;
            }
            else if (velocidad < VELOCIDAD_MAXIMA)
                velocidad *= 2;
            break;
        case '-':
            if (velocidad > VELOCIDAD_MINIMA) {
                velocidad /= 2;
            }
            else {
                paused = true;
            }
            break;
        case ' ':
            rayos->push_back(new Rayo(Punto(nave.x, nave.y, nave.z), nave.z));
            break;
    }
}

void teclasEspeciales(int tecla, int x, int y)
{
    switch (tecla) {
        case GLUT_KEY_LEFT:
                estadosFlechas[0] = true;
                break;
        case GLUT_KEY_RIGHT:
                estadosFlechas[1] = true;
                break;
        case GLUT_KEY_UP:
                estadosFlechas[2] = true;
                break;
        case GLUT_KEY_DOWN:
                estadosFlechas[3] = true;
                break;
    }
}

void teclasEspecialesSoltar(int tecla, int x, int y)
{
    switch (tecla) {
        case GLUT_KEY_LEFT:
            estadosFlechas[0] = false;
            break;
        case GLUT_KEY_RIGHT:
            estadosFlechas[1] = false;
            break;
        case GLUT_KEY_UP:
            estadosFlechas[2] = false;
            break;
        case GLUT_KEY_DOWN:
            estadosFlechas[3] = false;
            break;
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

void cargarModelos() {
    virus = glmReadOBJ(string_virus);
    glmUnitize(virus);
    glmFacetNormals(virus);
    glmVertexNormals(virus, 90.0);
    globulo_blanco = glmReadOBJ(string_globulo_blanco);
    glmUnitize(globulo_blanco);
    glmFacetNormals(globulo_blanco);
    glmVertexNormals(globulo_blanco, 90.0);
}

void dibujarModelo(float x, float y, float z, float scale, GLMmodel* m) {
    glPushMatrix();

    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    glRotatef(180.0f, 0.0f, 230.0f, 0.0f);
    glmDraw(m, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();
}

void obtenerGlobulosRojos(float z, float p) {
    if (p >= 0.95) {
        float r_x = 1.6 * rand()/float(INT_MAX) - 0.8;
        float r_y = 1.2 * rand()/float(INT_MAX) - 0.6;
        globulosRojos->push_back(new Punto(r_x, r_y, z));

        if (p >= 0.97) {
            globulosRojos->push_back(new Punto(r_x, r_y, z - 10));
            globulosRojos->push_back(new Punto(r_x, r_y, z - 20));
        }
    }
}

void obtenerGlobulosBlancos(float z, float p) {
    if (p >= 0.9) {
        float r_x = 1.6 * rand()/float(INT_MAX) - 0.8;
        float r_y = 1.2 * rand()/float(INT_MAX) - 0.6;
        globulosBlancos->push_back(new Punto(r_x, r_y, z));
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

    revisarFlechas();

    camara.z -= velocidad;

    if (!(frames % int(10 * VELOCIDAD_MAXIMA / velocidad))) {
        float pRojos = rand()/float(INT_MAX);
        float pBlancos = rand()/float(INT_MAX);
        // Adaptar al jugador!

        obtenerGlobulosRojos(camara.z - 30, pRojos);
        obtenerGlobulosBlancos(camara.z - 30, pBlancos);
    }

    vector<Punto*>::iterator it;
    for (it = globulosRojos->begin(); it < globulosRojos->end(); ++it) {
        anillo((*it)->x, (*it)->y, (*it)->z, 0.05, 0.25, 1);
    }
    if (!globulosRojos->empty() && globulosRojos->front()->z > camara.z)
        globulosRojos->erase(globulosRojos->begin());

    for (it = globulosBlancos->begin(); it < globulosBlancos->end(); ++it) {
        dibujarModelo((*it)->x, (*it)->y, (*it)->z, 0.2, globulo_blanco);
    }
    if (!globulosBlancos->empty() && globulosBlancos->front()->z > camara.z)
        globulosBlancos->erase(globulosBlancos->begin());

    vector<Rayo*>::iterator it2;
    // pendiente de los rayos que nunca se dejan de dibujar
    for (it2 = rayos->begin(); it2 < rayos->end(); ++it2) {
            glDisable(GL_LIGHTING);
            glColor3ub(252, 238, 113);
            glBegin(GL_LINES);
            glVertex3f((*it2)->pos.x, (*it2)->pos.y, (*it2)->pos.z);
            glVertex3f((*it2)->pos.x, (*it2)->pos.y, (*it2)->pos.z - 2);
            (*it2)->pos.z -= velocidad * 2;
            glEnd();
            glEnable(GL_LIGHTING);
    }
    if (!rayos->empty() && rayos->front()->z_inicial > camara.z + 20) {
        rayos->erase(rayos->begin());
    }

    glColor3ub(252, 238, 113);
    dibujarModelo(nave.x, nave.y, nave.z, 0.2, virus);
    //esfera(nave.x, nave.y, nave.z, 0.05);

    glColor3ub(255, 0, 0);
    nave.z = camara.z - 2;

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

    configurarEscena();
    cargarModelos();
    sonido("data/darling.mp3");

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutReshapeFunc(cambioventana);

    glutKeyboardFunc(teclas);
    glutKeyboardUpFunc(teclasSoltar);
    glutSpecialFunc(teclasEspeciales);
    glutSpecialUpFunc(teclasEspecialesSoltar);

    glutMouseFunc(mouse);
    glutMotionFunc(movimientoMouse);

    globulosRojos = new vector<Punto*>;
    globulosBlancos = new vector<Punto*>;
    rayos = new vector<Rayo*>;

    srand(time(NULL));
    glutMainLoop();

    return 0;
}

