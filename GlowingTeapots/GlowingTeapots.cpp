//////////////////////////////////////////////////////////////////////////////////////
//
// Shahzore Qureshi
// November 19th, 2013
// CSC350 - Graphics
// Final Project
//
// GlowingTeapots.cpp
//
// This program draws teapots in a 3D world that the
// user can explore. Teapots reflect light correctly,
// and the user can move around. When the user is near
// a teapot, the teapot becomes brighter.
//
//////////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define PI 3.14159265
#define TAU 6.2831853

#define X 0
#define Y 1
#define Z 2
#define DIR_FORWARD  0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT     0x04
#define DIR_RIGHT    0x08

using namespace std;

// Globals.
static float spotAngle = 40.0; // Spotlight cone half-angle.
static float xMove = 0.0, zMove = 9.0; // Movement components.
static float spotExponent = 2.0; // Spotlight exponent = attenuation.
static float colorsForTeapots[27]; // Random colors for teapots.

static short player_dir = 0; // start off with no direction
static float player_pos[] = {0, 0, 0}; // at origin
static float player_rot[] = {0, 0, 0}; // without having rotated

static int isThrowTeapotEnabled = 0; //boolean that determines whether to throw a teapot
static int animationPeriod = 100; // Time interval between frames.

static float thrownTeapotX = 0.0; //X coordinate of thrown teapot.
static float thrownTeapotY = 1.0; //Y coordinate of thrown teapot.
static float thrownTeapotZ = 0.0; //Z coordinate of thrown teapot.
static float timeForThrow = 0; //Time used to calculate teapot throw.
static float horizontalVelocity = 1.8; // Horizontal component of initial velocity.
static float verticalVelocity = 1.0; // Vertical component of initial velocity.
static float gravityAcceleration = 0.2;  // Gravitational accelaration.
static int isBreakTeapotEnabled = 0; //boolean that determines whether to break a teapot

// Initialization routine.
void setup(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST); // Enable depth testing.
    
    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);
    
    // Light property vectors.
    float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float globAmb[] = { 0.69, 0.69, 0.69, 1.0 };    ///adsadsadsadsadsad CHANGE BACK !!!!!!
    
    // Light properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
    
    glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.
    
    // Material property vectors.
    float matSpec[] = { 0.5, 0.5, 0.5, 1.0 };
    float matShine[] = { 90.0 };
    
    // Material properties shared by all the spheres.
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    
    // Cull back faces.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Enable color material mode:
    // The ambient and diffuse color of the front faces will track the color set by glColor().
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Define random colors for teapots. Only do this once.
    srand (static_cast <unsigned> (time(0)));
    for (int count = 0; count < 27; count++)
        colorsForTeapots[count] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 0.1;
}

// Timer function.
void animate(int value)
{
    if (isThrowTeapotEnabled)
    {
        timeForThrow += 0.6;
        cout << "time: " << timeForThrow << endl;
        
    }
    else if (timeForThrow != 0)
        timeForThrow = 0;
    
    glutTimerFunc(animationPeriod, animate, 1);
    glutPostRedisplay();
}

void drawTeapots()
{
    int distance = 5;
    int count = 0;
    for (int z = distance; z >= -distance; z = z - distance)
        for (int x = -distance; x <= distance; x = x + distance)
        {
            glPushMatrix();
            glColor4f(colorsForTeapots[count], colorsForTeapots[count + 1], colorsForTeapots[count + 2], 1.0);
            
            if (isThrowTeapotEnabled && x == 0 && z == distance)
            {
                
                if (thrownTeapotY < 0.5)
                {
                    //Break teapot.
                    //This is temp. Change it.
                    isThrowTeapotEnabled = 0;
                }
                else
                {
                    //Apply throwing equations of motion to transform sphere.
                    thrownTeapotY = 0.5 + verticalVelocity * timeForThrow - (gravityAcceleration/2.0) * timeForThrow * timeForThrow;
                    thrownTeapotZ = z - horizontalVelocity * timeForThrow;
                    glTranslatef(x, thrownTeapotY, thrownTeapotZ);
                    glutSolidTeapot(0.5);
                }
            }
            else
            {
                glTranslatef(x, 0.5, z); //Draw teapot at regular location.
                glutSolidTeapot(0.5);
            }
            
            glPopMatrix();
            count = count + 2;
        }
}

