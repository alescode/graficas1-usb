#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <deque>
#include <sstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <AL/alut.h>
#else
#include <GL/glut.h>
#endif

#include "../lib/constantes.h"
#include "../lib/punto.h"
#include "../lib/objeto.h"
#include "../lib/glm.h"

#define VELOCIDAD_MAXIMA 0.125
#define VELOCIDAD_MINIMA 0.015625
int contador_velocidad;

typedef enum coordenada {coordenada_x, coordenada_y, coordenada_z};
typedef enum color {rojo, gris};

bool estadosFlechas[4] = {false};

Punto camara(0, 0, 100000);
Punto nave(0, 0, camara.z);

float velocidad = VELOCIDAD_MAXIMA;

int profundidad = camara.z;

GLMmodel* virus = NULL;
GLMmodel* globulo_blanco = NULL;

int frames;
float tiempo, tiempo_juego;
float tiempos_velocidades[3] = {0};
float inicio_pausa, fin_pausa;
/* 0: 0.5x
 * 1: 0.25x
 * 2: 0.125x */

bool paused;

GLint botonMouse; // boton izquierdo del mouse
int mouseX, mouseY; // ultimas coordenadas conocidas del mouse
int clicks;

using namespace std;

deque<Punto*>* globulosRojos;
deque<Punto*>* globulosBlancos;

deque<Objeto*>* rayos;

#define NUM_SONIDOS 2
ALuint sfx[NUM_SONIDOS];
ALuint sfx_fuentes[NUM_SONIDOS];

int numero_globulo = 1;

char string_globulo_blanco[] = "data/ghost.obj";
char string_virus[] = "data/virus.obj";

int score;

void esfera(float x, float y, float z, float radius) {
    glPushMatrix();
    glTranslatef(x,y,z);
    glutSolidSphere(radius, 3, 3);
    glPopMatrix();
}

void anillo(float x, float y, float z,
        float innerRadius, float outerRadius,
        float rotate, color c) {
    glPushMatrix();
    glTranslatef(x,y,z);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    if (c == rojo)
        glColor4f(1.0, 0.0, 0.0, 0.0);
    else
        glColor4f(0.3, 0.3, 0.3, 0.0);

    glutSolidTorus(innerRadius, outerRadius, 12, 12);
    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();
}

