//#include<windows.h>
#include<GL/glu.h>
#include<GL/glut.h>
#include "mainMemory.h"

void updatePosition();

Drawable d1,d2;
point position(-3,0,0);
float angle1=0,angle2=0;

float speed=0.8;
point getVector(float xx,float yy,int bb=0)
{
    if(bb==1)
    {
        xx-=3.141592653589/2;
        return point{cos(xx),sin(xx),0};
    }
    if(bb==2)
        yy+=3.141592653589/2;
    return point{cos(xx)*cos(yy),sin(xx)*cos(yy),sin(yy)};
}

void MyInit()
{
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1,1,-1,1,1,30);
    glMatrixMode(GL_MODELVIEW);

}

void idleFunction() {
    updatePosition();
    glutPostRedisplay();
}
void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    point cen=getVector(angle1,angle2,0)+position;
    point up=getVector(angle1,angle2,2);
    glLoadIdentity();
    gluLookAt(position.x,position.y,position.z,cen.x,cen.y,cen.z,up.x,up.y,up.z);
    d1.draw();
    d2.draw();
    glutSwapBuffers();
}
bool cameraMove[4]={0,0,0,0};// 0 1 2 3 right up left down
static void specialKey(int key,int xx,int yy)
{
    switch(key)
    {

    case GLUT_KEY_LEFT:
    {
        cameraMove[0]=1;
        break;
    }
    case GLUT_KEY_UP:
    {
        cameraMove[1]=1;
        break;
    }
    case GLUT_KEY_RIGHT:
    {
        cameraMove[2]=1;
        break;
    }
    case GLUT_KEY_DOWN:
    {
        cameraMove[3]=1;
        break;
    }
    }
}
static void specialKeyUp(int key,int xx,int yy)
{
    switch(key)
    {

    case GLUT_KEY_LEFT:
    {
        cameraMove[0]=0;
        break;
    }
    case GLUT_KEY_UP:
    {
        cameraMove[1]=0;
        break;
    }
    case GLUT_KEY_RIGHT:
    {
        cameraMove[2]=0;
        break;
    }
    case GLUT_KEY_DOWN:
    {
        cameraMove[3]=0;
        break;
    }
    }
}
point vecrl,vecfb;
bool mov[6]={0,0,0,0,0,0};// 0 1 2 3  4 5// right front left back up down
float t[2]={0,0};
void updatePosition() {
    vecrl=getVector(angle1,angle2,1)*speed*(t[1]-t[0]);
    vecfb=getVector(angle1+acos(-1)/2,angle2,1)*speed*(t[1]-t[0]);
    if(cameraMove[0])
    {
        angle1+=speed*(t[1]-t[0]);
    }
    if(cameraMove[1])
    {
        angle2+=speed*(t[1]-t[0]);
    }
    if(cameraMove[2])
    {
        angle1-=speed*(t[1]-t[0]);
    }
    if(cameraMove[3])
    {
        angle2-=speed*(t[1]-t[0]);
    }
    if(mov[0])
    {
        position=position+vecrl;
    }
    if(mov[1])
    {
        position=position+vecfb;
    }
    if(mov[2])
    {
        position=position-vecrl;
    }
    if(mov[3])
    {
        position=position-vecfb;
    }
    if(mov[4])
        position.z+=speed*(t[1]-t[0]);
    if(mov[5])
        position.z-=speed*(t[1]-t[0]);
    t[0]=t[1];
    t[1]=Timer::current_time();
}
void Key(unsigned char ch,int x,int y)
{
    switch(ch)
    {
    case 's': { mov[3]=1; break; }
    case 'w': { mov[1]=1; break; }


    case 'a': { mov[2]=1; break; }
    case 'd': { mov[0]=1; break; }

    case 'z': { mov[4]=1; break; }
    case 'x': { mov[5]=1; break; }
    }
}
void KeyUp(unsigned char ch,int x,int y)
{
    switch(ch)
    {
    case 's': { mov[3]=0; break; }
    case 'w': { mov[1]=0; break; }
    case 'a': { mov[2]=0; break; }
    case 'd': { mov[0]=0; break; }
    case 'z': { mov[4]=0; break; }
    case 'x': { mov[5]=0; break; }
    }
}
const int W=800,H=600;
float cx,cy;
void mouseMotion(int x,int y){
    cx=float(x)/W;
    cy=float(y)/H;
    cx=cx*2-1;
    cy=cy*2-1;
    angle1=-cx*acos(-1);
    angle2=-cy*acos(-1)/2;
}

void mouseMotionClick(int x,int y){

}

int main(int argc,char* argv[ ])
{
    cout<<"camera direction left right up down arrow"<<endl;
    cout<<"camera move with a s d w z x"<<endl;
    glutInit(&argc,argv);
    glutInitWindowSize(W,H);
    glutInitWindowPosition(100,150);
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
    glutCreateWindow("Color Cube with Camera");
   MyInit();
   glutIdleFunc(idleFunction);
    glutDisplayFunc(Draw);
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMotionFunc(mouseMotionClick);
    Timer::start();

    //project.obj   project(1).obj
    d1.loadFile("files/project.obj");
    d1.rotate("ship","diameter1",1,1,-3.14159/2,3.14159/2);
    d1.rotate("game","diameter2",1);
    d1.rotate("train","diameter3",1);
    d1.rotate("swing1","diameter4",1,1,-3.14159/4,3.14159/4);
    d1.rotate("swing2","diameter4",1.1,1,-3.14159/4,3.14159/4);
    d1.rotate("swing3","diameter4",1.2,1,-3.14159/4,3.14159/4);

    glutMainLoop();

    return 0;
}// g++ -Wall  main.cpp -lGL -lglut -lGLU   -o main.o && ./main.o