void drawReflectedTeapots()
{
    int distance = 5;
    int count = 0;
    for (int z = distance; z >= -distance; z = z - distance)
        for (int x = -distance; x <= distance; x = x + distance)
        {
            glPushMatrix();
            
            //Draw reflection on ground.
            glScalef(1.0, -0.4, 1.0);
            glFrontFace(GL_CW); // Because of reflection front-faces are drawn clockwise.
            glPushMatrix();
            
            if (isThrowTeapotEnabled && x == 0 && z == distance)
            {
                // Apply equations of motion to transform sphere.
                float y = 0.5 + verticalVelocity * timeForThrow - (gravityAcceleration/2.0) * timeForThrow * timeForThrow;
                // Apply equations of motion to transform sphere.
                glTranslatef(x, y, z - horizontalVelocity * timeForThrow);
                
                if (y < 0.5)
                    isThrowTeapotEnabled = 0;
            }
            else
                glTranslatef(x, 0.5, z);
            
            glColor4f(colorsForTeapots[count], colorsForTeapots[count + 1], colorsForTeapots[count + 2], 1.0);
            glutSolidTeapot(0.5);
            glPopMatrix();
            glFrontFace(GL_CCW);
            
            //Draw reflection on back wall.
//            glPushMatrix();
//            glTranslatef(x, 0.5, z - distance * 3);
//            glColor4f(colorsForTeapots[count], colorsForTeapots[count + 1], colorsForTeapots[count + 2], 1.0);
//            glutSolidTeapot(0.5);
//            glPopMatrix();
            
            
            glPopMatrix();
            count = count + 2;
        }
}

