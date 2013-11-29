#include "CSCIx229.h"

// Direction definitions
#define Up 0
#define Right 1
#define Down 2
#define Left 3
#define NA -3000

// View Globals
int first_person = 1;
int th = 0;         
int ph = 90;        
int fov = 55;       
double asp = 1;     
double dim = 20.0;   

// Lighting Globals
int emission  =   0;
int ambient   =  30;
int diffuse   = 100;
int specular  =   0;
int shininess =   0;
float shinyvec[1];  
int zh        =  90;
float ylight  =   0;

// Game Globals
int snakepos[100][2];
int speed = 500;
int size = 20;
int currentdir = Up;
int currentlen = 0;
int foodx = NA;
int foody = NA;
int crashed = 0;

void Vertex(double th, double ph) {
  double x = Sin(th) * Cos(ph);
  double y = Cos(th) * Cos(ph);
  double z =           Sin(ph);
  glNormal3d(x, y, z);
  glVertex3d(x, y, z);
}

void sphere(double x, double y, double z, double r) {
  const int d = 5;
  int th, ph;

  glPushMatrix();
  glTranslated(x, y, z);
  glScaled(r, r, r);

  for (ph = -90; ph < 90; ph += d) {
    glBegin(GL_QUAD_STRIP);
    for (th = 0; th <= 360; th += d) {
      Vertex(th, ph);
      Vertex(th, ph + d);
    }
    glEnd();
  }

  glPopMatrix();
}

void cube(double x, double y, double z, 
          double dx, double dy, double dz,
          double th) {

  // Set specular color to white
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);


  glPushMatrix();
  
  // Translations
  glTranslated(x, y, z);
  glRotated(th, 0, 1, 0);
  glScaled(dx, dy, dz);

  glColor3ub(255,255,255);

  glBegin(GL_QUADS);
    //Front
  glNormal3f( 0, 0, 1);
  glVertex3f(-1,-1, 1);
  glVertex3f(+1,-1, 1);
  glVertex3f(+1,+1, 1);
  glVertex3f(-1,+1, 1);
  //  Back
  glNormal3f( 0, 0,-1);
  glVertex3f(+1,-1,-1);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,+1,-1);
  glVertex3f(+1,+1,-1);
  //  Right
  glNormal3f(+1, 0, 0);
  glVertex3f(+1,-1,+1);
  glVertex3f(+1,-1,-1);
  glVertex3f(+1,+1,-1);
  glVertex3f(+1,+1,+1);
  //  Left
  glNormal3f(-1, 0, 0);
  glVertex3f(-1,-1,-1);
  glVertex3f(-1,-1,+1);
  glVertex3f(-1,+1,+1);
  glVertex3f(-1,+1,-1);
  //  Top
  glNormal3f( 0,+1, 0);
  glVertex3f(-1,+1,+1);
  glVertex3f(+1,+1,+1);
  glVertex3f(+1,+1,-1);
  glVertex3f(-1,+1,-1);
  glEnd();

  glPopMatrix();
}

void gameBoard() {
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

  glPushMatrix();
  
  glBegin(GL_QUADS);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-100, 0.0, 100);
  glVertex3f(-100, 0.0, -100);
  glVertex3f(100, 0.0, -100);
  glVertex3f(100, 0.0, 100);
  glEnd();

  cube(size, 0.0, 0.0,  0.5, 1.0, size,  0);
  cube(-size, 0.0, 0.0,  0.5, 1.0, size,  0);
  cube(0.0, 0.0, size,  size, 1.0, 0.5, 0);
  cube(0.0, 0.0, -size,  size, 1.0, 0.5, 0);

  glPopMatrix();

}

void initSnake() {
  int i;

  for(i = 0; i < 100; i++) {
    snakepos[i][0] = NA;
    snakepos[i][1] = NA;
  }

  snakepos[0][0] = 0; snakepos[0][1] = 0;
  snakepos[1][0] = 1; snakepos[1][1] = 0;
  snakepos[2][0] = 2; snakepos[2][1] = 0;

  currentlen = 3;

  foodx = snakepos[currentlen - 1][0];
  foody = snakepos[currentlen - 1][1];
}

