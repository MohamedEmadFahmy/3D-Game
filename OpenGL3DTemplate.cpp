#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
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
float thirdPersonCameraHeightOffset = 1.5f;
float thirdPersonCameraDistanceOffset = 3.0f;


// Player Global Variables
float playerX = groundLength / 2;
float playerZ = groundWidth / 2;
float playerY = 0;




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
	std::string view;

    Camera() {
        view = "front";  // Default view

        eye = Vector3f(groundLength / 2, 3.0f, 2.0f * groundWidth);  // Initial eye position
        center = Vector3f(groundLength / 2, 0.0f, groundWidth / 2);  // Look at the center of the ground
        up = Vector3f(0.0f, 1.0f, 0.0f);  // Up vector pointing in the Y direction
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

    void toggleView() {
        if (view == "front") {
            view = "side";


            // Side View
            eye = Vector3f(-2.0 * groundLength / 2, 3.0f, groundWidth / 2);
            center = Vector3f(groundLength / 2, 0.0f, groundWidth / 2);
            up = Vector3f(0.0f, 1.0f, 0.0f);
        }
        else if (view == "side") {
            view = "top";

			// Top View
            eye = Vector3f(groundLength / 2, 10.0f, groundWidth / 2);
            center = Vector3f(groundLength / 2, 0.0f, groundWidth / 2);
            up = Vector3f(0.0f, 0.0f, -1.0f);
        }
        else if (view == "top") {
            view = "third-person";

			// Third-person View
            eye = Vector3f(playerX, playerY + thirdPersonCameraHeightOffset, playerZ + thirdPersonCameraDistanceOffset);
            center = Vector3f(playerX, playerY, playerZ);
            up = Vector3f(0.0f, 1.0f, 0.0f);
        }
        else if (view == "third-person") {
            view = "front";

			// Front View
            eye = Vector3f(groundLength / 2, 3.0f, 2.0f * groundWidth);
            center = Vector3f(groundLength / 2, 0.0f, groundWidth / 2);
            up = Vector3f(0.0f, 1.0f, 0.0f);
        }
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
    glPushMatrix();

    // Translate to player's position
    glTranslated(x, y, z);

    // Rotate the player 180 degrees around the Y-axis to face away
    glRotated(180, 0, 1, 0);

    // Head (Sphere)
    glPushMatrix();
    glTranslated(0, 0.9, 0); // Position for the head relative to the center
    glColor3f(1.0, 0.8, 0.6);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();

    // Scarf (Cylinder)
    glPushMatrix();
    glTranslated(0, 0.9, 0); // Position around the neck
    glRotated(90, 1, 0, 0); // Rotate to align with the neck
    glColor3f(1.0, 0.0, 0.0);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 0.11, 0.11, 0.1, 20, 20); // Cylinder for the scarf
    glPopMatrix();

    // Body (Cuboid)
    glPushMatrix();
    glTranslated(0, 0.6, 0); // Center position for the body
    glColor3f(0.2, 0.2, 0.25);
    glScaled(0.2, 0.4, 0.1); // Scale to form a cuboid shape
    glutSolidCube(1.0);
    glPopMatrix();

    // Left Arm (Cylinder)
    glPushMatrix();
    glTranslated(-0.11, 0.8, 0); // Position for the left arm
    glRotated(70, 1, 0, 0); // Rotate slightly to show a natural bend
    gluCylinder(quad, 0.02, 0.02, 0.3, 20, 20); // Cylinder for the left arm
    glPopMatrix();

    // Right Arm (Cylinder)
    glPushMatrix();
    glTranslated(0.11, 0.8, 0); // Position for the right arm
    glRotated(70, 1, 0, 0); // Rotate slightly to show a natural bend
    gluCylinder(quad, 0.02, 0.02, 0.3, 20, 20); // Cylinder for the right arm
    glPopMatrix();

    // Left Leg (Cylinder)
    glPushMatrix();
    glTranslated(-0.05, 0.4, 0); // Position for the left leg
    glRotated(110, 1, 0, 0); // Slight rotation for natural stance
    gluCylinder(quad, 0.03, 0.03, 0.4, 20, 20); // Cylinder for the left leg
    glPopMatrix();

    // Right Leg (Cylinder)
    glPushMatrix();
    glTranslated(0.05, 0.4, 0); // Position for the right leg
    glRotated(80, 1, 0, 0); // Slight rotation for natural stance
    gluCylinder(quad, 0.03, 0.03, 0.4, 20, 20); // Cylinder for the right leg
    glPopMatrix();

    gluDeleteQuadric(quad);
    glPopMatrix();  // Pop the player matrix
}


void updateCamera() {
    if (camera.view == "third-person") {
        // Continuously update camera position to follow player
        camera.eye = Vector3f(playerX, playerY + thirdPersonCameraHeightOffset, playerZ + thirdPersonCameraDistanceOffset);
        camera.center = Vector3f(playerX, playerY, playerZ);
        camera.up = Vector3f(0.0f, 1.0f, 0.0f);
    }
}

void Display() {
    setupCamera();
    setupLights();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawWalls();
	drawPlayer(playerX, playerY, playerZ);
	drawGridlines();
	updateCamera();

    glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
    float playerSpeed = 0.1;

    //printf("Key %c ", key);

    switch (key) {
        case 'w':
			playerZ -= playerSpeed;
            break;
        case 's':
            playerZ += playerSpeed;
            break;
        case 'a':
            playerX -= playerSpeed;
            break;
        case 'd':
            playerX += playerSpeed;
            break;
        case 'c':
			camera.toggleView();
			break;
    }
    glutPostRedisplay();
}

void Special(int key, int x, int y) {
    float cameraSpeed = 0.2;

    switch (key) {
        case GLUT_KEY_UP:
            camera.moveY(cameraSpeed);  // Move up (increase Y)
            break;
        case GLUT_KEY_DOWN:
            camera.moveY(-cameraSpeed); // Move down (decrease Y)
            break;
        case GLUT_KEY_ESCAPE:
            exit(EXIT_SUCCESS);
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