// Drawing routine.
void drawScene()
{
    //Determine player velocity and direction.
    float player_vel[] = {0,0,0}; // the velocity this time around
    static float player_speed = 0; // velocity magnitude
    static float player_last[] = {0,0,0}; // the previous velocity
    
    char moved = 0; // did the player try to move?
    if (player_dir & DIR_FORWARD) {
        if (player_pos[Z] < 5.0f)
        {
            //Use unit circle and direction to determine velocity.
            player_vel[X] += sin(player_rot[Y]);
            player_vel[Z] += cos(player_rot[Y]);
            moved = 1;
        }
    }
    if (player_dir & DIR_BACKWARD) {
        if (player_pos[Z] > -5.0f)
        {
            player_vel[X] += -sin(player_rot[Y]);
            player_vel[Z] += -cos(player_rot[Y]);
            moved = 1;
        }
    }
    if (player_dir & DIR_RIGHT) {
        if (player_pos[X] > -7.0f)
        {
            player_vel[X] += -cos(player_rot[Y]);
            player_vel[Z] += sin(player_rot[Y]);
            moved = 1;
        }
    }
    if (player_dir & DIR_LEFT) {
        if (player_pos[X] < 7.0f)
        {
            player_vel[X] += cos(player_rot[Y]);
            player_vel[Z] += -sin(player_rot[Y]);
            moved = 1;
        }
    }
    
    //If there is velocity, increase until maximum velocity is reached.
    //Otherwise, decrease velocity when user is not moving anymore.
    if (moved) {
        player_speed += 0.1;
    } else {
        player_speed -= 0.1;
        player_vel[X] = player_last[X];
        player_vel[Y] = player_last[Y];
        player_vel[Z] = player_last[Z];
        
        
    }
    if (player_speed > 1)
        player_speed = 1;
    if (player_speed < 0)
        player_speed = 0;
    
    
    //Normalize velocity vector.
    float magnitude = sqrt(pow(player_vel[X], 2) + pow(player_vel[Y], 2) + pow(player_vel[Z], 2));
    
    if (magnitude != 0)
    {
        player_vel[X] = player_vel[X] / magnitude;
        player_vel[Y] = player_vel[Y] / magnitude;
        player_vel[Z] = player_vel[Z] / magnitude;
    }
    
    //Scale via speed.
    player_vel[X] *= player_speed;
    player_vel[Y] *= player_speed;
    player_vel[Z] *= player_speed;
    
    //Set last velocity.
    player_last[X] = player_vel[X];
    player_last[Y] = player_vel[Y];
    player_last[Z] = player_vel[Z];
    
    //Change player position.    
    player_pos[X] += 0.3 * player_vel[X];
    player_pos[Y] += 0.3 * player_vel[Y];
    player_pos[Z] += 0.3 * player_vel[Z];
    
    //Change light position based on user controls.
    float lightPos[] = {-player_pos[X], 3.0, -player_pos[Z], 1.0}; // Spotlight position.
    
    //Always point the light downwards.
    float spotDirection[] = {0.0, -1.0, 0.0}; // Spotlight direction.
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    
    //Control camera using user input.
    gluLookAt(-player_pos[X], 4.0, -player_pos[Z] + 6.0f,
              -player_pos[X], -player_pos[Y], -player_pos[Z] - 6.0f, 0.0, 1.0, 0.0);
    
    glPushMatrix();
    
    // Spotlight properties including position.
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotAngle);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spotExponent);
    
    glPopMatrix();
    
    drawReflectedTeapots();
    
    drawTeapots();
    
    glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize (int w, int h)
{
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w/(float)h, 1.0, 40.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard down input processing routine.
void keyInputDown(unsigned char key, int x, int y)
{
    
        if (key == 'W' || key == 'w') {
            player_dir &= ~DIR_BACKWARD;
            player_dir |= DIR_FORWARD;
        } else if (key == 'S' || key == 's') {
            player_dir &= ~DIR_FORWARD;
            player_dir |= DIR_BACKWARD;
        } else if (key == 'A' || key == 'a') {
            player_dir &= ~DIR_RIGHT;
            player_dir |= DIR_LEFT;
        } else if (key == 'D' || key == 'd') {
            player_dir &= ~DIR_LEFT;
            player_dir |= DIR_RIGHT;
        }
    glutPostRedisplay();
}

// Keyboard up input processing routine.
void keyInputUp(unsigned char key, int x, int y)
{

        if (key == 'W' || key == 'w') {
            player_dir &= ~DIR_FORWARD;
        } else if (key == 'S' || key == 's') {
            player_dir &= ~DIR_BACKWARD;
        } else if (key == 'A' || key == 'a') {
            player_dir &= ~DIR_LEFT;
        } else if (key == 'D' || key == 'd') {
            player_dir &= ~DIR_RIGHT;
        }
    
        if (key == 't')
        {
            isThrowTeapotEnabled = 1;
        }
    
    glutPostRedisplay();
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    if (key == GLUT_KEY_PAGE_DOWN)
    {
        spotAngle -= 1.0;
    }
    if( key == GLUT_KEY_PAGE_UP)
    {
        spotAngle += 1.0;
    }
    if (key == GLUT_KEY_UP)
    {
        if (zMove >= -2.0) zMove -= 0.2;
    }
    if (key == GLUT_KEY_DOWN)
    {
        if (zMove <= 9.0) zMove += 0.2;
    }
    if (key == GLUT_KEY_LEFT)
    {
        xMove -= 0.2;
    }
    if (key == GLUT_KEY_RIGHT)
    {
        xMove += 0.2;
    }
    glutPostRedisplay();
}

void mouseInput(int x, int y)
{
    if (x >= 0 && x <= 500 && y >= 0 && y <= 500)
    {
        cout << "X: " << x << ", Y: " << y << endl;
        player_rot[Y] += x * -1.0;
        if (player_rot[Y] >= TAU)
            player_rot[Y] -= TAU;
        if (player_rot[Y] < 0)
            player_rot[Y] += TAU;
        glutPostRedisplay();
    }
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Use WASD (W = up, A = left, S = down, D = right) to move the player." << endl;
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("GlowingTeapots.cpp");
    setup();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInputDown);
    glutKeyboardUpFunc(keyInputUp);
    //glutSpecialFunc(specialKeyInput);
    //glutPassiveMotionFunc(mouseInput);
    glutTimerFunc(5, animate, 1);
    glutMainLoop();
    
    return 0;
}