void drawHead() {
  glColor3ub(0, 0, 200);
  sphere(snakepos[0][0], 1, snakepos[0][1], .4);
}

void drawBody(int i) {
  glColor3ub(0, 200, 0);
  sphere(snakepos[i][0], 1, snakepos[i][1], .4);
}

void drawFood() {
  glColor3ub(200, 0, 0);
  sphere(foodx, 1, foody, .3);
}

void drawSnake() {
  int i;

  drawHead();
  for(i = 0; i < 100; i++) {
    drawBody(i);
  }
}



void step(int dir) {
  int i;

  if(snakepos[0][0] == foodx && snakepos[0][1] == foody) {
    for(i = currentlen; i > 0; i--) {
      snakepos[i][0] = snakepos[i - 1][0];
      snakepos[i][1] = snakepos[i - 1][1];
    }       
    currentlen++;
    foodx = snakepos[currentlen - 1][0];
    foody = snakepos[currentlen - 1][1];
  } else {
    for(i = currentlen - 1; i > 0; i--) {
      snakepos[i][0] = snakepos[i - 1][0];
      snakepos[i][1] = snakepos[i - 1][1];
    }
  }

  switch(dir) {
    case Up:
      snakepos[0][1] -= 1;
      break;
    case Right:
      snakepos[0][0] += 1;
      break;
    case Down:
      snakepos[0][1] += 1;
      break;
    case Left:
      snakepos[0][0] -= 1;
      break;
  }
}

void idle() {
  double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
  zh = fmod(90 * t,360.0);
  step(currentdir);
  glutPostRedisplay();
}

void isCrashed() {
  int i;

  for(i = 1; i < currentlen; i++) {
    if(snakepos[0][0] == snakepos[i][0] && snakepos[0][1] == snakepos[i][1])
      crashed = 1;
  }

  glutIdleFunc(crashed ? NULL : idle);
}

void drawGame() {
  gameBoard();
  drawSnake();
  drawFood();
  isCrashed();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glLoadIdentity();
  if (first_person) {
    double Ex = -2 * dim * Sin(th) * Cos(ph);
    double Ey = +2 * dim           *Sin(ph);
    double Ez = +2 * dim * Cos(th) * Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
  } else {
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
  }

  glShadeModel(GL_SMOOTH);

  float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
  float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
  float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
  float Position[]  = {5 * Cos(zh), ylight, 5 * Sin(zh), 1.0};
  glColor3f(1,1,1);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,0);
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
  glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
  glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
  glLightfv(GL_LIGHT0,GL_POSITION,Position);

  drawGame();

  ErrCheck("display");
  glFlush();
  glutSwapBuffers();
}



void special(int key, int x, int y) {
  if (key == GLUT_KEY_RIGHT)
    currentdir = Right;
  else if (key == GLUT_KEY_LEFT)
    currentdir = Left;
  else if (key == GLUT_KEY_UP)
    currentdir = Up;
  else if (key == GLUT_KEY_DOWN)
    currentdir = Down;

  th %= 360;
  ph %= 360;
   
  Project(first_person ? fov : 0, asp, dim);
  glutPostRedisplay();
}

void key(unsigned char ch, int x, int y) {
  if (ch == 27)
    exit(0);
  else if (ch == '0') {
    th = 0;
    ph = 90;
  }
  Project(first_person ? fov : 0,asp,dim);
  glutIdleFunc(idle);
  glutPostRedisplay();
}

void reshape(int width, int height) {
  asp = (height>0) ? (double)width/height : 1;
  glViewport(0,0, width,height);
  Project(first_person ? fov : 0,asp,dim);
}

int main(int argc, char* argv[]) {
  glutInit(&argc,argv);
  initSnake();
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(400,400);
  glutCreateWindow("Lighting");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);
  ErrCheck("init");
  glutMainLoop();
  return 0;
}
