#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

using namespace std;

// PROTOTYPES
void displayBackground();
void displayToriGate();
void displayGodzilla();
void displayFire();
void displayBoy();
void initVBOs();
void displayBirds();
void mouseCallback(int button, int state, int x, int y);
void animateGodzilla(int value);

const int NUM_BIRDS = 15;
const GLfloat BIRD_COLOR[] = { 0.0f, 0.0f, 0.0f };
const GLfloat BIRD_SCALE = 0.015f;
GLuint boyVBO[2];
GLfloat birdOffsets[NUM_BIRDS * 2];

bool isFiring = false;

// --- ANIMATION VARIABLES ---
// Start off-screen to the LEFT
float godzillaX = 1.0f;
float godzillaY = 0.0f;  // New variable for the shaking height

// ----------------------------------------------------------------
// TIMER FUNCTION (ANIMATION)
// ----------------------------------------------------------------
void animateGodzilla(int value) {
    if (value == 0) {
        glutTimerFunc(16, animateGodzilla, 1);
    }
    else if (value == 1) {
        // Move to the RIGHT until he hits the center/right side
        if (godzillaX > -2.5f) {
            godzillaX -= 0.002f; // Move Right speed

            // --- WALKING SHAKE LOGIC ---
            // sin(godzillaX * 20.0f) creates a wave based on horizontal position.
            // 0.03f scales it down so it's a small shake, not a giant jump.
            // abs() makes it a bounce (always up) rather than up-down.
            godzillaY = 0.01f * abs(sin(godzillaX * 20.0f));

            glutPostRedisplay();
            glutTimerFunc(16, animateGodzilla, 1);
        }
    }
}

// ----------------------------------------------------------------
// MOUSE CALLBACK
// ----------------------------------------------------------------
void mouseCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) isFiring = true;
        else if (state == GLUT_UP) isFiring = false;
        glutPostRedisplay();
    }
}

// ----------------------------------------------------------------
// DISPLAY CALLBACK
// ----------------------------------------------------------------
void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glPushMatrix();
    glTranslatef(0.0f, godzillaY, 0.0f);
    displayBackground();

    glPushMatrix();

    glTranslatef(godzillaX, godzillaY, 0.0f);

    displayGodzilla();
    if (isFiring) {
        displayFire();
    }
    glPopMatrix();

    displayToriGate();
    displayBoy();
    glPopMatrix();

    displayBirds();

    glutSwapBuffers();
}

// ----------------------------------------------------------------
// MAIN FUNCTION
// ----------------------------------------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Manuzon - Sandiego - Computer Graphics");
    glutFullScreen();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLenum err = glewInit();
    if (GLEW_OK != err) return 1;

    initVBOs();

    // Initialize Birds
    GLfloat flockSpreadX = 0.25f;
    GLfloat flockSpreadY = 0.2f;
    for (int i = 0; i < NUM_BIRDS; ++i) {
        birdOffsets[i * 2] = ((float)rand() / RAND_MAX * flockSpreadX) - (flockSpreadX / 2.0f);
        birdOffsets[i * 2 + 1] = ((float)rand() / RAND_MAX * flockSpreadY) - (flockSpreadY / 2.0f);
    }

    glutTimerFunc(1000, animateGodzilla, 0);
    glutMouseFunc(mouseCallback);
    glutDisplayFunc(Display);
    glutMainLoop();

    return 0;
}

