// OPenglStuff.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


//  Slava-8 Roma-9 Pasha -3

#include <iostream>
#include<freeglut.h>
#include "Object2D.h"
#include <algorithm>
#include <cmath>
#include <chrono>

#define DEST_L 0
#define DEST_R 1
#define DEST_B 2
#define DEST_T 3

//#define SLAVA

typedef enum MiceState {
    NONE, DRUG,ROTATE
};

struct animation
{
    dot start;
    dot end;
    double rotations;

};


void ResizeFunction(int, int);
void RenderFunction(void);
void regularKeys(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void matrixOut(double matrix[], int xSize=4, int ySize=4);
void My_mouse_routine(int x, int y);
void makeLines();

double scalef{0.3};
//bool mousePressed = false;
int width, height;
double aspect{ 1 };
double generalScale{ 1 };



double borders[4] = {-0.75,0.75,-0.75,0.75 };//lest,right,bottom,top
std::vector<Object2D> objects;

//=====================ANIMATION=======================
std::vector<animation> anims;
double animPercent=0;
double animSpeed=0.1;
std::chrono::steady_clock::time_point lastTick {};
bool inited{ false };

//=====================BORDER CHANGE====================
bool border_editing[4] = { false,false,false,false };
double border_grub_dist{ 0.03 };
double border_limit = 0.95;
double border_dif_min = 0.1;
MiceState mice;

bool somethingEditing{false};
int object_editing{ 0 };

std::pair<double, double> lasthold{0,0};

Color colors[3] { {0,0,0},{0,0,0},{0,0,0} };

void timer(int) {
    RenderFunction();
    glutTimerFunc(1000.f / 60.f, timer, 0);
}


int main(int argc, char* argv[])                                            //инициализация
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glMatrixMode(GL_MODELVIEW);
    glutCreateWindow("figure view");
    glEnable(GL_DEPTH_TEST);
    glutMouseFunc(mouse);
    glutDisplayFunc(RenderFunction);
    glutKeyboardFunc(regularKeys);
    glutReshapeFunc(ResizeFunction);
    glutMotionFunc(My_mouse_routine);
   
    {
        Object2D box;
        box.conture = { {-0.2, 0.1}, { -0.2,-0.1 }, { 0.2,-0.1 }, { 0.2,0.1 } };
        box.scale = 0.9;
        box.position = { -0.34666666666666672 , 0 };
        box.recalculate();
        objects.push_back(box);
        anims.push_back({ { -1,0 }, {-0.34667,0},1 });


        Object2D trapecium;
        trapecium.conture = { {-0.25,0.2},{-0.6,-0.3},{0.6,-0.3},{0.25,0.2} };
        trapecium.scale = 0.9;
        trapecium.position = { 0,0 };
        trapecium.recalculate();
        objects.push_back(trapecium);
        anims.push_back({ { 1,0 }, {0,0},2 });

    }
    
    glutReshapeWindow(1440, 780);
    glutMainLoop();
    return 0;

}

std::pair<double, double> originToScreen(double x, double y) {  // координаты опенгл в координаты окна
    return std::make_pair(x*generalScale/aspect, y*generalScale);
}
std::pair<double, double> screenToOrigin(double x, double y) {  // координаты окна в координаты опенгл
    return std::make_pair(x / generalScale * aspect, y / generalScale);
}


void ResizeFunction(int Width, int Height)                      // функция изменения размеров окна
{
    width = Width; height = Height;
    glViewport(0, 0, width, height);
    aspect = ((float)width / height);
    generalScale = (aspect >= 1) ? 1 : aspect;
}

void My_mouse_routine(int x, int y)                             // функция перемещения курсора
{
    auto point = screenToOrigin(((double)x / width) * 2 - 1, -((double)y / height) * 2 + 1);
    bool changed = false;
    switch (mice)
    {
    case NONE:
        break;
    case DRUG:
        for (int i = 0; i < 4; i++) {
            if (border_editing[i]) {
                borders[i] = std::clamp(
                    ((i < 2) ? point.first : point.second),
                    (i & 1) ? borders[i - 1] + border_dif_min : -border_limit,
                    (i & 1) ? border_limit : borders[i + 1] - border_dif_min);
                changed = true;
            }
        }
        if (! changed && object_editing >= 0) {
            objects[object_editing].position.X() = std::clamp(point.first + lasthold.first, -1.0, 1.0);
            objects[object_editing].position.Y() = std::clamp(point.second + lasthold.second, -1.0, 1.0);
            objects[object_editing].recalculate();
            changed= true;
        }   
        break;
    case ROTATE:
        if (object_editing >= 0) {
            dot p{ point.first,point.second };
            dot d = p - objects[object_editing].position;
            double a1 = atan2f( d.X(), d.Y() ) / DEG_TO_RAD;
            double a2 = atan2f(-lasthold.first, -lasthold.second) / DEG_TO_RAD;
            objects[object_editing].angle = -a1;
            changed = true;
        }
        break;
    }
}

