#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef enum coordenada {coordenada_x, coordenada_y, coordenada_z};

float x_camara = 30.0;
float y_camara = 30.0;
float z_camara = 100.0;

void ConfiguracionEscena() { 
    float position[] = {0,0,1,0};
    float diffuse[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_LIGHT0);

}

void teclas(unsigned char tecla, int x, int y) {
    switch (tecla) {
        // ESC = Salir
        case 27:
            exit(0);
        // W, arriba
        case 119:
        case 87:
            printf("W\n");
            break;
        // A, izquierda
        case 97:
        case 65:
            printf("A\n");
            break;
        // S, abajo
        case 115:
        case 83:
            printf("S\n");
            break;
        // D, derecha
        case 100:
        case 68:
            printf("D\n");
            break;
        // E, dolly in
        case 69:
        case 101:
            z_camara -= 1.0;
            printf("E\n");
            break;
        // Q, dolly out
        case 81:
        case 113:
            printf("Q\n");
            break;
    }
}

void malla(coordenada c) {
    double delta;
    for (int i = 1; i < 20 ; i++) {
        delta = i * 5.0;
        switch (c) {
            case coordenada_x:
                glVertex3f(0.0, delta, 0.0);
                glVertex3f(100.0, delta, 0.0);
                glVertex3f(0.0, 0.0, delta);
                glVertex3f(100.0, 0.0, delta);
                break;
            case coordenada_y:
                glVertex3f(delta, 0.0, 0.0);
                glVertex3f(delta, 100.0, 0.0);
                glVertex3f(0.0, 0.0, delta);
                glVertex3f(0.0, 100.0, delta);
                break;
            case coordenada_z:
                glVertex3f(delta, 0.0, 0.0);
                glVertex3f(delta, 0.0, 100.0);
                glVertex3f(0.0, delta, 0.0);
                glVertex3f(0.0, delta, 100.0);
                break;
        }
    }
}

void display(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f ,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gluLookAt(x_camara, y_camara, z_camara,
              0.0, 0.0, 0.0,0.0,1.0,0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

    // EJE X
    glBegin(GL_LINES);
    glColor3ub(255,0,0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(100.0,0.0,0.0);
    malla(coordenada_x);

    // EJE Y
    glColor3ub(0,255,0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,100.0,0.0);
    malla(coordenada_y);

    // EJE Z
    glColor3ub(0,0,255);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,100.0);
    malla(coordenada_z);
    glEnd();

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


    gluPerspective(35.0f, aspectratio, 1.0, 300.0);
}

int main(int argc,char** argv) {
    glutInit(&argc,argv);
    glutInitWindowSize (600, 600); 
    glutInitWindowPosition (10, 50);
    glutCreateWindow ("Proyecto 1");    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    ConfiguracionEscena();

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(cambioventana);

    glutKeyboardFunc(teclas);

    glutMainLoop();

    return 0;
}
