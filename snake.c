#include "CSCIx229.h"

int first_person=1;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=10;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world

// Light values
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

static void Vertex(double th, double ph) {
  double x = Sin(th) * Cos(ph);
  double y = Cos(th) * Cos(ph);
  double z =           Sin(ph);
  glNormal3d(x, y, z);
  glVertex3d(x, y, z);
}

static void sphere(double x, double y, double z, double r) {
  const int d = 5;
  int th, ph;

  glPushMatrix();
  glTranslated(x, y, z);
  glScaled(r, r, r);

  glColor3ub(0, 100, 0);
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

  ErrCheck("display");
  glFlush();
  glutSwapBuffers();
}

void idle() {
  double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
  zh = fmod(90 * t,360.0);
  glutPostRedisplay();
}

void special(int key, int x, int y) {
  if (key == GLUT_KEY_RIGHT)
    th -= 5;
  else if (key == GLUT_KEY_LEFT)
    th += 5;
  else if (key == GLUT_KEY_UP)
    ph += 5;
  else if (key == GLUT_KEY_DOWN)
    ph -= 5;

  th %= 360;
  ph %= 360;
   
  Project(first_person ? fov : 0, asp, dim);
  glutPostRedisplay();
}

void key(unsigned char ch, int x, int y) {
  if (ch == 27)
    exit(0);
  else if (ch == '0')
    th = ph = 0;
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
