/* John Pease, Carnival.c */

//#include <GL/glut.h>
//#include <GL>
#include <GLUT/GLUT.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265

/* prototypes */
void writemessage();
void draw_base(int iteration);
void draw_ride(GLdouble height);
void draw_box(GLdouble height, int iteration);
void draw_fence(char side);
void display(void);
void spin(void);
void keyboard(unsigned char key, int x, int y);
void reshape(int w, int h);

/* camera location in longitude and latitude */
static float alpha = 0.0;
static float beta = PI/6.0;

/* position of camera, (0,0,0) */
static GLdouble cpos[3];

/* position of light source */
static GLfloat
lpos1[] = {-2.5, 2.5, 3.0, 1.0};

/* colors/materials */
static GLfloat
silver[]	= {0.9, 0.9, 0.98},
black[]		= {0.0, 0.0, 0.0, 1.0},
red[]		= {1.0, 0.0, 0.0, 1.0},
green[]		= {0.0, 1.0, 0.0, 1.0},
blue[]		= {0.0, 0.0, 1.0, 1.0},
yellow[]	= {1.0, 1.0, 0.0, 1.0},
magenta[]	= {1.0, 0.0, 1.0, 1.0};

/* placeholders for manipulating zoom, camera distance, toggle light, window size */
static GLdouble fov = 45.0;
static GLdouble cameraDistance = 20.0;
static int lightCheck1 = 1;
static GLfloat emission1[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat windowX;
static GLfloat windowY;
static int numBases = 8;
static GLfloat spinAngle = 0.0;
static GLfloat spinStep = 0.0000001;
static GLint window[2];
static int messageCount = 0;

/* global properties of the triangle base */
static GLfloat baseLength = 4;
static GLfloat baseHeight = 2;
static GLfloat baseAngle;
static GLfloat newHeight;
static GLfloat tiltAngle;

/* global properties of ground */
static GLfloat groundWidth = 10;
static GLfloat groundLength = 12;

/* ride base points */
static GLfloat center[] = {0.0, 1.0, 0.0};

void writemessage() {
    printf("\n\
           Homework 3, John Pease jjpease@uwm.edu\n\
           **********************************************************************\n\
           **********************************************************************\n\
           arrow keys -------------->> change camera position\n\
           xyzXYZ ------------------>> change the location of the light\n\
           left click, right click ->> starts/stops ride\n\
           w ----------------------->> toggle between fill and wire-frame\n\
           1 ----------------------->> toggle the light being on/off\n\
           <, > -------------------->> moves the camera towards, away from ride\n\
           -, + -------------------->> zooms in and out by changing the fov\n\
           ESC --------------------->> exit\n\
           EXTRAS ------------------>>\n\
           [, ] -------------------->> [ ] increases/decreases number of rides\n\
           \n");
           }
           
void init(void) {
   
    ++messageCount;
    if (messageCount == 1) writemessage();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    /* initially GL_FILL mode (default), later GL_LINE to show wireframe */
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHT0);
}

void reshape(int w, int h) {
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   
   /* store the window size so reshape can be called in */
   windowX = w;
   windowY = h;
   
   /* change the frustum based on the difference between the current cameraDistance and its original value */
   gluPerspective(fov, (GLfloat) windowX / (GLfloat) windowY, 1.0, 20.0 + (cameraDistance));
   
   glMatrixMode(GL_MODELVIEW);
}

void draw_base(int iteration) {
   
   static GLfloat z;
   
   /* calculate angle for each triangle base */
   baseAngle = ((360.0/numBases))/180 * PI;
   
   /* calculate values for the first triangle base */
   center[1] = baseHeight;
   z = baseLength * tan(baseAngle/2);
   
   /* common material settings */
   glMaterialfv(GL_FRONT, GL_EMISSION, black);
   glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, black);
   glMaterialfv(GL_BACK, GL_SPECULAR, black);
   
   /* 1st triangle base */
   if (iteration % 2 == 0) {
       glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
   } else {
       glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
   }
   glBegin(GL_TRIANGLES);
   glNormal3f(0.0, cos(baseAngle), -sin(baseAngle));
   glVertex3fv(center);
   glVertex3f(baseLength, 0, z);
   glVertex3f(baseLength, 0, z * -1);
   glEnd();
   
   /* draw the ride in every other base */
   if (iteration % 2 == 0) {
       newHeight = baseHeight + 1;
       newHeight = (baseHeight+1) + sin((spinAngle/10)+iteration);
       tiltAngle = (atan(baseHeight/baseLength))*(180/PI);
       
       glPushMatrix();
       glRotatef(tiltAngle, 0, 0, -1);
       draw_ride(newHeight);
       draw_box(newHeight, iteration);
       glPopMatrix();
   }
}