// ----------------------------------------------------------------
// DATA SECTION - TORII GATE GEOMETRY
// ----------------------------------------------------------------
GLfloat toriGateVertices[] = {
    // ---------------------------------------------------------
    // 3. LEFT PILLAR (Hashira)
    // ---------------------------------------------------------
    -0.2f,   0.19f,  0.0f,  // Top-Left
    -0.13f,  0.19f,  0.0f,  // Top-Right
    -0.13f, -0.6f,   0.0f,  // Bottom-Right
    -0.2f,  -0.6f,   0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 4. RIGHT PILLAR (Hashira)
    // ---------------------------------------------------------
     0.13f,  0.19f,  0.0f,  // Top-Left
     0.2f,   0.19f,  0.0f,  // Top-Right
     0.2f,  -0.6f,   0.0f,  // Bottom-Right
     0.13f, -0.6f,   0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 5. LOWER RED BEAM (Nuki)
     // ---------------------------------------------------------
     -0.28f,  0.1f,   0.0f,  // Top-Left
      0.28f,  0.1f,   0.0f,  // Top-Right
      0.26f,  0.02f,  0.0f,  // Bottom-Right
     -0.26f,  0.02f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 6. MIDDLE STRUT (Gakuzuka)
     // ---------------------------------------------------------
     -0.05f,  0.19f,  0.0f,  // Top-Left
      0.05f,  0.19f,  0.0f,  // Top-Right
      0.05f,  0.1f,   0.0f,  // Bottom-Right
     -0.05f,  0.1f,   0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 9. UPPER CONNECTOR RED BEAM LEFT
     // ---------------------------------------------------------
     -0.3f,  -0.18f,  0.0f,  // Top-Left
     -0.13f, -0.12f,  0.0f,  // Top-Right
     -0.13f, -0.24f,  0.0f,  // Bottom-Right
     -0.3f,  -0.3f,   0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 10. LOWER RED BEAM LEFT
     // ---------------------------------------------------------
     -0.3f,  -0.43f,  0.0f,  // Top-Left
     -0.13f, -0.37f,  0.0f,  // Top-Right
     -0.13f, -0.49f,  0.0f,  // Bottom-Right
     -0.3f,  -0.55f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 11. UPPER CONNECTOR RED BEAM RIGHT
     // ---------------------------------------------------------
      0.3f,  -0.18f,  0.0f,  // Top-Left
      0.13f, -0.12f,  0.0f,  // Top-Right
      0.13f, -0.24f,  0.0f,  // Bottom-Right
      0.3f,  -0.3f,   0.0f,  // Bottom-Left

      // ---------------------------------------------------------
      // 12. LOWER RED BEAM RIGHT
      // ---------------------------------------------------------
       0.3f,  -0.43f,  0.0f,  // Top-Left
       0.13f, -0.37f,  0.0f,  // Top-Right
       0.13f, -0.49f,  0.0f,  // Bottom-Right
       0.3f,  -0.55f,  0.0f,  // Bottom-Left

       // ---------------------------------------------------------
       // 7. SMALL LEFT PILLAR 
       // ---------------------------------------------------------
       -0.3f,  -0.08f,  0.0f,  // Top-Left
       -0.23f, -0.08f,  0.0f,  // Top-Right
       -0.23f, -0.7f,   0.0f,  // Bottom-Right
       -0.3f,  -0.7f,   0.0f,  // Bottom-Left

       // ---------------------------------------------------------
       // 8. SMALL RIGHT PILLAR 
       // ---------------------------------------------------------
        0.23f, -0.08f,  0.0f,  // Top-Left
        0.3f,  -0.08f,  0.0f,  // Top-Right
        0.3f,  -0.7f,   0.0f,  // Bottom-Right
        0.23f, -0.7f,   0.0f,  // Bottom-Left

        // ---------------------------------------------------------
        // 13. Pillar detail lower left
        // ---------------------------------------------------------
        -0.22f,  0.13f,  0.0f,  // Top-Left
        -0.11f,  0.13f,  0.0f,  // Top-Right
        -0.13f,  0.09f,  0.0f,  // Bottom-Right
        -0.2f,   0.09f,  0.0f,  // Bottom-Left

        // ---------------------------------------------------------
        // 14. Pillar detail lower right
        // ---------------------------------------------------------
         0.22f,  0.13f,  0.0f,  // Top-Left
         0.11f,  0.13f,  0.0f,  // Top-Right
         0.13f,  0.09f,  0.0f,  // Bottom-Right
         0.2f,   0.09f,  0.0f,  // Bottom-Left

         // ---------------------------------------------------------
         // 15. Pillar detail upper left
         // ---------------------------------------------------------
         -0.21f,  0.2f,   0.0f,  // Top-Left
         -0.12f,  0.2f,   0.0f,  // Top-Right
         -0.12f,  0.16f,  0.0f,  // Bottom-Right
         -0.21f,  0.16f,  0.0f,  // Bottom-Left

         // ---------------------------------------------------------
         // 16. Pillar detail upper right
         // ---------------------------------------------------------
          0.21f,  0.2f,   0.0f,  // Top-Left
          0.12f,  0.2f,   0.0f,  // Top-Right
          0.12f,  0.16f,  0.0f,  // Bottom-Right
          0.21f,  0.16f,  0.0f,  // Bottom-Left

          // ---------------------------------------------------------
          // 17. left side pillar roof
          // ---------------------------------------------------------
          -0.32f, -0.1f,   0.0f,  // Top-Left
          -0.21f, -0.1f,   0.0f,  // Top-Right
          -0.23f, -0.06f,  0.0f,  // Bottom-Right
          -0.3f,  -0.06f,  0.0f,  // Bottom-Left

          // ---------------------------------------------------------
          // 17. right side pillar roof
          // ---------------------------------------------------------
           0.32f, -0.1f,   0.0f,  // Top-Left
           0.21f, -0.1f,   0.0f,  // Top-Right
           0.23f, -0.06f,  0.0f,  // Bottom-Right
           0.3f,  -0.06f,  0.0f,  // Bottom-Left

           // ---------------------------------------------------------
           // 1. TOP BLACK ROOF (Kasagi)
           // ---------------------------------------------------------
           -0.39f,  0.3f,   0.0f,  // Top-Left
            0.39f,  0.3f,   0.0f,  // Top-Right
            0.37f,  0.25f,  0.0f,  // Bottom-Right
           -0.37f,  0.25f,  0.0f,  // Bottom-Left

           // ---------------------------------------------------------
           // 2. UPPER RED BEAM (Shimaki)
           // ---------------------------------------------------------
           -0.35f,  0.25f,  0.0f,  // Top-Left
            0.35f,  0.25f,  0.0f,  // Top-Right
            0.33f,  0.19f,  0.0f,  // Bottom-Right
           -0.33f,  0.19f,  0.0f,  // Bottom-Left
};

