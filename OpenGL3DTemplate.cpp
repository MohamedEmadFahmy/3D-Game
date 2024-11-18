#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <glut.h>
#include <cmath>
#include <vector>



#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
void drawCylinder(float radius, float height, int slices, int stacks);

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
float playerAngle = 0.0f;


// Golf Balls Global Variables


// Ball Color
float ballColor[3] = { 1.0f, 1.0f, 1.0f };  // White color for the golf ball

// Struct to represent a Ball
struct Ball {

        float x, y, z;   // Position of the ball
        float radius;    // Radius of the ball

        // Constructor to initialize the ball's properties
        Ball(float x = 0.0f, float z = 0.0f) {
            this->x = x;
            this->y = 0.3f;
            this->z = z;
            this->radius = 0.1;
        }

        // Method to draw the ball
        void draw() {  

            // Apply translation to the entire scene (big translation matrix)
            glPushMatrix();
            glTranslatef(x, y, z);  // This translates the entire scene by (x, y, z)

            // Drawing the Golf Ball (Solid Sphere)
            glPushMatrix();
            glTranslatef(0.0f, -0.075f, 0.0f);
            glColor3f(1.0f, 1.0f, 1.0f);  // White color for the golf ball
            glutSolidSphere(0.1f, 20, 20);  // Sphere representing the golf ball
            glPopMatrix();

            // Drawing the Cup Indicator (Small Cylinder inside the ball for visual interest)
            glPushMatrix();
            glTranslatef(0.0f, -0.17f, 0.0f);  // Position the cup indicator inside the ball
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate the cone 180 degrees around the X-axis
            glColor3f(1.0f, 0.0f, 0.0f);  // Gray for the cup indicator
            drawCylinder(radius, 0.03f, 10, 10);  // Small cylinder inside the ball
            glPopMatrix();

            // Drawing the Tee (Small Cone under the ball)
            glPushMatrix();
            glTranslatef(0.0f, -0.197f, 0.0f);  // Position the tee under the ball (relative to the ball)

            // Rotate the cone to point downwards
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate the cone 180 degrees around the X-axis

            glColor3f(0.6f, 0.3f, 0.0f);  // Brown color for the tee
            glutSolidCone(0.05f, 0.1f, 10, 10);  // Small cone representing the tee
            glPopMatrix();


            // Pop the translation matrix
            glPopMatrix();
        }
    };

// A list (vector) to hold multiple balls
std::vector<Ball> balls;


// Game Global Variables
int initialBallsCount;
int ballsLeftCount;
float timeLeft;  // Time in seconds







// Add balls to the list
void createAllBalls() {
    balls.push_back(Ball(2.0f, 6.0f));
    balls.push_back(Ball(4.0f, 4.0f));  
    balls.push_back(Ball(4.0f, 9.0f));
    balls.push_back(Ball(6.0f, 6.0f));
    balls.push_back(Ball(7.0f, 2.0f));
    balls.push_back(Ball(8.0f, 8.0f));
}

// Draw all the balls in the list
void drawAllBalls() {
    for (Ball& ball : balls) {
        ball.draw();  // Call the draw method for each ball in the list
    }
}



void drawCylinder(float radius, float height, int slices, int stacks) {
    // Draw the side of the cylinder (using gluCylinder)
    GLUquadricObj* quadric = gluNewQuadric();
    glPushMatrix();
    gluCylinder(quadric, radius, radius, height, slices, stacks);  // Draw the side of the cylinder
    glPopMatrix();

    // Draw the top face of the cylinder
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height / 2);  // Position at the top of the cylinder
    glBegin(GL_POLYGON);  // Start drawing the top face
    for (int i = 0; i < slices; ++i) {
        float angle = 2.0f * 3.14159265358979f * i / slices;
        glVertex3f(radius * cos(angle), radius * sin(angle), 0.0f);
    }
    glEnd();
    glPopMatrix();

    gluDeleteQuadric(quadric);
}