void draw_ride(GLdouble height) {
   static GLdouble radius;
   GLUquadricObj *quadratic;
   quadratic = gluNewQuadric();
   
   radius = (baseLength/2 * tan(baseAngle/2))/4;
   
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
   
   glPushMatrix();
   glRotatef(90.0f, -1.0f, 0.0f, 0.0f);  //not sure, but necessary
   glTranslatef(baseLength/2, 0.0, 0.0); //moves the cylinder over to the center of the triangle
   gluCylinder(quadratic, radius, radius, height, 32, 32);
   glPopMatrix();
}

void draw_box(GLdouble height, int iteration) {
   static GLfloat
   frontBotLeft[] = {0, 0, 0},
   frontBotRight[] = {0, 0, 0},
   frontTopRight[] = {0, 0, 0},
   frontTopLeft[] = {0, 0, 0},
   backBotLeft[] = {0, 0, 0},
   backBotRight[] = {0, 0, 0},
   backTopRight[] = {0, 0, 0},
   backTopLeft[] = {0, 0, 0},
   cutoutTopRight[] = {0, 0, 0},
   cutoutBotRight[] = {0, 0, 0},
   cutoutBotLeft[] = {0, 0, 0},
   cutoutTopLeft[] = {0, 0, 0};
   
   GLdouble boxWidth, boxLength, boxHeight, cutoutHeight, cutoutZ;
   
   boxWidth		= baseLength/2*tan(baseAngle/2)/1.5;
   boxLength		= boxHeight = boxWidth;
   cutoutHeight	= boxHeight/2;
   cutoutZ			= boxLength/2;
   
   /* x values */
   frontBotLeft[0]	= frontTopLeft[0] = backBotRight[0] = backTopRight[0] = cutoutTopRight[0] = cutoutBotRight[0] = cutoutBotLeft[0] = cutoutTopLeft[0] = 0 - boxWidth;
   frontBotRight[0] = frontTopRight[0] = backBotLeft[0] = backTopLeft[0] = 0 + boxWidth;
   
   /* y values */
   frontBotLeft[1] = frontBotRight[1] = backBotLeft[1] = backBotRight[1] = height;
   frontTopLeft[1] = frontTopRight[1] = backTopLeft[1] = backTopRight[1] = cutoutTopLeft[1] = cutoutTopRight[1] = height + boxHeight;
   cutoutBotLeft[1] = cutoutBotRight[1] = height + cutoutHeight;
   
   /* z values */
   frontBotLeft[2] = frontBotRight[2] = frontTopLeft[2] = frontTopRight[2] = boxLength;
   backBotLeft[2] = backBotRight[2] = backTopLeft[2] = backTopRight[2] = -1 * boxLength;
   cutoutBotRight[2] = cutoutTopRight[2] = cutoutZ;
   cutoutBotLeft[2] = cutoutTopLeft[2] = 0 - cutoutZ;
   
   glPushMatrix();
   glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, black);
   if (iteration != 0) {
       glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, silver);
   } else {
       glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
   }
   
   glTranslatef(baseLength/2, 0.0, 0.0);	//push the box into the middle of the base triangle
   glRotatef(spinAngle*6, 0.0, -2.0, 0.0); //rotate the box when the base is moving
   
   /* front side facing the camera */
   glBegin(GL_QUADS);
   glNormal3f(0.0, height + boxHeight/2, 1.0);
   glVertex3fv(frontTopLeft);
   glVertex3fv(frontBotLeft);
   glVertex3fv(frontBotRight);
   glVertex3fv(frontTopRight);
   glEnd();
   
   /* right side from the camera */
   glBegin(GL_QUADS);
   glNormal3f(1.0, height + boxHeight/2, 0.0);
   glVertex3fv(frontBotRight);
   glVertex3fv(backBotLeft);
   glVertex3fv(backTopLeft);
   glVertex3fv(frontTopRight);
   glEnd();
   
   /* back side opposite from the camera */
   glBegin(GL_QUADS);
   glNormal3f(0.0, height + boxHeight/2, -1.0);
   glVertex3fv(backBotLeft);
   glVertex3fv(backBotRight);
   glVertex3fv(backTopRight);
   glVertex3fv(backTopLeft);
   glEnd();
   
   /* bottom part of the cutout */
   glBegin(GL_QUADS);
   glNormal3f(-1.0, height + boxHeight/4, 0.0);
   glVertex3fv(backBotRight);
   glVertex3fv(frontBotLeft);
   glVertex3f(frontBotLeft[0], height + cutoutHeight, frontBotLeft[2]);
   glVertex3f(backBotRight[0], height + cutoutHeight, backBotRight[2]);
   glEnd();
   
   /* left part of the cutout */
   glBegin(GL_QUADS);
   glNormal3f(-1.0, height + boxHeight/2, -cutoutZ/2);
   glVertex3f(backBotRight[0], height + cutoutHeight, backBotRight[2]);
   glVertex3fv(cutoutBotLeft);
   glVertex3fv(cutoutTopLeft);
   glVertex3fv(backTopRight);
   glEnd();
   
   /* right part of the cutout */
   glBegin(GL_QUADS);
   glNormal3f(-1.0, height + boxHeight/2, cutoutZ/2);
   glVertex3f(frontBotLeft[0], height + cutoutHeight, frontBotLeft[2]);
   glVertex3fv(frontTopLeft);
   glVertex3fv(cutoutTopRight);
   glVertex3fv(cutoutBotRight);
   glEnd();
   
   /* bottom of the box */
   glBegin(GL_QUADS);
   glNormal3f(0.0, -height, 0.0);
   glVertex3fv(frontBotLeft);
   glVertex3fv(frontBotRight);
   glVertex3fv(backBotLeft);
   glVertex3fv(backBotRight);
   glEnd();
   
   glPopMatrix();
}

