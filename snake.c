/*
 * CSCI 4229 Final Project: Game of Snake 
 * Domenic Murtari (domenic.murtari@gmail.com)
 * Undergraduate
 *
 * Currently, functional, if basic, game of snake. All elements and function
 * calls are in place to allow for more complex graphics to be implemented by
 * the due date of the project.
 */

#include "CSCIx229.h"

// Direction definitions
#define Up 0
#define Right 1
#define Down 2
#define Left 3
#define NA -3000

// Size of gameboard
#define Size 20

// View Globals
int first_person = 0;
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
int snakepos[Size * Size][2];
int speed = 100;
int currentdir = Left;
int currentlen = 0;
int foodx = NA;
int foody = NA;
int crashed = 0;
int clock = 0;
int score = 0;
int highscore = 0;

/*
 * Given position of vertex in polar coordinates, calculate and draw a vertex
 * with normal
 */
void Vertex(double th, double ph) {
  double x = Sin(th) * Cos(ph);
  double y = Cos(th) * Cos(ph);
  double z =           Sin(ph);
  glNormal3d(x, y, z);
  glVertex3d(x, y, z);
}

/*
 * Draw a sphere at position (x, y, z) and radius r
 */
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

/*
 * Draw a cube at position (x, y, z), transformed by (dx, dy, dz), and rotated
 * around the z-axis th degrees
 */
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

/*
 * Draw the snake game board
 */
void gameBoard() {
  float white[] = {1,1,1,1};
  float black[] = {0,0,0,1};
  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

  glPushMatrix();
  
  glBegin(GL_QUADS);
  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(-100, -1.0, 100);
  glVertex3f(-100, -1.0, -100);
  glVertex3f(100, -1.0, -100);
  glVertex3f(100, -1.0, 100);
  glEnd();

  cube(Size, 0.0, 0.0,  1.0, 1.0, Size,  0);
  cube(-Size, 0.0, 0.0,  1.0, 1.0, Size,  0);
  cube(0.0, 0.0, Size,  Size, 1.0, 1.0, 0);
  cube(0.0, 0.0, -Size,  Size, 1.0, 1.0, 0);

  glPopMatrix();

}

/*
 * Initialize the position array of the snake as all positions where current
 * position is NA. Then, make a snake 3 segments long, and put the food where
 * the snake's tail is. 
 */