GLfloat colors[] = {
    // ---------------------------------------------------------
    // 3. LEFT PILLAR (Hashira)
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 4. RIGHT PILLAR (Hashira)
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 5. LOWER BEAM (Nuki)
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 6. MIDDLE STRUT (Gakuzuka)
    // ---------------------------------------------------------
    0.6f, 0.0f, 0.0f,  // Top-Left
    0.5f, 0.0f, 0.0f,  // Top-Right
    0.6f, 0.0f, 0.0f,  // Bottom-Right
    0.5f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 9. CONNECTOR LEFT TOP
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 10. CONNECTOR LEFT BOTTOM
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 11. CONNECTOR RIGHT TOP
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 12. CONNECTOR RIGHT BOTTOM
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 7. SMALL LEFT PILLAR
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 8. SMALL RIGHT PILLAR
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 13. LEFT LOWER DETAIL
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 14. RIGHT LOWER DETAIL
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 15. LEFT UPPER DETAIL
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 16. RIGHT UPPER DETAIL
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 17. ROOF SIDE PILLAR LEFT (Black)
    // ---------------------------------------------------------
    0.0f, 0.0f, 0.0f,  // Top-Left
    0.0f, 0.0f, 0.0f,  // Top-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 18. ROOF SIDE PILLAR RIGHT (Black)
    // ---------------------------------------------------------
    0.0f, 0.0f, 0.0f,  // Top-Left
    0.0f, 0.0f, 0.0f,  // Top-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 1. TOP BLACK ROOF (Kasagi)
    // ---------------------------------------------------------
    0.0f, 0.0f, 0.0f,  // Top-Left
    0.0f, 0.0f, 0.0f,  // Top-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 2. UPPER RED BEAM (Shimaki)
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left
};