float roundTo10DecimalPlaces(float number) {
    return std::round(number * 1e10f) / 1e10f;  // Round and return as float
}

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
        view = "third-person";  // Default view

        // Third-person View
        eye = Vector3f(playerX, playerY + thirdPersonCameraHeightOffset, playerZ + thirdPersonCameraDistanceOffset);
        center = Vector3f(playerX, playerY, playerZ);
        up = Vector3f(0.0f, 1.0f, 0.0f);
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
}

void drawFenceSegment(float xStart, float y, float zStart, float length, bool horizontal) {
    const int numPosts = 10;  // Number of vertical posts per side
    const float postHeight = 1.0f;
    const float postRadius = 0.05f;
    const float railThickness = 0.03f;
    const float railHeightOffset = 0.4f;

    GLUquadric* quad = gluNewQuadric();  // Create a quadric object for cylinders

    glColor3f(0.6f, 0.4f, 0.2f);  // Brown for wooden fences

    // Draw vertical posts
    for (int i = 0; i < numPosts; i++) {
        glPushMatrix();
        float offset = (length / (numPosts - 1)) * i;
        if (horizontal) {
            glTranslatef(xStart + offset, y, zStart);
        }
        else {
            glTranslatef(xStart, y, zStart + offset);
        }
        glRotatef(-90, 1.0, 0.0, 0.0);  // Rotate to make cylinder upright
        gluCylinder(quad, postRadius, postRadius, postHeight, 20, 20);  // Solid cylinder
        glPopMatrix();
    }

    // Draw horizontal rails
    glPushMatrix();
    if (horizontal) {
        glTranslatef(xStart + length / 2, y + railHeightOffset, zStart);
        glScalef(length, railThickness, railThickness);
    }
    else {
        glTranslatef(xStart, y + railHeightOffset, zStart + length / 2);
        glScalef(railThickness, railThickness, length);
    }
    glutSolidCube(1.0);
    glPopMatrix();

    gluDeleteQuadric(quad);  // Clean up the quadric object
}

void drawFences(float groundLength, float groundWidth) {
    float groundLevel = 0.0f;

    // Back fence (horizontal along the Z-axis)
    drawFenceSegment(0, groundLevel, 0, groundLength, true);

    // Left side fence (vertical along the X-axis)
    drawFenceSegment(0, groundLevel, 0, groundWidth, false);

    // Right side fence (vertical along the X-axis, at groundLength in X direction)
    drawFenceSegment(groundLength, groundLevel, 0, groundWidth, false);
}