void unselectEditing() {                                //функция обнуления выбора редактируемого элемента
    for (int i = 0; i < 4; i++)
        border_editing[i] = 0;
    somethingEditing = false;
   // object_editing = -1;
    mice = MiceState::NONE;
}

bool selectEditing(double x, double y, bool canDoBorder= true) {//функция выбора редактируемого элемента
    unselectEditing();
    bool somethingChosen = false;
    if (canDoBorder) {
        if (x< (borders[DEST_R] + border_grub_dist) && x>( borders[DEST_L] - border_grub_dist)) {
            border_editing[DEST_T] = (fabs(y - borders[DEST_T] ) < border_grub_dist);
            border_editing[DEST_B] = (fabs(y - borders[DEST_B] ) < border_grub_dist);
        }
        if (y<( borders[DEST_T] + border_grub_dist) && y>( borders[DEST_B] - border_grub_dist)) {
            border_editing[DEST_L] = (fabs(x - borders[DEST_L] ) < border_grub_dist);
            border_editing[DEST_R] = (fabs(x - borders[DEST_R] ) < border_grub_dist);
        }
        for (int i = 0; i < 4; i++) {
            somethingChosen |= border_editing[i];
        }
    }
    {
        for (int i = 0; (i < objects.size() && !somethingChosen); i++) {
            if (Object2D::dotInPolyline({ x, y }, objects[i].recalculatedShape)) {
                somethingChosen = true;
                object_editing = i;
                lasthold = std::make_pair( objects[i].position.X() - x, objects[i].position.Y() -y);

                
            }
        }
    }

    somethingEditing = somethingChosen;
    return somethingChosen;
}

void regularKeys(unsigned char key, int x, int y) {    //функция нажатия клавиш
    if (key == 'p') {
        if (animPercent >= 1) {
            animPercent = 0;
            inited = false;
        }
        else {
            animPercent = 1;
        }
    }
}


void mouse(int button, int state, int x, int y)  //функция нажатия мыши 
{

    auto point = screenToOrigin(((double)x / width  )*2-1 , - ((double)y / height)*2+1);
    switch (button) {

    case GLUT_LEFT_BUTTON:
        if (state == GLUT_UP) {
            unselectEditing();
        }
        else {
            
            if (selectEditing(point.first, point.second)) mice = MiceState::DRUG;
        }
        break;

    case GLUT_RIGHT_BUTTON:

        if (state == GLUT_UP) {
            unselectEditing();
        }
        else {
            if (selectEditing(point.first, point.second,false)) mice = MiceState::ROTATE;
        }
        break;

    }


}

void drawBorder() {           //функция отрисовки отсекающего окна

  
    auto tl = originToScreen(borders[DEST_L], borders[DEST_T]);
    auto tr = originToScreen(borders[DEST_R], borders[DEST_T]);
    auto bl = originToScreen(borders[DEST_L], borders[DEST_B]);
    auto br = originToScreen(borders[DEST_R], borders[DEST_B]);

    glBegin(GL_LINES);
 //   glColor3f(0 , 0, 0);

   glColor3f(0, 0, 0);
   glVertex2f(tl.first, tl.second);
   glVertex2f(bl.first, bl.second);

   glColor3f(0, 0, 0);
   glVertex2f(tr.first, tr.second);
   glVertex2f(br.first, br.second);

   glColor3f(0, 0, 0);
   glVertex2f(br.first, br.second);
   glVertex2f(bl.first, bl.second);

   glColor3f(0, 0, 0);
   glVertex2f(tl.first, tl.second);
   glVertex2f(tr.first, tr.second);

    glEnd();
}

void drawFigure(Object2D &src) { //функция отрисовки фигуры
  

    glColor3f(src.color.R, src.color.G, src.color.B);
   

    for (auto &l : src.linesToDraw) {
        if (l.visible) { 
            glDisable(GL_LINE_STIPPLE); 
        }
        else {
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(1, 0xA0A0);
        }
        std::pair<double, double> a = originToScreen(src.dotsToDraw[l.dot1].X(), src.dotsToDraw[l.dot1].Y());
        std::pair<double, double> b = originToScreen(src.dotsToDraw[l.dot2].X(), src.dotsToDraw[l.dot2].Y());
        glBegin(GL_LINES);
        glVertex2f(a.first, a.second);
        glVertex2f(b.first, b.second);
        glEnd();

    }
    glDisable(GL_LINE_STIPPLE);
}

