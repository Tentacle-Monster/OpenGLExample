// OPenglStuff.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<freeglut.h>
#include "customGeometry3D.h" 
#include "Model.h"


#define SLIDERS

void ResizeFunction(int, int);
void RenderFunction(void);
void mouse(int button, int state, int x, int y);
void matrixOut(double matrix[], int xSize=4, int ySize=4);
void My_mouse_routine(int x, int y);

#ifdef SLIDERS//============SLIDER=DEFINES==================

struct SLider {
    double xMin;
    double xMax;
    double yMin;
    double yMax;
    bool isHorizontal;
    double percent;

};
#define SLIDER_SCALE 0.1f
#define SLIDER_COUNT 10
#define MAX_Z -2
#define DELTA_TRANS 6
SLider sliders[SLIDER_COUNT] = {
    {0.6,0.99,0.9,1,true,0.54},
    {0.6,0.99,0.8,0.9,true,0.54},
    {0.6,0.99,0.7,0.8,true,0.064},
    {0.6,0.99,0.6,0.7,true,0.61},
    {0.6,0.99,0.5,0.6,true,0.56},
    {0.6,0.99,0.4,0.5,true,0.47},
    {0.6,0.99,0.3,0.4,true,1},
    {0.6,0.99,0.2,0.3,true,1},
    {0.6,0.99,0.1,0.2,true,1},
    {0.6,0.99,0.0,0.1,true,0.2}
};

int catchedSlider = -1;

void SetPercent(double coordX, double coordY, int sliderNo) {
    double percentX = (coordX - sliders[sliderNo].xMin) / (sliders[sliderNo].xMax - sliders[sliderNo].xMin);
    double percentY = (coordY - sliders[sliderNo].yMin) / (sliders[sliderNo].yMax - sliders[sliderNo].yMin);
    double percent = sliders[sliderNo].isHorizontal ? percentX : percentY;
    if (percent < 0) percent = 0;
    if (percent > 1) percent = 1;
    sliders[sliderNo].percent = percent;
}


#endif // SLIDERS=====================================



Model model1;
double scalef{0.3};
bool mousePressed = false;
int width, height;


void setupModel() {
    double c = 1.f / 4;
    double H = 0.7*c, W = c, L = 4 * c;
    model1.setupDots({
        {-L,-W,-H},
        {-L,-W,H},
        {-L,W,-H},
        {-L,W,H},
        {L,-W,-H},
        {L,-W,H},
        {L,W,-H},
        {L,W,H}
        // axis point
        });
    model1.setupPlanes({
        {0,1,5,4},
        {1,3,7,5},
        {3,2,6,7},
        {2,0,4,6},
        {0,1,3,2},
        {4,5,7,6}
        });
    model1.setupLines({
        {0,4},
        {1,5},
        {3,7},
        {2,6}, //outer tube

        {0,1},
        {1,3},
        {3,2},
        {2,0},//outer front

        {4,5},
        {5,7},
        {7,6},
        {6,4},//outer back
        {5,2}//axis
        });
    model1.setPosition(0, 0, -1);
    model1.setScale(0.3, 0.3, 0.3);
    model1.setRotation(0, 0, 0);
    model1.setupAxis(L, -W, H, -L, W, -H);
}


int main(int argc, char* argv[])
{
    setupModel();
    model1.setPosition(1, 1, -2);
    model1.setScale(0.2, 0.2, 0.2);
    model1.setRotation(0, 0, 0);
    model1.setupRotAxis(10);
    glutInit(&argc, argv);
    //  Request double buffered true color window with Z-buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glMatrixMode(GL_MODELVIEW);
    glutCreateWindow("figure view");
    glEnable(GL_DEPTH_TEST);
    glutMouseFunc(mouse);
    glutDisplayFunc(RenderFunction);
    glutReshapeFunc(ResizeFunction);
    glutMotionFunc(My_mouse_routine);
    glutReshapeWindow(1440, 780);
    glutMainLoop();
    return 0;

}



void matrixOut(double matrix[], int xSize, int ySize) {
    for (int y = 0; y < ySize; y++) {
        std::cout << std::endl;
        for (int x = 0; x < xSize; x++) {
            std::cout << matrix[x + xSize * y] << "  |  ";
        }
    }
}

void ResizeFunction(int Width, int Height)
{
    width = Width; height = Height;
    glViewport(0, 0, width, height);
    model1.setAspect((float)width / height);
   // glMatrixMode(GL_PROJECTION);

}