void drawPlayer() {
    glPushMatrix();

    // Translate by playerX, playerY, playerZ and rotate by playerAngle on the Y axis
    glTranslatef(playerX, playerY, playerZ);
    glRotatef(playerAngle, 0.0f, 1.0f, 0.0f);  // Rotate around the Y axis

    // Head (Sphere)
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.0f);  // Position the head above the body
    glColor3f(1.0f, 0.8f, 0.6f);      // Skin color
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();

    // Eyes (Two small spheres for the eyes)
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, -0.2f);  // Position the eyes slightly above the center of the face

    // Left Eye
    glPushMatrix();
    glTranslatef(-0.07f, 0.05f, 0.0f); // Left eye position
    glColor3f(0.0f, 0.0f, 0.0f);       // Black color for eyes
    glutSolidSphere(0.03, 10, 10);      // Left eye size
    glPopMatrix();

    // Right Eye
    glPushMatrix();
    glTranslatef(0.07f, 0.05f, 0.0f);  // Right eye position
    glColor3f(0.0f, 0.0f, 0.0f);       // Black color for eyes
    glutSolidSphere(0.03, 10, 10);      // Right eye size
    glPopMatrix();


    // Smile (A simple curve using line strip)
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.0f);  // Position smile at the center of the face
    glColor3f(0.0f, 0.0f, 0.0f);     // Smile color (black)
    glPopMatrix();

    glBegin(GL_LINE_STRIP);
    for (float i = -0.1f; i <= 0.1f; i += 0.02f) {
        float y = -0.05f + 0.03f * sin(i * 3.14159f);  // Create a smile shape
        glVertex3f(i, y, 0.0f); // Adjust coordinates for the smile
    }
    glEnd();
    glPopMatrix();

    // Body (Cylinder - Approximation with disk stack)
    glPushMatrix();
    glTranslatef(0.0f, 1.2f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue shirt
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.2, 0.6, 20, 20); // Create a cylinder-like shape
    glPopMatrix();

    // Left Arm (Cylinder - Approximation with disk stack)
    glPushMatrix();
    glTranslatef(-0.25f, 1.0f, 0.0f); // Position the arm
    glColor3f(1.0f, 0.0f, 1.0f);      // Blue sleeve color
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.07, 0.4, 20, 20);  // Create the left arm as a cylinder
    glPopMatrix();

    // Right Arm (Cylinder - Approximation with disk stack)
    glPushMatrix();
    glTranslatef(0.25f, 1.0f, 0.0f);  // Position the arm
    glColor3f(1.0f, 0.0f, 1.0f);      // Blue sleeve color
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.07, 0.4, 20, 20);  // Create the right arm as a cylinder
    glPopMatrix();

    // Left Leg (Cylinder - Approximation with disk stack)
    glPushMatrix();
    glTranslatef(-0.1f, 0.6f, 0.0f); // Position the leg
    glColor3f(0.3f, 0.3f, 0.3f);     // Gray pants color
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.1, 0.6, 20, 20);  // Create the left leg as a cylinder
    glPopMatrix();

    // Right Leg (Cylinder - Approximation with disk stack)
    glPushMatrix();
    glTranslatef(0.1f, 0.6f, 0.0f);  // Position the leg
    glColor3f(0.3f, 0.3f, 0.3f);     // Gray pants color
    glRotatef(90, 1, 0, 0);
    drawCylinder(0.1, 0.6, 20, 20);  // Create the right leg as a cylinder
    glPopMatrix();

    // Optional: Hands (Small Spheres for detail)
    glPushMatrix();
    glTranslatef(-0.25f, 1.1f, 0.0f);  // Left hand position
    glColor3f(1.0f, 0.8f, 0.6f);        // Skin color
    glutSolidSphere(0.1, 20, 20);       // Left hand sphere
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.25f, 1.1f, 0.0f);   // Right hand position
    glColor3f(1.0f, 0.8f, 0.6f);        // Skin color
    glutSolidSphere(0.1, 20, 20);       // Right hand sphere
    glPopMatrix();

    glPopMatrix();
}

void drawClubHouse(float x, float y, float z) {
    // Move the entire clubhouse to the specified position (x, y, z)
    glPushMatrix();

    glScalef(0.6f, 0.6f, 0.6f);  // Scaling factor to reduce size
    glTranslatef(x, y, z);  // Position the clubhouse

    // 1. Draw the walls of the clubhouse (a large cube representing the building)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 1.0f);  // Light gray for the walls
    glutSolidCube(3.0f);  // Cube representing the building
    glPopMatrix();

    // 2. Draw the roof of the clubhouse (a cone on top of the walls)
    glPushMatrix();
    glTranslatef(0.0f, 1.5f, 0.0f);  // Move up above the walls
    glRotatef(270.0f, 1.0f, 0.0f, 0.0f);  // Rotate the cone 180 degrees around the X-axis
    glColor3f(0.6f, 0.3f, 0.0f);  // Brown color for the roof
    glutSolidCone(1.5f, 2.5f, 100, 10);  // Cone representing the roof
    glPopMatrix();

    // 3. Draw the door (a small rectangle in the front)
    glPushMatrix();
    glTranslatef(0.0f, -0.7f, 1.45f);  // Position it slightly above the ground and at the center
    glColor3f(0.8235f, 0.7059f, 0.5490f);
    glScalef(1.0f, 1.5f, 0.2f);
    glutSolidCube(1.0f);  // Cube representing the door
    glPopMatrix();

    // 4. Draw the windows (four small squares on the sides of the building)
    glPushMatrix();
    glTranslatef(0.8f, 0.5f, 1.4f);  // Left window
    glColor3f(1.0f, 1.0f, 1.0f);  // Light blue for the window
    glutSolidCube(0.3f);  // Cube representing the window
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.2f, 0.5f, 1.4f);  // Right window
    glColor3f(1.0f, 1.0f, 1.0f);  // Light blue for the window
    glutSolidCube(0.3f);  // Cube representing the window
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.8f, 0.9f, 1.4f);  // Right window
    glColor3f(1.0f, 1.0f, 1.0f);  // Light blue for the window
    glutSolidCube(0.3f);  // Cube representing the window
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.2f, 0.9f, 1.4f);  // Right window
    glColor3f(1.0f, 1.0f, 1.0f);  // Light blue for the window
    glutSolidCube(0.3f);  // Cube representing the window
    glPopMatrix();

    // 5. Draw the chimney (a small cylinder on the roof)
    glPushMatrix();
    glTranslatef(1.2f, 2.5f, 1.2f);  // Position the chimney above the 
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate the cone 180 degrees around the X-axis
    glColor3f(0.5f, 0.5f, 0.5f);  // Gray color for the chimney
    drawCylinder(0.3f, 1.0f, 10, 10);  // Cylinder representing the chimney
    glPopMatrix();

    // Pop the transformation matrix
    glPopMatrix();
}