void splitLineWith(Object2D *src, int lineId, bool startsInvisible, std::vector<dot> &splitters, bool wasVisible =true) {  // функция рассечения линии точками

    LineToDraw * line = &(src->linesToDraw[lineId]);
    dot a1 = src->dotsToDraw[line->dot1];
    dot a2 = src->dotsToDraw[line->dot2];
    int start = line->dot1;
    int finish = line->dot2;
    int lastDot = start;
    line->visible = !startsInvisible && wasVisible;
    bool isntHor = fabs(a1.Y() - a2.Y()) > fabs(a1.X() - a2.X()) * 0.0000001;
    bool maxToMin = (src->dotsToDraw[line->dot1].data[isntHor] > src->dotsToDraw[line->dot2].data[isntHor]);
    {
        while (splitters.size()) {
            int max = 0;
            int min = 0;
            for (int i = 1; i < splitters.size(); i++) {
                if (splitters[i].data[isntHor] < splitters[min].data[isntHor])min = i;
                if (splitters[i].data[isntHor] > splitters[max].data[isntHor])max = i;
            }
            int nextDot = maxToMin ? max : min;
            src->dotsToDraw.push_back(splitters[nextDot]);

            LineToDraw newbe = { src->dotsToDraw.size() - 1,finish, startsInvisible && wasVisible};
            src->linesToDraw.push_back(newbe);
            startsInvisible = !startsInvisible;
            line->dot2 = src->dotsToDraw.size() - 1;
            line = & (src->linesToDraw[src->linesToDraw.size()-1]);
            splitters.erase(splitters.begin() + nextDot, splitters.begin() + nextDot + 1);
        }
    }
}


void SazerlendCoen(Object2D *src) {                                     //алгоритм Коэна-Сазерленда
    std::vector <char> flags;

    flags.resize(src->dotsToDraw.size() );
    for (int i = 0; i < src->dotsToDraw.size(); i++) {
        char flag = 0;
        flag |= (src->dotsToDraw[i].X() < borders[DEST_L]) << 3;
        flag |= (src->dotsToDraw[i].X() > borders[DEST_R]) << 2;
        flag |= (src->dotsToDraw[i].Y() < borders[DEST_B]) << 1;
        flag |= (src->dotsToDraw[i].Y() > borders[DEST_T]);
        flags[i ] = flag;
    }
    
    int lines_before = src->linesToDraw.size();
    for (int i = 0; i < lines_before; i++) {
        char flag1 = flags[src->linesToDraw[i].dot1];
        char flag2 = flags[src->linesToDraw[i].dot2];
        if (flag1 == 0 && flag2 == 0) {
            //line inside
        }
        else {
            //both dots out of one side
            if (flag1 & flag2) {
                src->linesToDraw[i].visible = false;
            }
            else {
                //here we go
                dot d1 = src->dotsToDraw[src->linesToDraw[i].dot1];
                dot d2 = src->dotsToDraw[src->linesToDraw[i].dot2];
                double a = (d2 - d1).Y() / (d2 - d1).X();
                double LeftY, RightY, BottomX, TopX;
                LeftY = RightY = BottomX = TopX = NAN;

                if (isinf(a)) { //vertical line
                    if (2 & (flag1 ^ flag2)) BottomX = d1.X();
                    if (1 & (flag1 ^ flag2)) TopX = d1.X();
                }
                else if (a==0) {//horizontal line
                    if (8 & (flag1 ^ flag2)) LeftY = d1.Y();
                    if (4 & (flag1 ^ flag2)) RightY = d1.Y();
                }
                else {//normal line
                    if (8 & (flag1 ^ flag2)) LeftY = d1.Y() + (borders[DEST_L] - d1.X()) * a;
                    if (4 & (flag1 ^ flag2)) RightY = d1.Y() + (borders[DEST_R] - d1.X()) * a;
                    if (2 & (flag1 ^ flag2)) BottomX = d1.X() + (borders[DEST_B] - d1.Y()) / a;
                    if (1 & (flag1 ^ flag2))TopX = d1.X() + (borders[DEST_T] - d1.Y()) / a;
                }

                std::vector<dot> intersections;
                if (LeftY > borders[DEST_B] && LeftY < borders[DEST_T]) intersections.push_back({ borders[DEST_L], LeftY });
                if (RightY > borders[DEST_B] && RightY < borders[DEST_T]) intersections.push_back({ borders[DEST_R], RightY });
                if (BottomX > borders[DEST_L] && BottomX < borders[DEST_R]) intersections.push_back({BottomX, borders[DEST_B] });
                if (TopX > borders[DEST_L] && TopX < borders[DEST_R]) intersections.push_back({ TopX, borders[DEST_T] });
        
                bool startsOutside = flag1;
                splitLineWith( src, i, startsOutside, intersections, src->linesToDraw[i].visible);

            }
        }
    }
}