// ----------------------------------------------------------------
// DISPLAY BACKGROUND (Sky, Sea, Snow)
// ----------------------------------------------------------------
void displayBackground() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Settings
    GLfloat snowFieldHeight = -0.60f;
    GLfloat seaLevel = -0.35f;

    // 1. Sky Gradient
    GLfloat skyVertices[] = {
        -1.0f,  1.0f, 0.0f,      // Top-Left 
         1.0f,  1.0f, 0.0f,      // Top-Right
         1.0f,  seaLevel, 0.0f,  // Bottom boundary
        -1.0f,  seaLevel, 0.0f   // Bottom boundary
    };
    GLfloat skyColors[] = {
        0.1f, 0.3f, 0.7f, // Top
        0.1f, 0.3f, 0.7f,
        0.6f, 0.7f, 0.95f, // Bottom
        0.6f, 0.7f, 0.95f
    };
    glVertexPointer(3, GL_FLOAT, 0, skyVertices);
    glColorPointer(3, GL_FLOAT, 0, skyColors);
    glDrawArrays(GL_QUADS, 0, 4);

    // 2. Sea Gradient
    GLfloat seaVertices[] = {
        -1.0f,  seaLevel, 0.0f,         // Top-Left
         1.0f,  seaLevel, 0.0f,         // Top-Right
         1.0f,  snowFieldHeight, 0.0f,  // Bottom Right
        -1.0f,  snowFieldHeight, 0.0f   // Bottom Left
    };
    GLfloat seaColors[] = {
        0.1f, 0.3f, 0.7f, // Top
        0.1f, 0.3f, 0.7f,
        0.2f, 0.5f, 0.8f, // Bottom
        0.2f, 0.5f, 0.8f
    };
    glVertexPointer(3, GL_FLOAT, 0, seaVertices);
    glColorPointer(3, GL_FLOAT, 0, seaColors);
    glDrawArrays(GL_QUADS, 0, 4);

    // 3. Snow Field (Procedural Bumps)
    GLfloat snowColor[] = { 1.0f, 1.0f, 1.0f };
    glColor3fv(snowColor);

    int numSegments = 20;
    GLfloat segmentWidth = 2.0f / numSegments;
    GLfloat snowBottom = -1.0f;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; ++i) {
        GLfloat x = -1.0f + (i * segmentWidth);
        GLfloat yBumpy = snowFieldHeight + 0.05f * sinf(x * 5.0f);
        glVertex3f(x, yBumpy, 0.0f);
        glVertex3f(x, snowBottom, 0.0f);
    }
    glEnd();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

// ----------------------------------------------------------------
// DISPLAY TORII GATE (Geometry Only)
// ----------------------------------------------------------------
void displayToriGate() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, toriGateVertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(GL_QUADS, 0, 72);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

// ----------------------------------------------------------------
// GODZILLA (Vertex Arrays + Procedural Spikes)
// ----------------------------------------------------------------
GLfloat godzillaVertices[] = {
    // 1. MAIN BODY
     0.45f,  0.62f,  0.0f,  // Top-Left
     0.65f,  0.55f,  0.0f,  // Top-Right
     0.7f,  -0.5f,  0.0f,  // Bottom-Right
     0.45f,  -0.5f,  0.0f,  // Bottom-Left

     // 2. HEAD
      0.4f,  0.63f,  0.0f,  // Top-Left
      0.6f,  0.6f,  0.0f,  // Top-Right
      0.6f,  0.4f,  0.0f,  // Bottom-Right
      0.4f,  0.4f,  0.0f,  // Bottom-Left

      // 3. EYE
       0.47f,  0.55f,  0.0f,  // Top-Left
       0.52f,  0.59f,  0.0f,  // Top-Right
       0.52f,  0.53f,  0.0f,  // Bottom-Right
       0.47f,  0.53f,  0.0f,  // Bottom-Left

       // 4. BACK TRIANGLE
        0.65f,  0.4f,  0.0f,  // Top-Left
        0.65f,  -0.5f,  0.0f,  // Top-Right
        1.0f,  -0.5f,  0.0f,  // Bottom-Right

        // 5. LEG
         0.65f,  -0.1f,  0.0f,  // peak
         0.8f,  -0.2f,  0.0f,  // Top-Right
         0.75f,  -0.55f,  0.0f,  // Bottom-Right
         0.5f,  -0.55f,  0.0f,  // Bottom-Left
         0.5f,  -0.3f,  0.0f,  // Top-left

         // 6. ARM
          0.5f,  0.25f,  0.0f,  // 1 ledt top
          0.6f,  0.25f,  0.0f,  // 1 right top
          0.6f,  0.1f,  0.0f,  // 1 right bottom
          0.4f,  0.1f,  0.0f,  // 1 left bottom
          0.45f,  0.2f,  0.0f,  // 1 left middle
          0.5f,  0.2f,  0.0f,  // 1 right middle
};