void drawGolfCart(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);  // Position the golf cart at (x, y, z)
    glRotatef(90.0, 0.0f, 1.0f, 0.0f);

    // 1. Draw the cart body (a rectangular shape for the cart)
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);  // Position the body slightly above the ground
    glColor3f(1.0f, 0.0f, 0.0f);  // Green color for the cart body
    glScalef(2.0f, 0.5f, 1.0f);  // Scale to make a long, flat body
    glutSolidCube(1.0f);  // Cube representing the body of the cart
    glPopMatrix();

    // 2. Draw the roof (a flat rectangle above the cart, higher than before)
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.0f);  // Position the roof higher above the cart body
    glColor3f(0.8f, 0.8f, 0.8f);  // Light gray color for the roof
    glScalef(2.0f, 0.2f, 1.0f);  // Scale to make a flat roof
    glutSolidCube(1.0f);  // Cube representing the roof
    glPopMatrix();


	float wheelHeightOffset = -0.1f;  // Height offset for the wheels
    // 3. Draw the wheels (using cylinders for the wheels)
    glPushMatrix();
    glTranslatef(-0.8f, wheelHeightOffset, 0.4f);  // Position the back-right wheel
    glColor3f(0.2f, 0.2f, 0.2f);  // Dark color for the wheels
    drawCylinder(0.2f, 0.1f, 100, 10);  // Wheel 1
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.8f, wheelHeightOffset, 0.4f);  // Position the front-right wheel
    glColor3f(0.2f, 0.2f, 0.2f);  // Dark color for the wheels
    drawCylinder(0.2f, 0.1f, 100, 10);  // Wheel 2
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.8f, wheelHeightOffset, -0.5f);  // Position the back-left wheel
    glColor3f(0.2f, 0.2f, 0.2f);  // Dark color for the wheels
    drawCylinder(0.2f, 0.1f, 100, 10);  // Wheel 3
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.8f, wheelHeightOffset, -0.5f);  // Position the front-left wheel
    glColor3f(0.2f, 0.2f, 0.2f);  // Dark color for the wheels
    drawCylinder(0.2f, 0.1f, 100, 10);  // Wheel 4
    glPopMatrix();

    // 4. Draw the seat (a simple rectangle on top of the cart body)
    glPushMatrix();
    glTranslatef(0.5f, 0.6f, 0.0f);  // Position the seat above the cart body
    glColor3f(0.6f, 0.3f, 0.0f);  // Brown color for the seat
    glScalef(0.4f, 0.2f, 0.8f);  // Scale to make a flat seat
    glutSolidCube(1.0f);  // Cube representing the seat
    glPopMatrix();

    // 5. Draw the steering wheel (a simple circle on top of the cart)
    glPushMatrix();
    glTranslatef(-1.0f, 0.5f, 0.0f);  // Position the steering wheel at the front
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);  // Rotate the beam to stand vertically
    glColor3f(0.0f, 0.0f, 0.0f);  // Black color for the steering wheel
    glutSolidTorus(0.1f, 0.15f, 10, 10);  // Steering wheel represented as a torus
    glPopMatrix();