bool isRight(dot a, dot b, dot c) //проверка векторов на коммунизм
{
    float abX = b.X() - a.X();
    float abY = b.Y() - a.Y();
    float acX = c.X() - a.X();
    float acY = c.Y() - a.Y();
    return abX * acY - acX * abY < 0;
}


void KirusBack(Object2D* source, Object2D* cutter) {  // обход-против часовой, нормали-внутрь!

    int linesBefore = source->linesToDraw.size();
    for (int j = 0; j < linesBefore; j++) {
        dot P1 = source->dotsToDraw[j];
        dot P2 = source->dotsToDraw[(j + 1) % linesBefore];
        dot D = P2 - P1;
        if (D.module() == 0) {
            break;
        }

        double t0 = 0;
        double t1 = 1;
        bool fuulInside = true;
        for (int i = 0; i < cutter->linesToDraw.size(); i++) {
            dot C1 = cutter->dotsToDraw[cutter->linesToDraw[i].dot1];
            dot C2 = cutter->dotsToDraw[cutter->linesToDraw[i].dot2];
            dot PNV = C2 - C1;
            dot normal = {-PNV.Y(),PNV.X()};
            double Psc = D * normal;
            dot w = P1 - C1;
            double Q = w * normal;
            if (Object2D::ifLinesCrosing2D(C1, C2, P1, P2))
            {
                if (Psc == 0) {            /* Паралл ребру или точка */
                    if (Q < 0) { /*visible = 0;  */break; }
                }
                else {
                    double  r = -Q / Psc;
                    if (Psc < 0) {          /* Поиск верхнего предела t */
                        if (r < t0) { /*visible = 0;*/  break; }
                        if (r < t1) t1 = r;
                        fuulInside = false;
                    }
                    else {               /* Поиск нижнего предела t */
                        if (r > t1) { /*visible = 0; */ break; }
                        if (r > t0) t0 = r;
                    }
                }
            }
        }
        {
            if (t1 <= t0) continue;
            std::vector<dot> splits;
            bool startsIn = false;
            if (t0 != 0) {
                splits.push_back(P1 + D * t0);
            }
            if (t1 != 1) {
                splits.push_back(P1 + D * t1);
                if (!t0) startsIn = true;
            }
            if (!splits.size() && Object2D::dotInPolyline(P1, cutter->dotsToDraw))startsIn = true;
            splitLineWith(source, j, startsIn, splits, source->linesToDraw[j].visible);
        }
    }
}

void playAnim(double delta=0.01) {
    if (animPercent >= 1) {
        if (inited) {
            animPercent = 1;
            inited = false;
        }
        else return;
    } 
    for (int i = 0; i < anims.size(); i++) {
        double angle = 360 * anims[i].rotations * animPercent;
        dot pos = anims[i].start + ((anims[i].end - anims[i].start)* animPercent);
        objects[i].position = pos;
        objects[i].angle = angle;
    }
   
    animPercent += animSpeed * delta;
}

void makeLines() {
    for (int i = 0; i < objects.size();i++) {
        auto f = &objects[i];
        f->recalculate();
        f->init_graphics();
    }
    for (int i = 0; i < objects.size(); i++) {
        if(object_editing!=i)
        KirusBack(&objects[i], &objects[object_editing]);
    }
   
    for (int i = 0; i < objects.size(); i++) {
        auto f = &objects[i];
        SazerlendCoen(f);
    }
}

void RenderFunction(void)
{

    auto now = std::chrono::high_resolution_clock::now();
    auto f = std::chrono::duration<double>(now - lastTick).count();
    lastTick = now;
    glClearColor(1.0f, 1.0f, 1.0f,1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //glPointSize(6.5);
    glLineWidth(2.5);
    if(inited) playAnim(f);
    if(animPercent<1)inited= true;

    makeLines();
    for (int i = 0; i < objects.size(); i++) {
         objects[i].color = colors[i == object_editing ? 2 : 1];
         drawFigure(objects[i]);
    }
        
    drawBorder();

    glutSwapBuffers();
    glFlush();
    

    glutPostRedisplay();
}