void dibujarTexto(const char *string, float x,float y,float z, float scale) {
    const char *c;
    glPushMatrix();
    glTranslatef(x,y,z);
    glScalef(0.018f,0.016f,z);
    glDisable(GL_LIGHTING);
    glColor3d(1.0f, 1.0f, 1.0f);
    glScalef(scale, scale, scale);
    for (c=string; *c != '\0'; c++)
    {
        glLineWidth(4);
        glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void cargarSonidos() {
    alutInit(0, NULL);
    alGenBuffers(NUM_SONIDOS, sfx);
    ALenum format;
    ALvoid* data;
    ALsizei size, freq;

    ALfloat listenerPos[]={0.0,0.0,4.0};
    ALfloat listenerVel[]={0.0,0.0,0.0};
    ALfloat listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};
    ALfloat source0Pos[]={-2.0, 0.0, 0.0};
    ALfloat source0Vel[]={ 0.0, 0.0, 0.0};
    alListenerfv(AL_POSITION,listenerPos);
    alListenerfv(AL_VELOCITY,listenerVel);
    alListenerfv(AL_ORIENTATION,listenerOri);

    alGenBuffers(NUM_SONIDOS, sfx);
    alutLoadWAVFile((ALbyte*) "data/darling.wav",&format,&data,&size,&freq);
    alBufferData(sfx[0],format,data,size,freq);
    alutUnloadWAV(format,data,size,freq);
    alutLoadWAVFile((ALbyte*) "data/disparo.wav",&format,&data,&size,&freq);
    alBufferData(sfx[1],format,data,size,freq);
    alutUnloadWAV(format,data,size,freq);

    alGenSources(NUM_SONIDOS, sfx_fuentes);
    alSourcef(sfx_fuentes[0],AL_PITCH,1.0f);
    alSourcef(sfx_fuentes[0],AL_GAIN,1.0f);
    alSourcefv(sfx_fuentes[0],AL_POSITION,source0Pos);
    alSourcefv(sfx_fuentes[0],AL_VELOCITY,source0Vel);
    alSourcei(sfx_fuentes[0],AL_BUFFER, sfx[0]);
    alSourcei(sfx_fuentes[0],AL_LOOPING,AL_FALSE);

    alSourcef(sfx_fuentes[1],AL_PITCH,1.0f);
    alSourcef(sfx_fuentes[1],AL_GAIN,1.0f);
    alSourcefv(sfx_fuentes[1],AL_POSITION,source0Pos);
    alSourcefv(sfx_fuentes[1],AL_VELOCITY,source0Vel);
    alSourcei(sfx_fuentes[1],AL_BUFFER, sfx[1]);
    alSourcei(sfx_fuentes[1],AL_LOOPING,AL_FALSE);
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

void dibujarModelo(float x, float y, float z, float scale, GLMmodel* m, GLenum mode) {
    if (mode == GL_SELECT) {
        glLoadName(1);
    }

    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    glRotatef(180.0f, 0.0f, 230.0f, 0.0f);
    glmDraw(m, GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();
}


#define BUFSIZE 512

Punto pixelesACoordenadas(int x, int y) {
    return Punto((x - 512)/1024.0 * 1.6,
                 -(y - 384)/768.0 * 1.2, 0);
}

void disparar() {
    rayos->push_back(new Objeto(Punto(nave.x, nave.y, nave.z), nave.z));
    //alSourcePlay(sfx_fuentes[1]);
}

void mouse(int boton, int estado, int x, int y)
{
    clicks += 1;
    if (boton != GLUT_LEFT_BUTTON)
        return;

    Punto p = pixelesACoordenadas(x, y);

    deque<Punto*>::iterator it;
    mouseX = x;
    mouseY = y;
    disparar();

    GLuint selectBuf[BUFSIZE];
    GLint hits;
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glSelectBuffer(BUFSIZE, selectBuf);
    (void) glRenderMode(GL_SELECT);

    glInitNames();
    glPushName(0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    /*  create 5x5 pixel picking region near cursor location */
    gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y),
            5.0, 5.0, viewport);
    glOrtho(0.0, 8.0, 0.0, 8.0, -0.5, 2.5);

    glLoadName(1);
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3i(nave.x - 10, nave.y + 10, nave.z);
    glVertex3i(nave.x + 10, nave.y + 10, nave.z);
    glVertex3i(nave.x + 10, nave.y - 10, nave.z);
    glVertex3i(nave.x - 10, nave.y - 10, nave.z);
    glEnd();

    glPopMatrix();
    glFlush();

    hits = glRenderMode(GL_RENDER);
}

void movimientoMouse(int x, int y)
{
    if (botonMouse == GLUT_DOWN) {
    }
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

inline int posicion_velocidad_inicio(float velocidad) {
    return 2 * log2(VELOCIDAD_MAXIMA/velocidad) - 2;
}

inline int posicion_velocidad_final(float velocidad) {
    return 2 * log2(VELOCIDAD_MAXIMA/velocidad - 1) - 2;
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
        case 'p':
        case 'P':
            paused = !paused;
            if (paused)
                inicio_pausa += tiempo;
            else
                fin_pausa += tiempo;
            break;
        case '+':
            if (paused) {
                paused = false;
                fin_pausa += tiempo;
            }
            else if (velocidad < VELOCIDAD_MAXIMA)
                velocidad *= 2;
            contador_velocidad = 90;
            break;
        case '-':
            if (velocidad > VELOCIDAD_MINIMA) {
                velocidad /= 2;
            }
            else {
                paused = true;
                inicio_pausa += tiempo;
            }
            contador_velocidad = 90;
            break;
        case ' ':
            disparar();
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
    frames += 1;
    tiempo = glutGet(GLUT_ELAPSED_TIME);
    if (paused) {
        dibujarTexto((char*) "pausa", -0.15, 0, camara.z - 1, 0.05f);
        glFlush();
        return;
    }

    //int pos_velocidades = log2(VELOCIDAD_MAXIMA/velocidad) - 1;
    /*if (velocidad < VELOCIDAD_MAXIMA)
        tiempos_velocidades[pos_velocidades] =
    */
    tiempo_juego = (tiempo
                    - 0.5 * tiempos_velocidades[0]
                    - 0.25 * tiempos_velocidades[1]
                    - 0.125 * tiempos_velocidades[2]
                    + inicio_pausa
                    - fin_pausa)/1000.0;

    //if (tiempo > 0)
    //    cout << frames / (tiempo/1000.0) << endl;

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

    dibujarModelo(nave.x, nave.y, nave.z, 0.2, virus, GL_RENDER);
    deque<Punto*>::iterator it;

    for (it = globulosRojos->begin(); it < globulosRojos->end(); ++it) {
        if ((*it)->z >= nave.z &&
            (*it)->y - 0.2 <= nave.y && nave.y <= (*it)->y + 0.2 &&
            (*it)->x - 0.2 <= nave.x && nave.x <= (*it)->x + 0.2) {
                anillo((*it)->x, (*it)->y, (*it)->z, 0.05, 0.25, 1, gris);
            if ((*it)->z == nave.z) {
                score += 1;
            }
        }
        else {
            anillo((*it)->x, (*it)->y, (*it)->z, 0.05, 0.25, 1, rojo);
        }
    }
    for (it = globulosBlancos->begin(); it < globulosBlancos->end(); ++it) {
        dibujarModelo((*it)->x, (*it)->y, (*it)->z, 0.2, globulo_blanco, GL_RENDER);
        if ((*it)->z == nave.z &&
            (*it)->y - 0.2 <= nave.y && nave.y <= (*it)->y + 0.2 &&
            (*it)->x - 0.2 <= nave.x && nave.x <= (*it)->x + 0.2) {
                score = max(0, score - 3);
        }
    }

    deque<Objeto*>::iterator it2;
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
        for (it = globulosBlancos->begin(); it < globulosBlancos->end(); ++it) {
            if ((*it)->z == (*it2)->pos.z &&
                (*it)->y - 0.4 <= (*it2)->pos.y && (*it2)->pos.y <= (*it)->y + 0.4 &&
                (*it)->x - 0.4 <= (*it2)->pos.x && (*it2)->pos.x <= (*it)->x + 0.4) {
                (*it2)->pos.z = camara.z - 100;
                (*it)->z = camara.z + 100;
                score += 1;
            }
        }
    }

    std::stringstream out;
    out << score;
    dibujarTexto(out.str().c_str(), 18, 12.5, camara.z - 50, 1);

    std::stringstream out2;
    //out2 << ceil(60 - tiempo_juego) << endl;
    out2 << (60 - tiempo_juego) << endl;
    dibujarTexto(out2.str().c_str(), -20, 12.5, camara.z - 50, 1);

    if (contador_velocidad) {
        contador_velocidad -= 1;
        std::stringstream out3;
        out3 << velocidad / VELOCIDAD_MAXIMA << "x";
        dibujarTexto(out3.str().c_str(), 13, -14, camara.z - 50, 1);
    }

    // Recoleccion de basura
    if (!globulosRojos->empty() && globulosRojos->front()->z > camara.z)
        globulosRojos->pop_front();
    if (!globulosBlancos->empty() && globulosBlancos->front()->z > camara.z)
        globulosBlancos->pop_front();
    if (!rayos->empty() && rayos->front()->z_inicial > camara.z + 20) {
        rayos->pop_front();
    }

    glLoadName(1);
    nave.z = camara.z - 3;

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
    cargarSonidos();
    //alSourcePlay(sfx_fuentes[0]);

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutReshapeFunc(cambioventana);

    glutKeyboardFunc(teclas);
    glutKeyboardUpFunc(teclasSoltar);
    glutSpecialFunc(teclasEspeciales);
    glutSpecialUpFunc(teclasEspecialesSoltar);

    glutMouseFunc(mouse);
    glutMotionFunc(movimientoMouse);

    globulosRojos = new deque<Punto*>;
    globulosBlancos = new deque<Punto*>;
    rayos = new deque<Objeto*>;

    srand(time(NULL));
    tiempo = 0;
    inicio_pausa = 0;
    fin_pausa = 0;

    glutMainLoop();

    return 0;
}