// 6. Draw the support beams (cylinders between the roof and body)
// First left beam (Positioned at the front-left
	float beamHeight = 0.8f;  // Height of the support beams
    float beamTranslationOffset = 1.3f;
    glPushMatrix();
    glTranslatef(-0.8f, beamTranslationOffset, 0.45f);  // Position the first left support beam (closer to the front of the cart)
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate the beam to stand vertically
    glColor3f(0.6f, 0.3f, 0.0f);  // Brown color for the support beams
    drawCylinder(0.05f, beamHeight, 10, 10);  // First left support beam, longer (height = 0.5)
    glPopMatrix();

    // Second left beam (Positioned at the back-left)
    glPushMatrix();
    glTranslatef(-0.8f, beamTranslationOffset, -0.45f);  // Position the second left support beam (closer to the back of the cart)
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate the beam to stand vertically
    glColor3f(0.6f, 0.3f, 0.0f);  // Brown color for the support beams
    drawCylinder(0.05f, beamHeight, 10, 10);  // Second left support beam, longer (height = 0.5)
    glPopMatrix();

    // Right beams
    // First right beam (Positioned at the front-right)
    glPushMatrix();
    glTranslatef(0.8f, beamTranslationOffset, 0.45f);  // Position the first right support beam (closer to the front of the cart)
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate the beam to stand vertically
    glColor3f(0.6f, 0.3f, 0.0f);  // Brown color for the support beams
    drawCylinder(0.05f, beamHeight, 10, 10);  // First right support beam, longer (height = 0.5)
    glPopMatrix();

    // Second right beam (Positioned at the back-right)
    glPushMatrix();
    glTranslatef(0.8f, beamTranslationOffset, -0.45f);  // Position the second right support beam (closer to the back of the cart)
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate the beam to stand vertically
    glColor3f(0.6f, 0.3f, 0.0f);  // Brown color for the support beams
    drawCylinder(0.05f, beamHeight, 10, 10);  // Second right support beam, longer (height = 0.5)
    glPopMatrix();


    glPopMatrix();  // Pop the transformation matrix for the entire golf cart
}

void drawFlagPole(float x, float y, float z) {
    // Move the entire flagpole to the specified position (x, y, z)
    glPushMatrix();
    glTranslatef(x, y, z);  // Position the flagpole

    // 1. Draw the base of the flagpole (a short cylinder)
    glPushMatrix();
    glColor3f(0.3f, 0.3f, 0.3f);  // Gray color for the base
    glTranslatef(0.0f, 0.15f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Rotate cylinder upright
    drawCylinder(0.1f, 0.05f, 20, 20);  // Base cylinder
    glPopMatrix();

    // 2. Draw the pole (a tall cylinder)
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f);  // Light gray for the pole
    glTranslatef(0.0f, 0.2f, 0.0f);  // Raise the pole above the base
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Rotate cylinder upright
    drawCylinder(0.05f, 3.0f, 20, 20);  // Pole cylinder
    glPopMatrix();

    // 3. Draw the hole near the top (a torus/ring)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);  // Black color for the hole
    glTranslatef(0.0f, 2.9f, 0.0f);  // Position near the top of the pole
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);  // Rotate torus to align with the pole
    glutSolidTorus(0.05f, 0.15f, 20, 20);  // Torus for the hole
    glPopMatrix();

    // 4. Draw the flag (a flat rectangle using triangles)
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);  // Red color for the flag
    glTranslatef(0.15f, 2.9f, 0.0f);  // Position flag slightly below the hole
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.2f, 0.0f);  // Top corner
    glVertex3f(0.0f, -0.2f, 0.0f); // Bottom corner
    glVertex3f(0.4f, 0.0f, 0.0f);  // End corner
    glEnd();
    glPopMatrix();

    glPopMatrix();  // Pop the flagpole transformation
}

