#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>



#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

// Window Global Variables
int screenWidth = 1000;
int screenHeight = 1000;

float groundLength = 10.0f;
float groundWidth = 10.0f;

// Camera Global Variables
int lastMouseX, lastMouseZ;
bool isLeftMouseButtonPressed, isRightMouseButtonPressed = false;




class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f operator+(Vector3f& v) {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(Vector3f& v) {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() {
        return *this / sqrt(x * x + y * y + z * z);
    }

    Vector3f cross(Vector3f v) {
        return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = groundWidth / 2 , float eyeY = 3.0f, float eyeZ = 1.2 * groundWidth, float centerX = groundWidth / 2, float centerY = 0.0f, float centerZ = groundLength / 2, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
        eye = Vector3f(eyeX, eyeY, eyeZ);
        center = Vector3f(centerX, centerY, centerZ);
        up = Vector3f(upX, upY, upZ);
    }

    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        eye = eye + right * d;
        center = center + right * d;
    }

    void moveY(float d) {
        eye = eye + up.unit() * d;
        center = center + up.unit() * d;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        eye = eye + view * d;
        center = center + view * d;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
        right = view.cross(up);
        center = eye + view;
    }

    void rotateZ(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();

        // Rotate the view and right vectors around the Z-axis
        float cosA = cos(DEG2RAD(a));
        float sinA = sin(DEG2RAD(a));

        // Update view and right vectors for Z-axis rotation
        float newViewX = cosA * view.x - sinA * view.y;
        float newViewY = sinA * view.x + cosA * view.y;
        float newRightX = cosA * right.x - sinA * right.y;
        float newRightY = sinA * right.x + cosA * right.y;

        view.x = newViewX;
        view.y = newViewY;
        right.x = newRightX;
        right.y = newRightY;

        // Recalculate the center point based on the rotated view vector
        center = eye + view;
    }


    void look() {
        gluLookAt(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            up.x, up.y, up.z
        );
    }
};

Camera camera;

void drawWall(double thickness) {
    glPushMatrix();

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function

    // Set the color to green with some transparency (0.5 is 50% transparent)
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);  // RGBA, where A is the alpha value

    // Transform the wall
    glTranslated(10.0 / 2, 0, 10.0/ 2);
    glScaled(groundLength, thickness, groundWidth);

    // Draw the wall (as a solid cube)
    glutSolidCube(1);

    // Disable blending after drawing the transparent object
    glDisable(GL_BLEND);

    glPopMatrix();
}


void drawGridlines() {
    glPushMatrix();

	float y_coord_of_gridlines = 0.0f;  // Y-coordinate of the grid lines

    glLineWidth(10.0f);  // Adjust the line width here

    // Draw X-axis (Red)
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glBegin(GL_LINES);
    glVertex3f(0.0f, y_coord_of_gridlines, 0.0f);  // Start at the origin
    glVertex3f(10.0f, y_coord_of_gridlines, 0.0f);  // End point along the X-axis
    glEnd();

    // Draw Y-axis (Yellow)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
    glBegin(GL_LINES);
    glVertex3f(0.0f, y_coord_of_gridlines, 0.0f);  // Start at the origin
    glVertex3f(0.0f, 10.0f, 0.0f);  // End point along the Y-axis
    glEnd();

    // Draw Z-axis (Blue)
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color
    glBegin(GL_LINES);
    glVertex3f(0.0f, y_coord_of_gridlines, 0.0f);  // Start at the origin
    glVertex3f(0.0f, y_coord_of_gridlines, 10.0f);  // End point along the Z-axis
    glEnd();

    glPopMatrix();
}



void drawWalls() {
    drawWall(0.01);

   /* glPushMatrix();
    glRotated(90, 0, 0, 1.0);
    drawWall(0.02);
    glPopMatrix();

    glPushMatrix();
    glRotated(-90, 1.0, 0.0, 0.0);
    drawWall(0.02);
    glPopMatrix();*/
}