void draw_fence(char side) {
   int i, end;
   GLdouble offset;
   GLUquadricObj *quadratic;
   
   switch(side) {
       case 'w':
           i = groundWidth/2;
           offset = groundLength/2;
           break;
       case 'l':
           i = groundLength/2;
           offset = groundWidth/2;
           break;
       default:
           return;
           break;
   }
   
   quadratic = gluNewQuadric();
   end = i * -1;
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, magenta);
   glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, magenta);
   for (i; i >= end; --i) {
       /* draw positive side of fence */
       glPushMatrix();
       glRotatef(90.0f, -1.0f, 0.0f, 0.0f); //not sure, but required
       if (side == 'w') {
           glTranslatef(i, 0.0, 0.0);
           glTranslatef(0.0, offset, 0.0);
       } else if (side == 'l') {
           glTranslatef(offset, 0.0, 0.0);
           glTranslatef(0.0, i, 0.0);
       }
       gluCylinder(quadratic, 0.1, 0.1, 0.5, 32, 32);
       glPopMatrix();
       
       /* draw negative side of fence */
       glPushMatrix();
       glRotatef(90, -1.0f, 0.0f, 0.0f); //not sure, but required
       if (side == 'w') {
           glTranslatef(i, 0.0, 0.0);
           glTranslatef(0.0, offset * -1, 0.0);
       } else if (side == 'l') {
           glTranslatef(offset * -1, 0.0, 0.0);
           glTranslatef(0.0, i, 0.0);
       }
       gluCylinder(quadratic, 0.1, 0.1, 0.5, 32, 32);
       glPopMatrix();
   }
   
   /* draw the pole connecting each set of fences */
   glPushMatrix();
   glRotatef(0.0f, -1.0f, 0.0f, 0.0f);  //not sure, but necessary
   glTranslatef(groundWidth/2, 0.0, 0.0);
   glTranslatef(0.0, 0.0, -1 * groundLength/2);
   glTranslatef(0.0, 0.5, 0.0);
   gluCylinder(quadratic, 0.1, 0.1, groundLength, 32, 32);
   glPopMatrix();
   
   glPushMatrix();
   glRotatef(0.0f, -1.0f, 0.0f, 0.0f);  //not sure, but necessary
   glTranslatef(groundWidth/2 * -1, 0.0, 0.0);
   glTranslatef(0.0, 0.0, -1 * groundLength/2);
   glTranslatef(0.0, 0.5, 0.0);
   gluCylinder(quadratic, 0.1, 0.1, groundLength, 32, 32);
   glPopMatrix();
   
   glPushMatrix();
   glRotatef(0.0f, -1.0f, 0.0f, 0.0f);  //not sure, but necessary
   glTranslatef(0.0, 0.0, groundLength/2 * -1);
   glTranslatef(-1 * groundWidth/2, 0.0, 0.0);
   glTranslatef(0.0, 0.5, 0.0);
   glRotatef(90.0, 0.0, 1.0, 0.0);
   gluCylinder(quadratic, 0.1, 0.1, groundWidth, 32, 32);
   glPopMatrix();
   
   glPushMatrix();
   glRotatef(0.0f, -1.0f, 0.0f, 0.0f);  //not sure, but necessary
   glTranslatef(0.0, 0.0, groundLength/2 * 1);
   glTranslatef(-1 * groundWidth/2, 0.0, 0.0);
   glTranslatef(0.0, 0.5, 0.0);
   glRotatef(90.0, 0.0, 1.0, 0.0);
   gluCylinder(quadratic, 0.1, 0.1, groundWidth, 32, 32);
   glPopMatrix();
   
}