void drawSandpit(float x, float y, float z) {
    // Move the entire sandpit to the specified position (x, y, z)
    glPushMatrix();
    glTranslatef(x, y, z);  // Position the sandpit
    glScalef(0.3f, 0.3f, 0.3f);  // Scaling factor to reduce size

    // 1. Draw the base of the sandpit (a short cylinder)
    glPushMatrix();
    glColor3f(0.0f, 0.0f, 0.0f);  // Sand color for the base
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Rotate cylinder to lie flat
    drawCylinder(3.0f, 0.3f, 50, 10);  // Cylinder as the sandpit base
    glPopMatrix();

    
    glPushMatrix();
    glColor3f(1.0f, 0.70f, 0.15f);  // Light sand color
    glTranslatef(0.0f, 0.22f, 0.0f);  // Raise slightly above the base
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // Align with the ground
    drawCylinder(3.1f, 0.15f, 50, 10);  // Slightly larger cylinder for the border
    glPopMatrix();
    

    // Oval pebbles with more spaced-out positions
    float positions[10][2] = {
        {-1.0f, -1.0f},
        {1.2f, 0.8f},
        {-1.5f, 1.3f},
        {0.8f, -1.2f},
        {-1.8f, 0.5f},
        {1.5f, -0.5f},
        {-0.8f, 1.5f},
        {0.3f, 1.8f},
        {1.7f, 1.2f},
        {-1.2f, -1.8f}
    };

    for (int i = 0; i < 10; i++) {
        glPushMatrix();
        glTranslatef(positions[i][0], 0.4f, positions[i][1]);
        glScalef(1.5f, 0.5f, 1.0f);  // Non-uniform scaling to create oval shapes
        glColor3f(0.75f, 0.75f, 0.75f);  // Silver color for the pebbles
        glutSolidSphere(0.2f, 20, 20);  // Spherical pebble
        glPopMatrix();
    }

    glPopMatrix();  // Pop the sandpit transformation
}

void drawRockGroup(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);  // Move the group of rocks to the specified position
    glScalef(0.3, 0.3, 0.3);

    // 1. Draw a larger base rock using a scaled sphere
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.5f);  // Gray color for the rock
    glScalef(1.5f, 0.5f, 1.0f);  // Scale the sphere to make it more flattened
    glutSolidSphere(0.5f, 20, 20);  // Base of the rock group
    glPopMatrix();

    // 2. Draw smaller pebbles as spheres scattered around the base
    float pebblePositions[5][2] = {
        {-0.5f, 0.3f},
        {0.6f, -0.2f},
        {-0.2f, -0.5f},
        {0.4f, 0.7f},
        {-0.8f, 0.4f}
    };

    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef(pebblePositions[i][0], 0.1f, pebblePositions[i][1]);
        glColor3f(0.7f, 0.7f, 0.7f);  // Light gray color for the pebbles
        glutSolidSphere(0.2f, 10, 10);  // Smaller pebble sphere
        glPopMatrix();
    }

    // 3. Draw a slightly larger pebble using another sphere, more centralized
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);  // Position it in the center
    glColor3f(0.6f, 0.6f, 0.6f);  // Slightly darker gray for variation
    glutSolidSphere(0.3f, 15, 15);  // Central larger pebble
    glPopMatrix();

    glPopMatrix();  // Pop the transformation matrix
}


void updateCamera() {
    if (camera.view == "third-person") {
        // Continuously update camera position to follow player
        camera.eye = Vector3f(playerX, playerY + thirdPersonCameraHeightOffset, playerZ + thirdPersonCameraDistanceOffset);
        camera.center = Vector3f(playerX, playerY, playerZ);
        camera.up = Vector3f(0.0f, 1.0f, 0.0f);
    }
}