void My_mouse_routine(int x, int y)
{
    if (mousePressed) {
#ifdef  SLIDERS
        {
            float coordX = (x / (float)width) * 2 - 1;
            float coordY = ((height - y) / (float)height) * 2 - 1;
    
            if (catchedSlider != -1) {
                SetPercent(coordX, coordY, catchedSlider);
            }
        }
#endif
    }


    //puts("--||--");
 //mouse_x = x; place current mouse pos in mouse_x
 //mouse_y = y;

}


void mouse(int button, int state, int x, int y)
{

    
    switch (button) {


    case GLUT_LEFT_BUTTON:
        if (state == GLUT_UP) {
            mousePressed = false;
            catchedSlider = -1;
        }
        else {
            mousePressed = true;
#ifdef SLIDERS
            catchedSlider = -1;
            float coordX = (x / (float)width) * 2 - 1;
            float coordY = ((height - y) / (float)height) * 2 - 1;
            for (int i = 0; i < SLIDER_COUNT; i++) {
                if ((coordX > sliders[i].xMin)
                    && (coordX < sliders[i].xMax)
                    && (coordY > sliders[i].yMin)
                    && (coordY < sliders[i].yMax)
                    ) {
                    catchedSlider = i;
                    SetPercent(coordX, coordY, i);
                    
                }
            }
#endif // SLIDERS
        }
        break;

    case  3:
        if (scalef > 0.05)scalef -= 0.01;
        model1.setScale(scalef, scalef, scalef);
        break;
    case 4:
        if (scalef < 2.0) scalef += 0.01;
        model1.setScale(scalef, scalef, scalef);
        break;
    }
}

#ifdef  SLIDERS



void DrawSliders() 
{
    for (int i = 0; i < SLIDER_COUNT; i++) {

        double x1 = sliders[i].xMin;
        double x2 = sliders[i].xMax;
        double y1 = sliders[i].yMin;
        double y2 = sliders[i].yMax;
        double percent = sliders[i].percent;

        double x3, x4, y3, y4 = 0;
        double midY = (y1 + y2) / 2;
        double midX = (x1 + x2) / 2;
        double deltaY = fabs(y2 - y1);
        double deltaX = fabs(x2 - x1);
        double centrX, centrY=0;

        if (sliders->isHorizontal) {
            y1 = midY - deltaY / 2 * SLIDER_SCALE;
            y2 = midY + deltaY / 2 * SLIDER_SCALE;
            centrY = midY;
            centrX = x1 + deltaX * percent;
            x3 = centrX - deltaX/2 * SLIDER_SCALE;
            x4 = centrX + deltaX/2 * SLIDER_SCALE;
            y3 = centrY - deltaY / 4;
            y4 = centrY + deltaY / 4;

        }
        else {
            x1 = midX - deltaY / 2 * SLIDER_SCALE;
            x2 = midX + deltaY / 2 * SLIDER_SCALE;
            centrX = midX;
            centrY = y1 + deltaY * percent;
            y3 = centrY - deltaY/2 * SLIDER_SCALE;
            y4 = centrY + deltaY/2 * SLIDER_SCALE;
            x3 = centrX - deltaX / 4;
            x4 = centrX + deltaX / 4;
        }
        glColor3f(1, 0, 0);
        glRectd(x1,y1,x2,y2);
        glRectd(x3, y3, x4, y4);
}

}
#endif //  SLIDERS

void RenderFunction(void)
{
#ifdef SLIDERS
    model1.setPosition((DELTA_TRANS / 2-(sliders[0].percent * DELTA_TRANS) ),
        (DELTA_TRANS / 2 - (sliders[1].percent * DELTA_TRANS)),
        (MAX_Z - (sliders[2].percent * DELTA_TRANS/2))
    );
    model1.setRotation(sliders[3].percent * 360, sliders[4].percent * 360, sliders[5].percent * 360);
    model1.setScale(sliders[6].percent, sliders[7].percent, sliders[8].percent);
    model1.setupRotAxis(sliders[9].percent * 360);
#endif // SLIDERS

    glClearColor(0.f, 0.f, 0.f,1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glLineWidth(1.5);
  //  glPointSize(6.5);
    model1.render();
#ifdef  SLIDERS
    DrawSliders();
#endif //  SLIDERS
    glutSwapBuffers();
    glFlush();
    glutPostRedisplay();
}