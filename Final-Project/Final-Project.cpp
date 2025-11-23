#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

using namespace std;

// PROTOTYPES
void displayBackground();
void displayToriGate();
void snowpile();
void displayGodzilla();
void displayFire();
void displayBoy();
void initVBOs();
void displayBirds();
void mouseCallback(int button, int state, int x, int y);
void animateGodzilla(int value);

const int NUM_BIRDS = 15;
const GLfloat BIRD_COLOR[] = { 0.0f, 0.0f, 0.0f };
const GLfloat BIRD_SCALE = 0.020f;
GLuint boyVBO[2];
GLfloat birdOffsets[NUM_BIRDS * 2];

bool isFiring = false;

// --- ANIMATION VARIABLES ---
float godzillaX = 1.0f;
float godzillaY = 0.0f;
float groundShake = 0.0f;

// Cloud Animation Variables (Different speeds for parallax effect)
float cloudOffset1 = 0.0f; // Horizon (Slowest)
float cloudOffset2 = 0.0f; // Mid (Medium)
float cloudOffset3 = 0.0f; // High (Fastest)

// ----------------------------------------------------------------
// HELPER: DRAW A CLOUD (Procedural Circles)
// ----------------------------------------------------------------
void drawCloud(float cx, float cy, float size, float offset) {
    // 1. Apply Animation Offset
    float finalX = cx + offset;

    // 2. Infinite Wrapping Logic
    // If cloud goes too far right (> 1.3), move it to far left (-1.3)
    // The screen width is 2.0 (-1 to 1), so 2.6 covers the whole width plus buffer
    while (finalX > 1.3f) {
        finalX -= 2.6f;
    }
    while (finalX < -1.3f) {
        finalX += 2.6f;
    }

    // Enable blending for soft edges
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.7f, 0.8f, 1.5f, 0.15f); // White with 90% opacity

    // Draw 4 overlapping circles (Top, Sides, and a wide Bottom base)
    float offsets[6][3] = {
        { 0.0f,  0.5f,  1.0f},   // 1. Main Top Center
        { 0.0f,  -0.1f,  0.6f},   // 1. Main Top Center
        {-0.75f, 0.0f,  0.75f},  // 2. Left Middle
        { 0.75f, 0.0f,  0.75f},  // 3. Right Middle
        {-1.5f, -0.4f, 0.3f},  // 4. Bottom Left (Creates the oblong base)
        {1.5f, -0.4f, 0.3f},  // 4. Bottom Left (Creates the oblong base)
    };

    for (int j = 0; j < 6; j++) {
        glBegin(GL_TRIANGLE_FAN);
        float xPos = finalX + (offsets[j][0] * size);
        float yPos = cy + (offsets[j][1] * size);
        float radius = size * offsets[j][2];

        glVertex2f(xPos, yPos); // Center of circle
        for (int i = 0; i <= 20; i++) {
            float angle = 2.0f * 3.14159f * i / 20;
            float dx = radius * cosf(angle);
            float dy = radius * sinf(angle);
            glVertex2f(xPos + dx, yPos + dy);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
}


// ----------------------------------------------------------------
// TIMER FUNCTION (ANIMATION)
// ----------------------------------------------------------------
void animateGodzilla(int value) {
    if (value == 0) {
        glutTimerFunc(16, animateGodzilla, 1);
    }
    else if (value == 1) {
        // --- 1. UPDATE CLOUDS (THIS WAS MISSING) ---
        cloudOffset1 += 0.00005f; // Horizon moves slow
        cloudOffset2 += 0.00008f; // Mid moves medium
        cloudOffset3 += 0.00012f; // High moves fast

        // --- 2. UPDATE GODZILLA ---
        if (godzillaX > -2.5f) {
            godzillaX -= 0.002f;
            godzillaY = 0.007f * abs(sin(godzillaX * 20.0f));
            groundShake = 0.002f * abs(sin(godzillaX * 20.0f));
        }

        // --- 3. ALWAYS LOOP ---
        glutPostRedisplay();
        glutTimerFunc(16, animateGodzilla, 1);
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
// DISPLAY BACKGROUND (Sky, Clouds, Blended Sea, Snow)
// ----------------------------------------------------------------
void displayBackground() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Settings
    GLfloat snowFieldHeight = -0.60f;
    GLfloat seaLevel = -0.25f;

    // ------------------------------------
    // 1. SKY GRADIENT
    // ------------------------------------
    GLfloat skyVertices[] = {
        -1.0f,  1.0f, 0.0f,      // Top-Left 
         1.0f,  1.0f, 0.0f,      // Top-Right
         1.0f,  seaLevel, 0.0f,  // Bottom boundary
        -1.0f,  seaLevel, 0.0f   // Bottom boundary
    };
    GLfloat skyColors[] = {
        0.1f, 0.3f, 0.7f, // Top (Dark Blue)
        0.1f, 0.3f, 0.7f,
        0.6f, 0.7f, 0.95f, // Bottom (Light Blue - Horizon)
        0.6f, 0.7f, 0.95f
    };
    glVertexPointer(3, GL_FLOAT, 0, skyVertices);
    glColorPointer(3, GL_FLOAT, 0, skyColors);
    glDrawArrays(GL_QUADS, 0, 4);

    // ------------------------------------
    // 2. CLOUDS (Procedural)
    // ------------------------------------
    // We draw these before the sea so they appear behind the horizon if low
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // LAYER 1: HORIZON (Smallest, Furthest Back)
       // Y-positions: -0.2 to 0.1 | Size: 0.04 to 0.07
    drawCloud(-0.95f, -0.15f, 0.04f, cloudOffset1);
    drawCloud(0.85f, -0.12f, 0.05f, cloudOffset1);
    drawCloud(-0.60f, -0.05f, 0.06f, cloudOffset1);
    drawCloud(0.40f, 0.00f, 0.05f, cloudOffset1);
    drawCloud(0.10f, -0.08f, 0.06f, cloudOffset1);
    drawCloud(-0.25f, 0.05f, 0.07f, cloudOffset1);
    drawCloud(0.65f, 0.08f, 0.06f, cloudOffset1);
    drawCloud(0.30f, -0.15f, 0.05f, cloudOffset1); // Low Center-Right
    drawCloud(-0.45f, 0.02f, 0.06f, cloudOffset1); // Mid Horizon Left
    drawCloud(-0.80f, -0.10f, 0.05f, cloudOffset1); // Low Far Left
    drawCloud(-0.98f, -0.05f, 0.04f, cloudOffset1); // Far edge left
    drawCloud(0.95f, 0.02f, 0.05f, cloudOffset1); // Far edge right
    drawCloud(-0.15f, -0.18f, 0.06f, cloudOffset1); // Low center
    drawCloud(-0.50f, 0.32f, 0.04f, cloudOffset1); // Mid-left horizon
    drawCloud(0.55f, -0.10f, 0.05f, cloudOffset1); // Mid-right horizon
    drawCloud(-0.90f, -0.20f, 0.04f, cloudOffset1); // Far left bottom
    drawCloud(0.92f, -0.18f, 0.05f, cloudOffset1); // Far right bottom
    drawCloud(0.75f, -0.05f, 0.04f, cloudOffset1); // Mid right
    drawCloud(-0.70f, 0.23f, 0.05f, cloudOffset1); // Mid left
    drawCloud(1.25f, 0.18f, 0.06f, cloudOffset1); // Big Far Right
    drawCloud(-1.25f, 0.13f, 0.08f, cloudOffset1); // Big Far Right
    drawCloud(1.75f, -0.06f, 0.056f, cloudOffset1); // Big Far Right
    drawCloud(-1.85f, 0.11f, 0.058f, cloudOffset1); // Big Far Right

    // LAYER 2: MID-SKY (Medium Size)
    // Y-positions: 0.2 to 0.5 | Size: 0.09 to 0.14
    drawCloud(-0.85f, 0.30f, 0.10f, cloudOffset2);
    drawCloud(0.90f, 0.35f, 0.11f, cloudOffset2);
    drawCloud(0.20f, 0.40f, 0.13f, cloudOffset2);
    drawCloud(-0.45f, 0.25f, 0.09f, cloudOffset2);
    drawCloud(0.55f, 0.50f, 0.14f, cloudOffset2);
    drawCloud(0.00f, 0.30f, 0.11f, cloudOffset2); // Center Mid
    drawCloud(-0.20f, 0.45f, 0.12f, cloudOffset2); // Mid-High Left
    drawCloud(0.70f, 0.25f, 0.10f, cloudOffset2); // Low-Mid Right
    drawCloud(-0.68f, 0.08f, 0.12f, cloudOffset2); // Low-Mid Right
    drawCloud(0.68f, 0.08f, 0.12f, cloudOffset2); // Low-Mid Right
    drawCloud(-0.65f, 0.35f, 0.11f, cloudOffset2); // Left side filler
    drawCloud(0.35f, 0.28f, 0.10f, cloudOffset2); // Right side filler
    drawCloud(1.25f, 0.24f, 0.15f, cloudOffset3); // Big Far Right
    drawCloud(-1.25f, 0.39f, 0.13f, cloudOffset2); // Big Far Right
    drawCloud(1.75f, 0.41f, 0.14f, cloudOffset2); // Big Far Right
    drawCloud(-1.85f, 0.33f, 0.11f, cloudOffset2); // Big Far Right


    // LAYER 3: HIGH SKY (Largest, Closest/Front)
    // Y-positions: 0.6 to 0.9 | Size: 0.18 to 0.25
    drawCloud(-0.60f, 0.70f, 0.18f, cloudOffset3); // Big Left
    drawCloud(0.50f, 0.80f, 0.20f, cloudOffset3); // Big Right
    drawCloud(-0.10f, 0.85f, 0.22f, cloudOffset3); // Huge Top Center
    drawCloud(0.85f, 0.75f, 0.19f, cloudOffset3); // Big Far Right
    drawCloud(-0.90f, 0.60f, 0.16f, cloudOffset3); // Big Far Left
    drawCloud(0.20f, 0.90f, 0.21f, cloudOffset3); // Huge Top Right
    drawCloud(-0.35f, 0.55f, 0.19f, cloudOffset3); // Big Mid-Left
    drawCloud(0.0f, 0.55f, 0.19f, cloudOffset3); // Big Mid-Left
    drawCloud(-0.75f, 0.80f, 0.23f, cloudOffset3); // Huge Top Far Left
    drawCloud(1.25f, 0.7f, 0.21f, cloudOffset3); // Big Far Right
    drawCloud(-1.25f, 0.65f, 0.19f, cloudOffset3); // Big Far Right

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // ------------------------------------
    // 3. SEA GRADIENT (Seamless Blend)
    // ------------------------------------
    GLfloat seaVertices[] = {
        -1.0f,  seaLevel, 0.0f,         // Top-Left 
         1.0f,  seaLevel, 0.0f,         // Top-Right
         1.0f,  snowFieldHeight, 0.0f,  // Bottom Right
        -1.0f,  snowFieldHeight, 0.0f   // Bottom Left
    };

    // UPDATED COLORS: The Top of the sea now matches the Bottom of the sky
    GLfloat seaColors[] = {
        0.5f, 0.7f, 0.95f, // Top (Light Blue - Matches Sky Horizon)
        0.5f, 0.7f, 0.95f,
        0.1f, 0.2f, 0.5f,  // Bottom (Darker Blue - Depth near shore)
        0.1f, 0.2f, 0.5f
    };
    glVertexPointer(3, GL_FLOAT, 0, seaVertices);
    glColorPointer(3, GL_FLOAT, 0, seaColors);
    glDrawArrays(GL_QUADS, 0, 4);

    // ------------------------------------
    // 4. TEXTURED SNOW FIELD
    // ------------------------------------
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    int numSegments = 60;
    GLfloat segmentWidth = 2.0f / numSegments;
    GLfloat snowBottom = -1.0f;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; ++i) {
        GLfloat x = -1.0f + (i * segmentWidth);

        // Complex Noise for rugged snow
        GLfloat yBumpy = snowFieldHeight
            + 0.05f * sinf(x * 3.5f)
            + 0.02f * cosf(x * 15.0f)
            + 0.005f * sinf(x * 50.0f);

        // Height-based shading
        float heightFactor = (yBumpy - snowFieldHeight + 0.05f) * 10.0f;
        if (heightFactor > 1.0f) heightFactor = 1.0f;
        if (heightFactor < 0.0f) heightFactor = 0.0f;

        // Surface: Mix White with Shadow Blue
        float r = 0.85f + (0.15f * heightFactor);
        float g = 0.90f + (0.10f * heightFactor);
        float b = 0.95f + (0.05f * heightFactor);

        glColor3f(r, g, b);
        glVertex3f(x, yBumpy, 0.0f);

        // Underground: Darker Grey/Blue
        glColor3f(0.7f, 0.75f, 0.85f);
        glVertex3f(x, snowBottom, 0.0f);
    }
    glEnd();
}

// ----------------------------------------------------------------
// DISPLAY CALLBACK
// ----------------------------------------------------------------
void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Apply ground shake to the background
    glPushMatrix();
    glTranslatef(0.0f, groundShake, 0.0f);
    displayBackground();

    // Draw Godzilla (He shakes independently)
    glPushMatrix();
    glTranslatef(godzillaX, godzillaY, 0.0f);
    displayGodzilla();
    if (isFiring) {
        displayFire();
    }
    glPopMatrix();

    // Gate and Boy also shake with the ground
    displayToriGate();
    snowpile();
    displayBoy();
    glPopMatrix(); // End ground shake

    // Birds fly smoothly (No shake)
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

//-----------------------------------------------------------------
// PILE OF SNOW
//-----------------------------------------------------------------
void snowpile() {
    // Enable Blending for soft edges/transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable Point Smoothing to make the GL_POINTS look like round snowballs
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    // =========================================================
    // LAYER 3: RANDOM SNOW CHUNKS (GL_POINTS)
    // =========================================================
    glPointSize(20.0f); // Big chunky snowballs
    glBegin(GL_POINTS);
    glColor4f(0.9f, 0.95f, 1.0f, 0.25f); // Almost pure white

    srand(50);

    for (int i = 0; i < 10000; i++) {
        // Random X between -1.0 and 1.0
        float rX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

        // Random Y between -1.0 and -0.6 (Snowfield area)
        float rY = -1.0f + ((float)rand() / RAND_MAX) * 0.38f;

        glVertex3f(rX, rY, 0.0f);
    }
    glEnd();

    glPointSize(18.0f); // Big chunky snowballs
    glBegin(GL_POINTS);
    glColor4f(1.0f, 1.0f, 1.0f, 0.15f); // pure white

    // Use a constant seed so they don't jitter every frame
    srand(50);

    for (int i = 0; i < 5000; i++) {
        // Random X between -1.0 and 1.0
        float rX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

        // Random Y between -1.0 and -0.6 (Snowfield area)
        float rY = -1.0f + ((float)rand() / RAND_MAX) * 0.38f;

        glVertex3f(rX, rY, 0.0f);
    }
    glEnd();

    // =========================================================
    // LAYER 2: ORIGINAL PILES (Foreground)
    // Updated to use glColor4f for blending
    // =========================================================

    // PILE 1: FAR LEFT
    glBegin(GL_POLYGON);

    glColor4f(0.85f, 0.85f, 0.95f, 0.65f); // Base
    glVertex3f(-0.95f, -0.90f, 0.0f);
    glVertex3f(-0.85f, -0.92f, 0.0f);
    glVertex3f(-0.70f, -0.90f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f, 0.65f);    // Top
    glVertex3f(-0.65f, -0.80f, 0.0f);
    glVertex3f(-0.72f, -0.70f, 0.0f);
    glVertex3f(-0.80f, -0.75f, 0.0f);
    glVertex3f(-0.88f, -0.68f, 0.0f);

    glColor4f(0.95f, 0.95f, 1.0f, 0.65f);
    glVertex3f(-0.98f, -0.80f, 0.0f);
    glEnd();

    // PILE 2: MID LEFT
    glBegin(GL_POLYGON);

    glColor4f(0.85f, 0.85f, 0.95f, 0.65f);
    glVertex3f(-0.60f, -0.85f, 0.0f);
    glVertex3f(-0.50f, -0.88f, 0.0f);
    glVertex3f(-0.35f, -0.85f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f, 0.85f);
    glVertex3f(-0.38f, -0.75f, 0.0f);
    glVertex3f(-0.42f, -0.65f, 0.0f);
    glVertex3f(-0.48f, -0.70f, 0.0f);
    glVertex3f(-0.52f, -0.62f, 0.0f);
    glVertex3f(-0.58f, -0.72f, 0.0f);
    glEnd();

    // PILE 3: CENTER LEFT
    glBegin(GL_POLYGON);

    glColor4f(0.85f, 0.85f, 0.95f, 0.65f);
    glVertex3f(-0.3f, -0.75f, 0.0f);
    glVertex3f(-0.2f, -0.78f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f, 0.85f);
    glVertex3f(-0.15f, -0.65f, 0.0f);
    glVertex3f(-0.19f, -0.68f, 0.0f);
    glVertex3f(-0.23f, -0.62f, 0.0f);
    glVertex3f(-0.27f, -0.59f, 0.0f);
    glVertex3f(-0.31f, -0.65f, 0.0f);
    glVertex3f(-0.33f, -0.70f, 0.0f);
    glEnd();

    // PILE 4: CENTER RIGHT
    glBegin(GL_POLYGON);

    glColor4f(0.85f, 0.85f, 0.95f, 0.75f);
    glVertex3f(0.15f, -0.70f, 0.0f);
    glVertex3f(0.25f, -0.72f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f, 0.55f);
    glVertex3f(0.35f, -0.70f, 0.0f);
    glVertex3f(0.45f, -0.65f, 0.0f);
    glVertex3f(0.38f, -0.57f, 0.0f);
    glVertex3f(0.30f, -0.63f, 0.0f);
    glVertex3f(0.22f, -0.6f, 0.0f);
    glVertex3f(0.15f, -0.65f, 0.0f);
    glEnd();

    // PILE 5: MID RIGHT
    glBegin(GL_POLYGON);

    glColor4f(0.85f, 0.85f, 0.95f, 0.65f);
    glVertex3f(0.50f, -0.85f, 0.0f);
    glVertex3f(0.65f, -0.85f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f, 0.85f);
    glVertex3f(0.70f, -0.75f, 0.0f);
    glVertex3f(0.62f, -0.65f, 0.0f);
    glVertex3f(0.58f, -0.70f, 0.0f);
    glVertex3f(0.55f, -0.68f, 0.0f);
    glVertex3f(0.52f, -0.75f, 0.0f);
    glVertex3f(0.48f, -0.80f, 0.0f);
    glEnd();

    // PILE 6: FAR RIGHT
    glBegin(GL_POLYGON);
    glColor4f(0.85f, 0.85f, 0.95f, 0.85f);
    glVertex3f(0.75f, -0.90f, 0.0f);
    glVertex3f(0.95f, -0.90f, 0.0f);

    glColor4f(1.0f, 1.0f, 1.0f, 0.55f);
    glVertex3f(0.98f, -0.80f, 0.0f);
    glVertex3f(0.92f, -0.72f, 0.0f);
    glVertex3f(0.88f, -0.75f, 0.0f);
    glVertex3f(0.85f, -0.70f, 0.0f);
    glVertex3f(0.80f, -0.78f, 0.0f);
    glVertex3f(0.72f, -0.85f, 0.0f);
    glEnd();


    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glFlush();
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
// ----------------------------------------------------------------
// BOY OBJECT (Layered Hair, Thinner Body, Shorter Legs - 52 Vertices)
// ----------------------------------------------------------------
GLfloat boyVertices[] = {
    // 1. HEAD (Skin)
    -0.035f,  0.03f, 0.0f,  // Top-Left 
     0.035f,  0.03f, 0.0f,  // Top-Right
     0.035f, -0.04f, 0.0f,  // Bottom-Right
    -0.035f, -0.04f, 0.0f,  // Bottom-Left

    // 2. HAIR TOP VOLUME - Main block
    -0.045f,  0.08f, 0.0f,  // Top-Left 
     0.045f,  0.08f, 0.0f,  // Top-Right
     0.045f,  0.03f, 0.0f,  // Bottom-Right (Aligns with head top)
    -0.045f,  0.03f, 0.0f,  // Bottom-Left

    // 3. HAIR BACK LAYER
    -0.040f,  0.04f, 0.0f,  // Top-Left (Slightly overlaps main hair)
     0.040f,  0.04f, 0.0f,  // Top-Right
     0.025f, -0.01f, 0.0f,  // Bottom-Right (Tapered in towards the center)
    -0.025f, -0.01f, 0.0f,  // Bottom-Left

    // 4. NECK (Skin)
    -0.015f, -0.04f, 0.0f,  // Top-Left
     0.015f, -0.04f, 0.0f,  // Top-Right
     0.015f, -0.06f, 0.0f,  // Bottom-Right
    -0.015f, -0.06f, 0.0f,  // Bottom-Left

    // 5. BODY
    -0.04f,  -0.06f, 0.0f,  // Top-Left 
     0.04f,  -0.06f, 0.0f,  // Top-Right
     0.04f,  -0.18f, 0.0f,  // Bottom-Right 
    -0.04f,  -0.18f, 0.0f,  // Bottom-Left

    // 6. LEFT ARM 
    -0.07f,  -0.06f, 0.0f,  // Top-Left 
    -0.04f,  -0.06f, 0.0f,  // Top-Right 
    -0.04f,  -0.13f, 0.0f,  // Bottom-Right
    -0.07f,  -0.13f, 0.0f,  // Bottom-Left

    // 7. RIGHT ARM 
     0.04f,  -0.06f, 0.0f,  // Top-Left 
     0.07f,  -0.06f, 0.0f,  // Top-Right 
     0.07f,  -0.13f, 0.0f,  // Bottom-Right
     0.04f,  -0.13f, 0.0f,  // Bottom-Left

     // 8. LEFT HAND (Skin)
     -0.07f,  -0.13f, 0.0f,  // Top-Left
     -0.05f,  -0.13f, 0.0f,  // Top-Right
     -0.05f,  -0.15f, 0.0f,  // Bottom-Right
     -0.07f,  -0.15f, 0.0f,  // Bottom-Left

     // 9. RIGHT HAND (Skin)
      0.05f,  -0.13f, 0.0f,  // Top-Left
      0.07f,  -0.13f, 0.0f,  // Top-Right
      0.07f,  -0.15f, 0.0f,  // Bottom-Right
      0.05f,  -0.15f, 0.0f,  // Bottom-Left

     // 10. LEFT LEG (Shorter)
     -0.04f, -0.18f, 0.0f, // Top-Left 
     -0.01f, -0.18f, 0.0f, // Top-Right 
     -0.01f, -0.28f, 0.0f, // Bottom-Right 
     -0.04f, -0.28f, 0.0f, // Bottom-Left

     // 11. RIGHT LEG (Shorter)
      0.01f, -0.18f, 0.0f, // Top-Left 
      0.04f, -0.18f, 0.0f, // Top-Right 
      0.04f, -0.28f, 0.0f, // Bottom-Right
      0.01f, -0.28f, 0.0f, // Bottom-Left

     // 12. LEFT SHOE 
     -0.04f, -0.28f, 0.0f, // Top-Left
     -0.01f, -0.28f, 0.0f, // Top-Right
     -0.01f, -0.30f, 0.0f, // Bottom-Right
     -0.05f, -0.30f, 0.0f, // Bottom-Left

     // 13. RIGHT SHOE 
      0.01f, -0.28f, 0.0f, // Top-Left
      0.04f, -0.28f, 0.0f, // Top-Right
      0.05f, -0.30f, 0.0f, // Bottom-Right
      0.01f, -0.30f, 0.0f, // Bottom-Left
};

GLfloat boyColors[] = {
    // 1. HEAD (Skin Tone)
    0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f,
    // 2. HAIR (TOP VOLUME - Dark Purple/Brown)
    0.2f, 0.1f, 0.2f, 0.2f, 0.1f, 0.2f, 0.2f, 0.1f, 0.2f, 0.2f, 0.1f, 0.2f,
    // 3. HAIR (BACK LAYER - Same Dark Hair Color) 
    0.2f, 0.1f, 0.2f, 0.2f, 0.1f, 0.2f, 0.2f, 0.1f, 0.2f, 0.2f, 0.1f, 0.2f,
    // 4. NECK (Skin Tone)
    0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f,
    // 5. BODY (Maroon/Reddish-Purple Shirt)
    0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f,
    // 6. LEFT ARM (Maroon/Reddish-Purple Shirt)
    0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f,
    // 7. RIGHT ARM (Maroon/Reddish-Purple Shirt)
    0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f, 0.69f, 0.22f, 0.35f,
    // 8. LEFT HAND (Skin Tone)
    0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f,
    // 9. RIGHT HAND (Skin Tone)
    0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f, 0.96f, 0.73f, 0.58f,
    // 10. LEFT LEG (Blue Jeans)
    0.3f, 0.45f, 0.6f, 0.3f, 0.45f, 0.6f, 0.3f, 0.45f, 0.6f, 0.3f, 0.45f, 0.6f,
    // 11. RIGHT LEG (Blue Jeans)
    0.3f, 0.45f, 0.6f, 0.3f, 0.45f, 0.6f, 0.3f, 0.45f, 0.6f, 0.3f, 0.45f, 0.6f,
    // 12. LEFT SHOE (Dark Red/Purple)
    0.4f, 0.15f, 0.2f, 0.4f, 0.15f, 0.2f, 0.4f, 0.15f, 0.2f, 0.4f, 0.15f, 0.2f,
    // 13. RIGHT SHOE (Dark Red/Purple)
    0.4f, 0.15f, 0.2f, 0.4f, 0.15f, 0.2f, 0.4f, 0.15f, 0.2f, 0.4f, 0.15f, 0.2f,
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
    glDrawArrays(GL_QUADS, 0, 52);
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