void handleCollisions() {
    float playerRadius = 0.3f;  // Radius of the player

    // Use an iterator to safely remove elements from the vector
    for (auto it = balls.begin(); it != balls.end();) {
        float distance = sqrt(pow(playerX - it->x, 2) + pow(playerZ - it->z, 2));

        if (distance < it->radius + playerRadius) {
            // Collision detected
            printf("Collision detected with ball at (%f, %f)\n", it->x, it->z);

            // Remove the ball from the vector
            it = balls.erase(it);  // erase returns the next iterator
        }
        else {
            ++it;  // Only increment the iterator if no removal was made
        }
    }

	ballsLeftCount = balls.size();  // Update the number of balls left
}

void displayGameInfo() {
    // Set the background color for the overlay (light gray background)
    glColor3f(1.0f, 1.0f, 1.0f);  // Light gray color for the overlay

    // Create a rectangle background to cover the top-left corner for the overlay
    glBegin(GL_QUADS);
    glVertex2f(0.0f, screenHeight);  // Top-left corner
    glVertex2f(screenWidth, screenHeight);  // Top-right corner
    glVertex2f(screenWidth, screenHeight - 100);  // Bottom-right corner
    glVertex2f(0.0f, screenHeight - 100);  // Bottom-left corner
    glEnd();

    // Set the text color to green
    glColor3f(0.0f, 1.0f, 0.0f); // Green color for the text

    // Create the timer text
    char timerText[20];
    sprintf(timerText, "Time: %.1f", timeLeft);

    // Create the goals text
    char goalsText[20];
    sprintf(goalsText, "Balls Left: %d", ballsLeftCount);

    // Set up orthographic projection for 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);  // Set the correct orthographic projection

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();        // Reset the modelview matrix

    // Render timer text at the top-left corner
    glRasterPos2f(10.0f, screenHeight - 40);
    for (const char* c = timerText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Render goals text slightly below the timer
    glRasterPos2f(10.0f, screenHeight - 80);
    for (const char* c = goalsText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Restore the projection and modelview matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glFlush();  // Ensure everything is rendered
}

void displayGameWon() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set background color for the screen (light green for a winning message)
    glClearColor(0.2f, 0.8f, 0.2f, 1.0f);  // Light green background color

    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);  // White text for the win message

    // Set up orthographic projection for 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);  // Set the correct orthographic projection

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();        // Reset the modelview matrix

    // Render "GAME WON" text in the center of the screen
    const char* wonMessage = "GAME WON!";
    float textWidth = 0.0f;
    for (const char* c = wonMessage; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glRasterPos2f((screenWidth - textWidth) / 2, screenHeight / 2);
    for (const char* c = wonMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Optionally, display the score or additional information
    char scoreText[20];
    sprintf(scoreText, "Balls Collected: %d", initialBallsCount);
    glRasterPos2f((screenWidth - textWidth) / 2, (screenHeight / 2) - 40);
    for (const char* c = scoreText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Restore the projection and modelview matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glFlush();
}

void displayGameOver() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set background color for the screen (dark red for a game-over message)
    glClearColor(0.8f, 0.2f, 0.2f, 1.0f);  // Dark red background color

    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);  // White text for the game-over message

    // Set up orthographic projection for 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, screenWidth, 0, screenHeight);  // Set the correct orthographic projection

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();        // Reset the modelview matrix

    // Render "GAME OVER" text in the center of the screen
    const char* gameOverMessage = "GAME OVER";
    float textWidth = 0.0f;
    for (const char* c = gameOverMessage; *c != '\0'; c++) {
        textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glRasterPos2f((screenWidth - textWidth) / 2, screenHeight / 2);
    for (const char* c = gameOverMessage; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Optionally, display the score or additional information
    char scoreText[20];
    sprintf(scoreText, "Balls Left: %d", ballsLeftCount);
    glRasterPos2f((screenWidth - textWidth) / 2, (screenHeight / 2) - 40);
    for (const char* c = scoreText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Restore the projection and modelview matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glFlush();
}


void Display() {
    if (timeLeft <= 0.0f) {
        if (ballsLeftCount == 0) {
			displayGameWon();
        }
        else {
			displayGameOver();
        }
    }
    else {

        setupCamera();
        setupLights();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawWalls();
        drawFences(groundLength, groundWidth);
        drawClubHouse(1.7f, 1.5f, 1.5f);
        drawGolfCart(9.2f, 0.35f, 1.4f);
        drawFlagPole(6.5f, -0.16f, 4.5f);
        drawSandpit(9.0f, 0.0f, 9.0f);

        // Rock Groups
        drawRockGroup(1.0f, 0.0f, 6.0f);
        drawRockGroup(2.0f, 0.0f, 9.0f);
        drawRockGroup(9.0f, 0.0f, 6.0f);
        drawRockGroup(6.0f, 0.0f, 9.5f);


        drawGridlines();

        drawPlayer();
        updateCamera();

        handleCollisions();

        drawAllBalls();

        displayGameInfo();
       
    }

    glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
    float playerSpeed = 0.1f;
    float playerAngleSpeed = 40.0f;  // Angle speed in degrees
    float newX, newZ;  // To store the new position for boundary checking

    /*printf("Vector X: %f\n", playerSpeed * cos(DEG2RAD(360.0f - playerAngle)));
    printf("Vector Z: %f\n", playerSpeed * sin(DEG2RAD(360.0f - playerAngle)));*/

    switch (key) {
    case 'w':
        // Calculate new position for forward movement and round to nearest integer
        newX = playerX - playerSpeed * cos(DEG2RAD(360.0f - playerAngle));
        newZ = playerZ - playerSpeed * sin(DEG2RAD(360.0f - playerAngle));

        printf("New X: %f\n", newX);
		printf("New Z: %f\n", newZ);

        // Check boundaries before updating position
        if (newX >= 0 && newX <= groundLength && newZ >= 0 && newZ <= groundWidth) {

            playerX = roundTo10DecimalPlaces(newX);
            playerZ = roundTo10DecimalPlaces(newZ);
        }
        break;

    case 's':
        // Calculate new position for backward movement and round to nearest integer
        newX = playerX + playerSpeed * cos(DEG2RAD(360.0f - playerAngle));
        newZ = playerZ + playerSpeed * sin(DEG2RAD(360.0f - playerAngle));

        printf("New X: %f\n", newX);
        printf("New Z: %f\n", newZ);

        // Check boundaries before updating position
        if (newX >= 0 && newX <= groundLength && newZ >= 0 && newZ <= groundWidth){
            playerX = roundTo10DecimalPlaces(newX);
            playerZ = roundTo10DecimalPlaces(newZ);
        }
        break;

    case 'a':
        // Turn left (counter-clockwise)
        playerAngle += playerAngleSpeed;

        // Keep playerAngle in the range [0, 360)
        if (playerAngle >= 360.0f) {
            playerAngle -= 360.0f;
        }
        break;

    case 'd':
        // Turn right (clockwise)
        playerAngle -= playerAngleSpeed;

        // Keep playerAngle in the range [0, 360)
        if (playerAngle < 0.0f) {
            playerAngle += 360.0f;
        }
        break;

    case 'c':
        camera.toggleView();  // Toggle camera view
        break;
    }

    //glutPostRedisplay();
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

void updateTimer(int value) {
    if (timeLeft > 0) {
        timeLeft -= 0.1f;  // Decrease time by 0.1 seconds
    }
    else {
        timeLeft = 0.0f;  // Time has run out
    }

    // Redisplay the window
    glutPostRedisplay();
    glutTimerFunc(16, updateTimer, 0);  // Call this function every 100 ms
}

void initGame() {
	createAllBalls();
	initialBallsCount = balls.size();
    ballsLeftCount = balls.size();
	timeLeft = 100.0f;

    glutTimerFunc(16, updateTimer, 0);  // Start the timer update
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

    initGame();

    glutMainLoop();
}