void display(void) {
   static int i, j;
   static double xOffset;
   
   newHeight = baseHeight + 1;
   tiltAngle = (atan(baseHeight/baseLength))*(180/PI);
   
   
   for (i = 0; i < 2; i++) {
       
       glutSetWindow(window[i]);
       
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
       /* handle the zooming by recalling the reshape with the new fov and the same window sizes */
       reshape(windowX, windowY);
       
       /* initialize (modelview) matrix */
       glLoadIdentity();
       
       if (i == 0) {
           /* update camera position */
           cpos[0] = cameraDistance * cos(beta) * sin(alpha);
           cpos[1] = cameraDistance * sin(beta);
           cpos[2] = cameraDistance * cos(beta) * cos(alpha);
           gluLookAt(cpos[0],cpos[1],cpos[2], 0.0,1.0,0.0, 0.0,1.0,0.0);
       } else if (i == 1) {
           xOffset = ((newHeight+0.5) * cos(tiltAngle));
           gluLookAt(0.0, newHeight, 0.0, -baseLength/2, newHeight, 0.0, 0.0, 1.0, 0.0);
           //glRotatef(tiltAngle, 0, 0, -1); //tilt the view to account for the tilt on the ride
           glRotatef(-spinAngle*6, 0.0, -2.0, 0.0); //rotate it like in the ride
           glTranslatef(-baseLength/2 - xOffset, 0.0, 0.0);
           glRotatef(-spinAngle, 0.0, 1.0, 0.0); //rotate it around the base
       }
       
       /* light source 1 location */
       glLightfv(GL_LIGHT0, GL_POSITION, lpos1);
       glPushMatrix();
       glTranslatef(lpos1[0],lpos1[1],lpos1[2]);
       glutSolidSphere(0.1, 10, 8);
       glPopMatrix();
       
       /* draw surface */
       glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
       glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, green);
       glBegin(GL_QUADS);
       glNormal3f(0, 1, 0);
       glVertex3f(0 - groundWidth/2, 0.0, groundLength/2);
       glVertex3f(groundWidth/2, 0.0, groundLength/2);
       glVertex3f(groundWidth/2, 0.0, 0 - groundLength/2);
       glVertex3f(0 - groundWidth/2, 0.0, 0 - groundLength/2);
       glEnd();
       
       /* draw the fences */
       draw_fence('w');
       draw_fence('l');
       
       /* draw bases/rides */
       for (j = 0; j < numBases; ++j) {
           glPushMatrix();
           glRotatef(j * 360.0/numBases, 0.0, 1.0, 0.0);
           glRotatef(spinAngle, 0.0, 1.0, 0.0);
           draw_base(j);
           glPopMatrix();
       }
       
       glFlush();
       glutSwapBuffers();
   }
}

