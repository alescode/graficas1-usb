#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void ConfiguracionEscena() { 

    float position[] = {0,0,1,0};
    float diffuse[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glEnable(GL_LIGHT0);

}

void display(){

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f ,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gluLookAt(3.0,3.0,10.0,0.0,0.0,0.0,0.0,1.0,0.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

    glBegin(GL_LINES);
    glColor3ub(255,0,0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(50.0,0.0,0.0);

    glColor3ub(0,255,0);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,50.0,0.0);

    glColor3ub(0,0,255);
    glVertex3f(0.0,0.0,0.0);
    glVertex3f(0.0,0.0,50.0);
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
    glutReshapeFunc(cambioventana);

    glutMainLoop();

    return 0;
}