GLfloat godzillaColors[] = {
    // 1. MAIN BODY
    0.25f, 0.2f, 0.25f,   // Top-Left
    0.25f, 0.2f, 0.75f,   // Top-Right
    0.25f, 0.2f, 0.25f,   // Bottom-Right
    0.25f, 0.2f, 0.25f,   // Bottom-Left

    // 2. HEAD
    0.25f, 0.2f, 0.25f,   // Top-Left
    0.25f, 0.2f, 0.75f,   // Top-Right
    0.25f, 0.2f, 0.25f,   // Bottom-Right
    0.25f, 0.2f, 0.25f,   // Bottom-Left

    // 3. EYE
    1.0f, 1.0f, 1.0f,     // Top-Left
    1.0f, 1.0f, 1.0f,     // Top-Right
    1.0f, 1.0f, 1.0f,     // Bottom-Right
    1.0f, 1.0f, 1.0f,     // Bottom-Left

    // 4. BACK TRIANGLE
    0.25f, 0.2f, 0.65f,   // Top-Left
    0.25f, 0.2f, 0.25f,   // Top-Right
    0.25f, 0.2f, 0.75f,   // Bottom-Right

    // 5. LEG
    0.25f, 0.2f, 0.65f,   // Vertex 1
    0.25f, 0.2f, 0.55f,   // Vertex 2
    0.25f, 0.2f, 0.45f,   // Vertex 3
    0.25f, 0.2f, 0.25f,   // Vertex 4
    0.25f, 0.2f, 0.25f,   // Vertex 5

    // 6. ARM
    0.25f, 0.2f, 0.35f,   // Vertex 1
    0.25f, 0.2f, 0.75f,   // Vertex 2
    0.25f, 0.2f, 0.35f,   // Vertex 3
    0.25f, 0.2f, 0.25f,   // Vertex 4
    0.25f, 0.2f, 0.25f,   // Vertex 5
    0.0f, 0.0f, 0.0f,     // Vertex 6
};

void displayGodzilla() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, godzillaVertices);
    glColorPointer(3, GL_FLOAT, 0, godzillaColors);

    glDrawArrays(GL_QUADS, 0, 12);
    glDrawArrays(GL_TRIANGLES, 12, 3);
    glDrawArrays(GL_POLYGON, 15, 5);
    glDrawArrays(GL_POLYGON, 20, 6);

    // Draw Spikes
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    float startX = 0.65f, startY = 0.4f;
    float endX = 1.0f, endY = -0.5f;
    int numSpikes = 6;

    GLfloat neonTipColor[] = { 0.7f, 0.85f, 1.0f };
    GLfloat neonBaseColor[] = { 0.1f, 0.2f, 0.9f };

    for (int i = 0; i < numSpikes; i++) {
        float t = (float)i / (float)numSpikes;
        float t_next = (float)(i + 0.75) / (float)numSpikes;

        float baseX = startX + (endX - startX) * t;
        float baseY = startY + (endY - startY) * t;
        float nextBaseX = startX + (endX - startX) * t_next;
        float nextBaseY = startY + (endY - startY) * t_next;

        float size = 0.32f * (1.0f - (t * 0.3f));
        float tipX = (baseX + nextBaseX) / 2.0f + 0.08f;
        float tipY = (baseY + nextBaseY) / 2.0f + size;

        glBegin(GL_TRIANGLE_FAN);
        glColor3fv(neonTipColor);
        glVertex3f(tipX, tipY, 0.0f);
        glColor3fv(neonBaseColor);
        glVertex3f(baseX, baseY, 0.0f);
        glColor3fv(neonBaseColor);
        glVertex3f(nextBaseX, nextBaseY, 0.0f);
        glEnd();
    }
}

// ----------------------------------------------------------------
// BOY OBJECT
// ----------------------------------------------------------------
GLfloat boyVertices[] = {
    // 1. HEAD
    -0.025f,  0.07f, 0.0f,  // Top-Left
     0.025f,  0.07f, 0.0f,  // Top-Right
     0.025f, -0.01f, 0.0f,  // Bottom-Right
    -0.025f, -0.01f, 0.0f,  // Bottom-Left
    // 2. BODY
    -0.03f,  0.0f,  0.0f,   // Top-Left
     0.03f,  0.0f,  0.0f,   // Top-Right
     0.03f, -0.12f, 0.0f,   // Bottom-Right
    -0.03f, -0.12f, 0.0f,   // Bottom-Left
    // 3. LEFT ARM
    -0.065f,  0.0f,  0.0f,  // Top-Left
    -0.03f,   0.0f,  0.0f,  // Top-Right
    -0.03f,  -0.08f, 0.0f,  // Bottom-Right
    -0.065f, -0.08f, 0.0f,  // Bottom-Left
    // 4. RIGHT ARM
     0.03f,  0.0f,  0.0f,   // Top-Left
     0.065f, 0.0f,  0.0f,   // Top-Right
     0.065f, -0.08f, 0.0f,  // Bottom-Right
     0.03f, -0.08f, 0.0f,   // Bottom-Left
     // 5. Left leg
     -0.03f,  -0.12f, 0.0f,  // Top-Left
     -0.0f,   -0.12f, 0.0f,  // Top-Right
     -0.0f,   -0.25f, 0.0f,  // Bottom-Right
     -0.03f,  -0.25f, 0.0f,  // Bottom-Left
     // 6. Right leg
      0.0f,  -0.12f, 0.0f,   // Top-Left
      0.03f, -0.12f, 0.0f,   // Top-Right
      0.03f, -0.25f, 0.0f,   // Bottom-Right
      0.0f,  -0.25f, 0.0f,   // Bottom-Left
};