void setupLights() {
    GLfloat ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess[] = { 50 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    GLfloat lightIntensity[] = { 0.7f, 0.7f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1000/1000, 0.001, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look();
}

void drawPlayer(float x, float y, float z) {

    // Head (Sphere)
    glPushMatrix();
    glTranslated(x, y + 0.9, z); // Position for the head relative to the center
    glColor3f(1.0, 0.8, 0.6);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();

    // Scarf (Cylinder)
    glPushMatrix();
    glTranslated(x, y + 0.9, z); // Position around the neck relative to the center
    glRotated(90, 1, 0, 0); // Rotate to align with the neck
    glColor3f(1.0, 0.0, 0.0);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.11, 0.11, 0.1, 20, 20); // Cylinder for the scarf
    glPopMatrix();

    // Body (Cuboid)
    glPushMatrix();
    glTranslated(x, y + 0.6, z); // Center position for the body
    //glColor3f(0.0, 0.6, 1);
    glColor3f(0.2, 0.2, 0.25);
    glScaled(0.2, 0.4, 0.1); // Scale to form a cuboid shape
    glutSolidCube(1.0);
    glPopMatrix();

    // Left Arm (Cylinder)
    glPushMatrix();
    glTranslated(x - 0.11, y + 0.8, z); // Position for the left arm relative to the center
    //glRotated(-30, 0, 0, 1); // Rotate slightly to show a natural bend
    //glColor3f(0.2, 0.5, 1);
    glRotated(70, 1, 0, 0); // Rotate slightly to show a natural bend
    gluCylinder(quad, 0.02, 0.02, 0.3, 20, 20); // Cylinder for the left arm
    glPopMatrix();

    // Right Arm (Cylinder)
    glPushMatrix();
    glTranslated(x + 0.11, y + 0.8, z); // Position for the right arm relative to the center
    //glRotated(-30, 0, 0, 1); // Rotate slightly to show a natural bend
    glRotated(70, 1, 0, 0); // Rotate slightly to show a natural bend
    gluCylinder(quad, 0.02, 0.02, 0.3, 20, 20); // Cylinder for the right arm
    glPopMatrix();

    // Left Leg (Cylinder)
    glPushMatrix();
    glTranslated(x - 0.05, y + 0.4, z); // Position for the left leg relative to the center
    glRotated(110, 1, 0, 0); // Slight rotation for natural stance
    gluCylinder(quad, 0.03, 0.03, 0.4, 20, 20); // Cylinder for the left leg
    glPopMatrix();

    // Right Leg (Cylinder)
    glPushMatrix();
    glTranslated(x + 0.05, y + 0.4, z); // Position for the right leg relative to the center
    glRotated(80, 1, 0, 0); // Slight rotation for natural stance
    gluCylinder(quad, 0.03, 0.03, 0.4, 20, 20); // Cylinder for the right leg
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void Display() {
    setupCamera();
    setupLights();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawWalls();
	drawPlayer(groundWidth / 2, 0, groundLength / 2);
	drawGridlines();

    //// Adjust the camera to follow the skier if not in free-cam mode
    //if (view != "free") {
    //    camera.eye = Vector3f(skierPosition.x, skierPosition.y + 0.5f, skierPosition.z + 1.5f);
    //    camera.center = skierPosition;
    //    camera.look();
    //}

    glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
    float d = 0.05;

    switch (key) {
    case 'w':
        camera.moveY(d);
        break;
    case 's':
        camera.moveY(-d);
        break;
    case 'a':
        camera.moveX(d);
        break;
    case 'd':
        camera.moveX(-d);
        break;
    case 'q':
        camera.moveZ(d);
        break;
    case 'e':
        camera.moveZ(-d);
        break;

    case GLUT_KEY_ESCAPE:
        exit(EXIT_SUCCESS);
    }

    glutPostRedisplay();
}

void Special(int key, int x, int y) {
    float a = 1.0;

    switch (key) {
    case GLUT_KEY_UP:
        camera.rotateX(a);
        break;
    case GLUT_KEY_DOWN:
        camera.rotateX(-a);
        break;
    case GLUT_KEY_LEFT:
        camera.rotateY(a);
        break;
    case GLUT_KEY_RIGHT:
        camera.rotateY(-a);
        break;
    }

    glutPostRedisplay();
}

// Mouse event handlers
void Mouse(int button, int state, int x, int z) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            lastMouseX = x;
            lastMouseZ = z;
            isLeftMouseButtonPressed = true;
        }
        else {
            isLeftMouseButtonPressed = false;
        }
    }
    if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            lastMouseX = x;
            lastMouseZ = z;
            isRightMouseButtonPressed = true;
        }
        else {
            isRightMouseButtonPressed = false;
        }
    }
}

void MouseMotion(int x, int z) {
    if (isLeftMouseButtonPressed) {
        int deltaX = x - lastMouseX;
        int deltaZ = z - lastMouseZ;

        // Update camera position based on mouse movement
        camera.moveX(deltaX * 0.01);
        camera.moveZ(-deltaZ * 0.01);

        lastMouseX = x;
        lastMouseZ = z;
    }
    if (isRightMouseButtonPressed) {
        int deltaX = x - lastMouseX;
        int deltaZ = z - lastMouseZ;

		float rotationSpeed = 0.05;

        // Update camera position based on mouse movement
        camera.rotateY(deltaX * rotationSpeed);
        camera.rotateX(deltaZ * rotationSpeed);

        lastMouseX = x;
        lastMouseZ = z;

    }
    glutPostRedisplay();
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Golfito");
    glutDisplayFunc(Display);

    glutMouseFunc(Mouse);
    glutMotionFunc(MouseMotion);

    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Special);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);

    glutMainLoop();
}