void initSnake() {
  int i;

  for(i = 0; i < Size * Size; i++) {
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

/*
 * Draw the head of the snake (should contain more ability for the play to 
 * customize the head, based on what settings are chosen in a menu)
 */
void drawHead() {
  glColor3ub(0, 0, 200);
  sphere(snakepos[0][0], 0, snakepos[0][1], .4);
}

/*
 * Draw a body segment. Same as head, should contain more ability to customize
 */
void drawBody(int i) {
  glColor3ub(0, 200, 0);
  sphere(snakepos[i][0], 0, snakepos[i][1], .4);
}

/*
 * Draw a piece of food based on current food position. Should draw something
 * a little more interesting.
 */
void drawFood() {
  glColor3ub(200, 0, 0);
  sphere(foodx, 0, foody, .3);
}

/*
 * Draw the snake given current position array
 */
void drawSnake() {
  int i;

  drawHead();
  for(i = 0; i < 100; i++) {
    if(snakepos[i][0] != NA)
      drawBody(i);
  }
}

/*
 * Each step while the program is idling, modify the snake's position array
 * based on the direction that the snake is going, and whether the snake ate
 * a piece of food
 */
void step(int dir) {
  int i;

  if(snakepos[0][0] == foodx && snakepos[0][1] == foody) {
    // Snake has 'eaten' if the head is on a piece of food, so the snake grows
    for(i = currentlen; i > 0; i--) {
      snakepos[i][0] = snakepos[i - 1][0];
      snakepos[i][1] = snakepos[i - 1][1];
    }       
    currentlen++;

    // Define new position of food to be where last body segment of snake is
    foodx = snakepos[currentlen - 1][0];
    foody = snakepos[currentlen - 1][1];
    score += 100;
  } else {
    // Snake hasn't eaten, so each body segment just moves forward without growing
    for(i = currentlen - 1; i > 0; i--) {
      snakepos[i][0] = snakepos[i - 1][0];
      snakepos[i][1] = snakepos[i - 1][1];
    }
    score += 10;
  }

  // Based on the current direction, decide where the head of the snake should
  // go
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

  if (score > highscore)
    highscore = score;
}

/*
 * Moves the light during idle, and also calls the step function to move the 
 * snake. Needs some way to better control the speed of the snake (ie only call
 * the step function if certain conditions are met)
 */
void idle() {
  int elapsed = glutGet(GLUT_ELAPSED_TIME)/speed;
  double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
  zh = fmod(90 * t,360.0);
  if(elapsed > clock) {
    step(currentdir);
    clock = elapsed;
  }
  glutPostRedisplay();
}

/*
 * Decides whether or not the snake has crashed
 */
void isCrashed() {
  int i;

  for(i = 1; i < currentlen; i++) {
    // If the head is at the same position as any body segment, then crashed
    if(snakepos[0][0] == snakepos[i][0] && snakepos[0][1] == snakepos[i][1])
      crashed = 1;
  }

  // If the head is in a wall, then crashed
  if((snakepos[0][0] == Size - 1) || (snakepos[0][1] == Size - 1) ||
     (snakepos[0][0] == -Size + 1) || (snakepos[0][1] == -Size + 1))
    crashed = 1;

  // Stop animation if the snake crashed, otherwise keep going. 
  glutIdleFunc(crashed ? NULL : idle);
}

void printMessage() {
  glWindowPos2i(5, 5);
  Print("Score: %d, High Score: %d", score, highscore);

  if (crashed){
    glWindowPos2i(200, 400);
    Print("CRASHED! Hit 'r' to restart");
    glWindowPos2i(220, 300);
    Print("Your score was: %d", score);
  }
}
/*
 * Draw the game
  */
void drawGame() {
  gameBoard();
  isCrashed();
  drawSnake();
  drawFood();
  printMessage();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glLoadIdentity();
  if (first_person) {
    double Ex = snakepos[0][0];
    double Ey = 0;
    double Ez = snakepos[0][1];
    double Ax = 0;
    double Ay = 0;
    double Az = 0;

    switch (currentdir) {
      case Up:
        Ax = snakepos[0][0];
        Az = snakepos[0][1] - 1;
        break;
      case Right:
        Ax = snakepos[0][0] + 1;
        Az = snakepos[0][1];
        break;
      case Down:
        Ax = snakepos[0][0];
        Az = snakepos[0][1] + 1;
        break;
      case Left:
        Ax = snakepos[0][0] - 1;
        Az = snakepos[0][1];
        break;
    }
    gluLookAt(Ex, Ey, Ez, Ax, Ay, Az, 0, 1, 0);
  } else {
    double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
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
  

  glFlush();
  glutSwapBuffers();
}

void special(int key, int x, int y) {
  if (key == GLUT_KEY_RIGHT) {
    if (first_person) {
      currentdir = (currentdir + 1) % 4;
    } else {
      currentdir = Right;
    }
  } else if (key == GLUT_KEY_LEFT) {
    if (first_person) {
      currentdir = (currentdir - 1) % 4;
    } else {
      currentdir = Left;
    }
  } else if (key == GLUT_KEY_UP){
    if (!first_person)
      currentdir = Up;
  } else if (key == GLUT_KEY_DOWN){
    if (!first_person)
      currentdir = Down;
  }

  th %= 360;
  ph %= 360;
   
  Project(fov, asp, dim);
  glutPostRedisplay();
}

void key(unsigned char ch, int x, int y) {
  if (ch == 27)
    exit(0);
  else if (ch == '0') {
    th = 0;
    ph = 90;
  } else if (ch == 'r') {
    initSnake();
    crashed = 0;
    score = 0;
    currentdir = Left;
    glutIdleFunc(idle);
  } else if (ch == 'f') {
    first_person = 1 - first_person;
  }

  Project(fov ,asp, dim);
  glutIdleFunc(idle);
  glutPostRedisplay();
}

void reshape(int width, int height) {
  asp = (height>0) ? (double)width/height : 1;
  glViewport(0,0, width,height);
  Project(fov,asp,dim);
}

int main(int argc, char* argv[]) {
  glutInit(&argc,argv);
  initSnake();
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(600,600);
  glutCreateWindow("Snake");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);
  ErrCheck("init");
  glutMainLoop();
  return 0;
}