GLfloat boyColors[] = {
    // 1. HEAD (Skin)
    1.0f, 0.8f, 0.6f, 1.0f, 0.8f, 0.6f, 1.0f, 0.8f, 0.6f, 1.0f, 0.8f, 0.6f,
    // 2. BODY
    1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f,
    // 3. LEFT ARM
    1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f,
    // 4. RIGHT ARM
    1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f, 1.00f, 0.60f, 0.00f,
    // 5. LEFT LEG
    0.8f, 0.8f, 0.75f, 0.8f, 0.8f, 0.75f, 0.8f, 0.8f, 0.75f, 0.8f, 0.8f, 0.75f,
    // 6. RIGHT LEG
    0.8f, 0.8f, 0.75f, 0.8f, 0.8f, 0.75f, 0.8f, 0.8f, 0.75f, 0.8f, 0.8f, 0.75f,
};

void initVBOs() {
    glGenBuffers(2, boyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boyVertices), boyVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boyColors), boyColors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void displayBoy() {
    glPushMatrix();
    glTranslatef(0.0f, -0.40f, 0.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[1]);
    glColorPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_QUADS, 0, 32);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glPopMatrix();
}

void displayBirds() {
    glColor3fv(BIRD_COLOR);
    GLfloat flockCenterX = 0.6f;
    GLfloat flockCenterY = 0.8f;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < NUM_BIRDS; ++i) {
        GLfloat xOffset = birdOffsets[i * 2];
        GLfloat yOffset = birdOffsets[i * 2 + 1];
        GLfloat birdX = flockCenterX + xOffset;
        GLfloat birdY = flockCenterY + yOffset;
        glVertex3f(birdX - BIRD_SCALE * 1.5f, birdY, 0.0f);
        glVertex3f(birdX, birdY + BIRD_SCALE, 0.0f);
        glVertex3f(birdX - BIRD_SCALE * 0.5f, birdY + BIRD_SCALE * 0.5f, 0.0f);
        glVertex3f(birdX + BIRD_SCALE * 1.5f, birdY, 0.0f);
        glVertex3f(birdX, birdY + BIRD_SCALE, 0.0f);
        glVertex3f(birdX + BIRD_SCALE * 0.5f, birdY + BIRD_SCALE * 0.5f, 0.0f);
    }
    glEnd();
}

// ----------------------------------------------------------------
// ATOMIC BREATH
// ----------------------------------------------------------------
GLfloat fireVertices[] = {
    // 1. START POINT
    0.4f,   0.4f,   0.0f,   // Center/Base
    // 2. MOUTH FLARING
    0.4f,   0.5f,   0.0f,   // Top corner
    // 3. THE JAGGED BEAM
    -0.50f, 0.75f,  0.0f,   // Far Left
    -0.35f, 0.55f,  0.0f,   // Mid Left
    -0.70f, 0.40f,  0.0f,   // Furthest Left
    -0.35f, 0.35f,  0.0f,   // Mid Left
    // 4. BOTTOM FLARE
    -0.50f, 0.20f,  0.0f,   // Far Left
};

GLfloat fireColors[] = {
    0.9f, 1.0f, 1.0f,       // White-Hot Core
    0.1f, 0.2f, 0.9f,       // Darker Blue
    0.2f, 0.8f, 1.0f,       // Bright Neon Cyan
    0.1f, 0.2f, 0.9f,       // Deep Blue
    0.1f, 0.2f, 0.9f,       // Deep Blue
    0.2f, 0.8f, 1.0f,       // Bright Neon Cyan
    0.1f, 0.2f, 0.9f,       // Deep Blue
};

void displayFire() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, fireVertices);
    glColorPointer(3, GL_FLOAT, 0, fireColors);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_POLYGON, 0, 7);
    glDisable(GL_BLEND);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}