void spin(void) {
   if (spinStep < 1) spinStep = spinStep + .01;
   spinAngle = spinAngle + spinStep;
   if (spinAngle > (260.0 * 240)) spinAngle = spinAngle - (260.0 * 240);
   glutPostRedisplay();
}

void deSpin(void) {
   if (spinStep > 0) { 
       spinStep = spinStep - .01;
       spinAngle = spinAngle + spinStep;
   }
   glutPostRedisplay();
}

/* handle everything except for the arrow keys */
void keyboard(unsigned char key, int x, int y) {
   static int polygonmode[2]; 
   
   switch (key) {
       case 27:
           exit(0);
           break; 
           
       case 'w':
           glGetIntegerv(GL_POLYGON_MODE,polygonmode);
           if (polygonmode[0]==GL_FILL) 
               glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
           else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
           glutPostRedisplay();
           break;
       case 'x':
           lpos1[0] += 0.2;
           glutPostRedisplay();
           break;
       case 'X':
           lpos1[0] -= 0.2;
           glutPostRedisplay();
           break;
       case 'y':                
           lpos1[1] += 0.2;
           glutPostRedisplay();
           break;
       case 'Y':                 
           lpos1[1] -= 0.2;
           glutPostRedisplay();
           break;
       case 'z':
           lpos1[2] += 0.2;
           glutPostRedisplay();
           break;
       case 'Z':
           lpos1[2] -= 0.2;
           glutPostRedisplay();
           break;
       case '<':
           if (cameraDistance > 2.0) cameraDistance = cameraDistance - 1.0;
           glutPostRedisplay();
           break;
       case '>':
           cameraDistance = cameraDistance + 1.0;
           glutPostRedisplay();
           break;
       case '1':
           ++lightCheck1;
           if (lightCheck1 % 2 == 0) {
               glDisable(GL_LIGHT0);
               emission1[0] = emission1[1] = emission1[2] = emission1[3] = 0.0;
           } else {
               glEnable(GL_LIGHT0);
               emission1[0] = emission1[1] = emission1[2] = emission1[3] = 1.0;
           }
           glutPostRedisplay();
           break;
       case '+':
           --fov;
           glutPostRedisplay();
           break;
       case '-':
           ++fov;
           glutPostRedisplay();
           break;
       case '[':
           if (numBases > 6) numBases-=2;
           glutPostRedisplay();
           break;
       case ']':
           numBases+=2;
           glutPostRedisplay();
           break;
       default:
           break;
   }
}

void mouse(int button, int state, int x, int y) {
   switch (button) {
       case GLUT_LEFT_BUTTON:
           if (state == GLUT_DOWN) {
               glutIdleFunc(spin);
               break;
           }
       case GLUT_RIGHT_BUTTON:
           if (state == GLUT_DOWN) {
               if (spinStep > 0) glutIdleFunc(deSpin);
               break;
           }
       default:
           break;
   }
}

void specialkey(GLint key, int x, int y) {
   switch (key) {
       case GLUT_KEY_RIGHT:                
           alpha = alpha + PI/180;
           if (alpha > 2*PI) alpha = alpha - 2*PI;
           glutPostRedisplay();
           break;
       case GLUT_KEY_LEFT:                 
           alpha = alpha - PI/180;
           if (alpha < 0) alpha = alpha + 2*PI;
           glutPostRedisplay();
           break;
       case GLUT_KEY_UP:                
           if (beta < 0.45*PI) beta = beta + PI/180;
           glutPostRedisplay();
           break;
       case GLUT_KEY_DOWN:                 
           if (beta > 0.05*PI) beta = beta - PI/180;
           glutPostRedisplay();
           break;
           
       default:
           break;  
   }
}

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(800, 800); 
   glutInitWindowPosition(0, 0);
   window[0] = glutCreateWindow("Overview");
   init();
   
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutSpecialFunc(specialkey);
   glutInitWindowSize(800, 800);
   glutInitWindowPosition(800, 0);
   window[1] = glutCreateWindow("Rider's view");
   init();
   
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutSpecialFunc(specialkey);
   
   glutMainLoop();
   return 0;
}
           
