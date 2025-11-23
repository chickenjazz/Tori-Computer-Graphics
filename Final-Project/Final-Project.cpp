#include <GL/glew.h>
#include <GL/glut.h>
#include<cmath>
#include <iostream>

using namespace std;

// PROTOTYPES

void displayToriGate();
void displayGodzilla();
void displayBoy();
void initVBOs();
void displayBirds();

const int NUM_BIRDS = 15;
const GLfloat BIRD_COLOR[] = { 0.0f, 0.0f, 0.0f }; // Black Birds
const GLfloat BIRD_SCALE = 0.015f; // Size of each bird
GLuint boyVBO[2];
GLfloat birdOffsets[NUM_BIRDS * 2]; // Storage for static X and Y offsets


// ----------------------------------------------------------------
// DISPLAY CALLBACK
// ----------------------------------------------------------------
void Display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Reset transformations
    glLoadIdentity();
    displayToriGate();
    displayGodzilla();
    displayBoy();
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

    // Set background color to White
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    initVBOs();

    //Birds call function
    GLfloat flockSpreadX = 0.25f;
    GLfloat flockSpreadY = 0.2f;

    for (int i = 0; i < NUM_BIRDS; ++i) {
        // Calculate random offsets only once at program startup
        GLfloat xOffset = ((float)rand() / RAND_MAX * flockSpreadX) - (flockSpreadX / 2.0f);
        GLfloat yOffset = ((float)rand() / RAND_MAX * flockSpreadY) - (flockSpreadY / 2.0f);

        birdOffsets[i * 2] = xOffset;     // Store X offset
        birdOffsets[i * 2 + 1] = yOffset; // Store Y offset
    }
    glutDisplayFunc(Display);
    glutMainLoop();

    return 0;
}



// ----------------------------------------------------------------
// DATA SECTION
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
     -0.28f,  0.1f,  0.0f,  // Top-Left
      0.28f,  0.1f,  0.0f,  // Top-Right
      0.26f, 0.02f,  0.0f,  // Bottom-Right
     -0.26f, 0.02f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 6. MIDDLE STRUT (Gakuzuka)
     // ---------------------------------------------------------
     -0.05f,  0.19f,  0.0f,  // Top-Left
      0.05f,  0.19f,  0.0f,  // Top-Right
      0.05f, 0.1f,  0.0f,  // Bottom-Right
     -0.05f, 0.1f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
    // 9. UPPER CONNECTOR RED BEAM LEFT
    // ---------------------------------------------------------
    -0.3f,  -0.18f,  0.0f,  // Top-Left
     -0.13f,  -0.12f,  0.0f,  // Top-Right
     -0.13f,  -0.24f,  0.0f,  // Bottom-Right
    -0.3f,  -0.3f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
   // 10. LOWER RED BEAM LEFT
   // ---------------------------------------------------------
   -0.3f,  -0.43f,  0.0f,  // Top-Left
   -0.13f,  -0.37f,  0.0f,  // Top-Right
   -0.13f,  -0.49f,  0.0f,  // Bottom-Right
   -0.3f,  -0.55f,  0.0f,  // Bottom-Left

   // ---------------------------------------------------------
    // 11. UPPER CONNECTOR RED BEAM RIGHT
    // ---------------------------------------------------------
    0.3f,  -0.18f,  0.0f,  // Top-Left
     0.13f,  -0.12f,  0.0f,  // Top-Right
     0.13f,  -0.24f,  0.0f,  // Bottom-Right
    0.3f,  -0.3f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
   // 12. LOWER RED BEAM RIGHT
   // ---------------------------------------------------------
   0.3f,  -0.43f,  0.0f,  // Top-Left
   0.13f,  -0.37f,  0.0f,  // Top-Right
   0.13f,  -0.49f,  0.0f,  // Bottom-Right
    0.3f,  -0.55f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 7. SMALL LEFT PILLAR 
    // ---------------------------------------------------------
    -0.3f,   -0.08f,  0.0f,  // Top-Left
    -0.23f,  -0.08f,  0.0f,  // Top-Right
    -0.23f, -0.7f,   0.0f,  // Bottom-Right
    -0.3f,  -0.7f,   0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 8. SMALL RIGHT PILLAR 
    // ---------------------------------------------------------
     0.23f,  -0.08f,  0.0f,  // Top-Left
     0.3f,   -0.08f,  0.0f,  // Top-Right
     0.3f,  -0.7f,   0.0f,  // Bottom-Right
     0.23f, -0.7f,   0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 13. Pillar detail lower left
    // ---------------------------------------------------------
    -0.22f,  0.13f,  0.0f,  // Top-Left
     -0.11f,  0.13f,  0.0f,  // Top-Right
     -0.13f,  0.09f,  0.0f,  // Bottom-Right
    -0.2f,  0.09f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 14. Pillar detail lower right
    // ---------------------------------------------------------
         0.22f, 0.13f, 0.0f,  // Top-Left
         0.11f, 0.13f, 0.0f,  // Top-Right
         0.13f, 0.09f, 0.0f,  // Bottom-Right
         0.2f, 0.09f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 15. Pillar detail upper left
    // ---------------------------------------------------------
         -0.21f, 0.2f, 0.0f,  // Top-Left
         -0.12f, 0.2f, 0.0f,  // Top-Right
         -0.12f, 0.16f, 0.0f,  // Bottom-Right
         -0.21f, 0.16f, 0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 16. Pillar detail upper right
     // ---------------------------------------------------------
        0.21f, 0.2f, 0.0f,  // Top-Left
        0.12f, 0.2f, 0.0f,  // Top-Right
        0.12f, 0.16f, 0.0f,  // Bottom-Right
        0.21f, 0.16f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 17. left side pillar roof
    // ---------------------------------------------------------
         -0.32f, -0.1f, 0.0f,  // Top-Left
         -0.21f, -0.1f, 0.0f,  // Top-Right
         -0.23f, -0.06f, 0.0f,  // Bottom-Right
         -0.3f, -0.06f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 17. right side pillar roof
    // ---------------------------------------------------------
         0.32f, -0.1f, 0.0f,  // Top-Left
         0.21f, -0.1f, 0.0f,  // Top-Right
         0.23f, -0.06f, 0.0f,  // Bottom-Right
         0.3f, -0.06f, 0.0f,  // Bottom-Left

         // ---------------------------------------------------------
    // 1. TOP BLACK ROOF (Kasagi)
    // ---------------------------------------------------------
          -0.39f, 0.3f, 0.0f,  // Top-Left
          0.39f, 0.3f, 0.0f,  // Top-Right
          0.37f, 0.25f, 0.0f,  // Bottom-Right
          -0.37f, 0.25f, 0.0f,  // Bottom-Left

          // ---------------------------------------------------------
          // 2. UPPER RED BEAM (Shimaki)
          // ---------------------------------------------------------
          -0.35f, 0.25f, 0.0f,  // Top-Left
          0.35f, 0.25f, 0.0f,  // Top-Right
          0.33f, 0.19f, 0.0f,  // Bottom-Right
          -0.33f, 0.19f, 0.0f,  // Bottom-Left



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
// BACKGROUND GRADIENT DATA
// ----------------------------------------------------------------
GLfloat backgroundVertices[] = {
    // Top-Left
    -1.0f,  1.0f, 0.0f,
    // Top-Right
     1.0f,  1.0f, 0.0f,
     // Bottom-Right
      1.0f, -1.0f, 0.0f,
      // Bottom-Left
      -1.0f, -1.0f, 0.0f
};

GLfloat backgroundColors[] = {
    // Top-Left (Sky Blue)
    0.4f, 0.6f, 1.0f,
    // Top-Right (Sky Blue)
    0.4f, 0.6f, 1.0f,
    // Bottom-Right (Snow White)
    1.0f, 1.0f, 1.0f,
    // Bottom-Left (Snow White)
    1.0f, 1.0f, 1.0f
};
GLfloat snowFieldHeight = -0.60f; // This value will be used for the transition.
GLfloat seaLevel = -0.35f; // Adjust this value to raise/lower the sea line
GLfloat darkSkyLevel = 0.6f; // The top Y-coordinate is 1.0f, the dark sky extend down to 0.6f.


void displayToriGate() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // 1. Draw the SINGLE, Seamless Sky Gradient (Dark Top to Light Horizon) 
    GLfloat skyVertices[] = {
        // Vertices span from Y=1.0f down to Y=seaLevel
        -1.0f,  1.0f, 0.0f,      // Top-Left 
         1.0f,  1.0f, 0.0f,      // Top-Right
         1.0f,  seaLevel, 0.0f,  // Bottom boundary (Horizon)
        -1.0f,  seaLevel, 0.0f   // Bottom boundary (Horizon)
    };
    GLfloat skyColors[] = {
        // TOP: Darkest Blue
        0.1f, 0.3f, 0.7f,
        0.1f, 0.3f, 0.7f,
        // BOTTOM: Lightest Blue (Horizon)
        0.6f, 0.7f, 0.95f, 
        0.6f, 0.7f, 0.95f   
    };
    glVertexPointer(3, GL_FLOAT, 0, skyVertices);
    glColorPointer(3, GL_FLOAT, 0, skyColors);
    glDrawArrays(GL_QUADS, 0, 4);

    // --- 2. Draw the Sea Gradient (Sea Horizon to Snow Horizon) ---
    // Note: The top of the sea quad (Y=seaLevel) starts with a darker blue,
    // creating a slight break in color from the bright sky horizon, as expected for water.
    GLfloat seaVertices[] = {
        -1.0f,  seaLevel, 0.0f,         // Top-Left (Matches sky bottom)
         1.0f,  seaLevel, 0.0f,         // Top-Right
         1.0f,  snowFieldHeight, 0.0f,  // Snow Horizon Right
        -1.0f,  snowFieldHeight, 0.0f    // Snow Horizon Left
    };
    GLfloat seaColors[] = {
        0.1f, 0.3f, 0.7f, // Top-Left (Deep Sea Blue)
        0.1f, 0.3f, 0.7f, // Top-Right (Deep Sea Blue)
        0.2f, 0.5f, 0.8f, // Snow Horizon Right
        0.2f, 0.5f, 0.8f  // Snow Horizon Left
    };
    glVertexPointer(3, GL_FLOAT, 0, seaVertices);
    glColorPointer(3, GL_FLOAT, 0, seaColors);
    glDrawArrays(GL_QUADS, 0, 4);

    // --- 3. Draw the Snowy Field (Snow Horizon to Bottom) ---
   GLfloat snowColor[] = {1.0f, 1.0f, 1.0f};
    glColor3fv(snowColor); // Set the global color state for white snow

    // Number of segments for the bumps
    int numSegments = 20; 
    GLfloat segmentWidth = 2.0f / numSegments;
    GLfloat snowBottom = -1.0f;
    
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; ++i) {
        GLfloat x = -1.0f + (i * segmentWidth);
        
        // Use a sine function to create the wavy, bumpy surface
        GLfloat yBumpy = snowFieldHeight + 0.05f * sinf(x * 5.0f); 
        
        // Vertex 1: Top, Bumpy Edge
        glVertex3f(x, yBumpy, 0.0f);
        
        // Vertex 2: Bottom, Flat Edge
        glVertex3f(x, snowBottom, 0.0f);
    }
    glEnd();

    // --- 4. Draw the Tori Gate ---
    glVertexPointer(3, GL_FLOAT, 0, toriGateVertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(GL_QUADS, 0, 72);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

GLfloat godzillaVertices[] = {
    // ---------------------------------------------------------
    // 1. MAIN BODY (Torso) - Dark Grey/Purple
    // ---------------------------------------------------------
     0.45f,  0.62f,  0.0f,  // Top-Left
     0.65f,  0.55f,  0.0f,  // Top-Right
     0.7f,  -0.5f,  0.0f,  // Bottom-Right
     0.45f,  -0.5f,  0.0f,  // Bottom-Left


     // ---------------------------------------------------------
   // 2. HEAD - Dark Grey/Purple
   // ---------------------------------------------------------
    0.4f,  0.63f,  0.0f,  // Top-Left
    0.6f,  0.6f,  0.0f,  // Top-Right
    0.6f,  0.4f,  0.0f,  // Bottom-Right
    0.4f,  0.4f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
  // 3. EYE - WHITE
  // ---------------------------------------------------------
   0.47f,  0.55f,  0.0f,  // Top-Left
   0.52f,  0.59f,  0.0f,  // Top-Right
   0.52f,  0.53f,  0.0f,  // Bottom-Right
   0.47f,  0.53f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
   // 4. BACK TRIANGLE - Dark Grey/Purple
   // ---------------------------------------------------------
    0.65f,  0.4f,  0.0f,  // Top-Left
    0.65f,  -0.5f,  0.0f,  // Top-Right
    1.0f,  -0.5f,  0.0f,  // Bottom-Right

    // ---------------------------------------------------------
    // 5. LEG - Dark Grey/Purple (pentagon)
    // ---------------------------------------------------------
     0.65f,  -0.1f,  0.0f,  // peak
     0.8f,  -0.2f,  0.0f,  // Top-Right
     0.75f,  -0.55f,  0.0f,  // Bottom-Right
     0.5f,  -0.55f,  0.0f,  // Bottom-Left
     0.5f,  -0.3f,  0.0f,  // Top-left

     // ---------------------------------------------------------
    // 5. ARM - Dark Grey/Purple (pentagon)
    // ---------------------------------------------------------
     0.5f,  0.25f,  0.0f,  // 1 ledt top
     0.6f,  0.25f,  0.0f,  // 1 right top
     0.6f,  0.1f,  0.0f,  // 1 right bottom
     0.4f,  0.1f,  0.0f,  // 1 left bottom
     0.45f,  0.2f,  0.0f,  // 1 left middle
     0.5f,  0.2f,  0.0f,  // 1 right middle

     // ---------------------------------------------------------
    // 6. BACK SPIKES (Triangle Fan)
    // ---------------------------------------------------------

    // Center of fan (root at spine)
    0.70f, 0.15f, 0.0f,     // Fan center

    // Outer spike points (ordered CCW)
    0.82f, 0.28f, 0.0f,     // Spike tip 1 (upper)
    0.90f, 0.15f, 0.0f,     // Spike tip 2 (middle)
    0.82f, 0.00f, 0.0f,     // Spike tip 3 (lower)
    0.75f, 0.05f, 0.0f,     // Spike tip 4 (base return)
   
};

GLfloat godzillaColors[] = {
    // 1. BODY (Dark Grey/Purple)
    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.75f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,

    // 2. HEAD (Dark Grey/Purple)
    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.75f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,

    // 3. EYE

    1.0f, 1.0f, 1.0f,   1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,
    // 4. BACK

    0.25f, 0.2f, 0.65f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.75f,

    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f, 0.25f, 0.2f, 0.25f,

    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f, 0.25f, 0.2f, 0.25f, 0.25f, 0.2f, 0.25f,

    // 6. BACK SPIKES (light grey)
    0.75f, 0.75f, 0.85f,
    0.85f, 0.85f, 0.95f,
    0.80f, 0.80f, 0.90f,
    0.85f, 0.85f, 0.95f,
    0.75f, 0.75f, 0.85f,


};
void displayGodzilla() {
    // -------------------------------------------------
    // 1. DRAW BODY (Existing Vertex Arrays)
    // -------------------------------------------------
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, godzillaVertices);
    glColorPointer(3, GL_FLOAT, 0, godzillaColors);

    glDrawArrays(GL_QUADS, 0, 12);
    glDrawArrays(GL_TRIANGLES, 12, 3);
    glDrawArrays(GL_POLYGON, 15, 5);
    glDrawArrays(GL_POLYGON, 20, 6);

    // -------------------------------------------------
    // 2. DRAW SPIKES (Procedural Loop)
    // -------------------------------------------------
    // Turn off arrays to use manual glBegin/glEnd
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    float startX = 0.65f, startY = 0.4f;  // Neck
    float endX = 1.0f, endY = -0.5f; // Tail

    // CHANGE 1: Increased spikes to 6 to add density at the neck
    int numSpikes = 6;

    // CHANGE 2: Define Neon Blue-Green Colors
    GLfloat neonTipColor[] = { 0.7f, 0.85f, 1.0f };
    // Deep, saturated blue for the base flame
    GLfloat neonBaseColor[] = { 0.1f, 0.2f, 0.9f };

    for (int i = 0; i < numSpikes; i++) {
        // t goes from 0.0 (neck) to near 1.0 (tail)
        float t = (float)i / (float)numSpikes;
        // Offset for the second base point of the spike
        float t_next = (float)(i + 0.75) / (float)numSpikes;

        // Interpolate positions along the back
        float baseX = startX + (endX - startX) * t;
        float baseY = startY + (endY - startY) * t;

        float nextBaseX = startX + (endX - startX) * t_next;
        float nextBaseY = startY + (endY - startY) * t_next;

        // Size calculation (slightly larger base size for neon look)
        float size = 0.32f * (1.0f - (t * 0.3f));

        // Calculate tip position (peaking right)
        float tipX = (baseX + nextBaseX) / 2.0f + 0.08f;
        float tipY = (baseY + nextBaseY) / 2.0f + size;

        glBegin(GL_TRIANGLE_FAN);
        // CHANGE 3: Apply Neon Colors
        // 1. The Tip (Center of Fan)
        glColor3fv(neonTipColor);
        glVertex3f(tipX, tipY, 0.0f);

        // 2. Base Front
        glColor3fv(neonBaseColor);
        glVertex3f(baseX, baseY, 0.0f);

        // 3. Base Back
        glColor3fv(neonBaseColor);
        glVertex3f(nextBaseX, nextBaseY, 0.0f);
        glEnd();
    }
}



GLfloat boyVertices[] = {
    // ---------------------------------------------------------
    // 1. HEAD (Square)
    // ---------------------------------------------------------
    -0.025f,  0.07f, 0.0f,  // Head Top-Left
     0.025f,  0.07f, 0.0f,  // Head Top-Right
     0.025f, -0.01f, 0.0f,  // Head Bottom-Right
    -0.025f, -0.01f, 0.0f,  // Head Bottom-Left
    // ---------------------------------------------------------
    // 2. BODY (Rectangle)
    // ---------------------------------------------------------
    -0.03f,  0.0f,  0.0f,   // Body Top-Left
     0.03f,  0.0f,  0.0f,   // Body Top-Right
     0.03f, -0.12f, 0.0f,   // Body Bottom-Right
    -0.03f, -0.12f, 0.0f,   // Body Bottom-Left
    // ---------------------------------------------------------
    // 3. LEFT ARM
    // ---------------------------------------------------------
    -0.065f,  0.0f,  0.0f,  // Left Arm Top-Left
    -0.03f,   0.0f,  0.0f,  // Left Arm Top-Right
    -0.03f,  -0.08f, 0.0f,  // Left Arm Bottom-Right
    -0.065f, -0.08f, 0.0f,  // Left Arm Bottom-Left
    // ---------------------------------------------------------
    // 4. RIGHT ARM
    // ---------------------------------------------------------
     0.03f,  0.0f,  0.0f,   // Right Arm Top-Left
     0.065f, 0.0f,  0.0f,   // Right Arm Top-Right
     0.065f, -0.08f, 0.0f,  // Right Arm Bottom-Right
     0.03f, -0.08f, 0.0f,   // Right Arm Bottom-Left
     // ---------------------------------------------------------
     
     //5. Left leg
     -0.03f,  -0.12f, 0.0f,  // Left Leg Top-Left
     -0.0f,   -0.12f, 0.0f,  // Left Leg Top-Right
     -0.0f,   -0.25f, 0.0f,  // Left Leg Bottom-Right
     -0.03f,  -0.25f, 0.0f,  // Left Leg Bottom-Left

     //6. Right leg
      0.0f,  -0.12f, 0.0f,   // Right Leg Top-Left
      0.03f, -0.12f, 0.0f,   // Right Leg Top-Right
      0.03f, -0.25f, 0.0f,   // Right Leg Bottom-Right
      0.0f,  -0.25f, 0.0f,   // Right Leg Bottom-Left

};

GLfloat boyColors[] = {
    // 1. HEAD (Skin) - 4
    1.0f, 0.8f, 0.6f,
    1.0f, 0.8f, 0.6f,
    1.0f, 0.8f, 0.6f,
    1.0f, 0.8f, 0.6f,

    // 2. BODY - 4
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,

    // 3. LEFT ARM  - 4
     1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,

    // 4. RIGHT ARM  - 4
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,
    1.00f, 0.60f, 0.00f,

    // 5. LEFT LEG (Brown Pants) - 4
    0.8f, 0.8f, 0.75f,
    0.8f, 0.8f, 0.75f,
    0.8f, 0.8f, 0.75f,
    0.8f, 0.8f, 0.75f,

    // 6. RIGHT LEG (Brown Pants) - 4
    0.8f, 0.8f, 0.75f,
    0.8f, 0.8f, 0.75f,
    0.8f, 0.8f, 0.75f,
    0.8f, 0.8f, 0.75f,
};

// ----------------------------------------------------------------
// INITIALIZE VBOs for the Boy Object
// ----------------------------------------------------------------
void initVBOs() {
    // Generate two buffer IDs (one for vertices, one for colors)
    glGenBuffers(2, boyVBO);

    // 1. Bind and buffer data for vertices
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[0]);
    // Load the boyVertices array data into the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(boyVertices), boyVertices, GL_STATIC_DRAW);

    // 2. Bind and buffer data for colors
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[1]);
    // Load the boyColors array data into the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(boyColors), boyColors, GL_STATIC_DRAW);

    // Unbind the buffer to prevent accidental modification
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


// ----------------------------------------------------------------
// DRAW THE BOY USING VBOs, CENTERED INSIDE THE TORI GATE
// ----------------------------------------------------------------
void displayBoy() {
    glPushMatrix();

    // Position the boy at the middle of the Tori gate (slightly below the middle beam)
    glTranslatef(0.0f, -0.40f, 0.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Bind vertices
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    // Bind colors
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[1]);
    glColorPointer(3, GL_FLOAT, 0, 0);

    glDrawArrays(GL_QUADS, 0, 32); 

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();
}

void displayBirds() {
    glColor3fv(BIRD_COLOR); // Set color to black

    // Coordinates for the flock placement (top right quadrant)
    GLfloat flockCenterX = 0.6f;
    GLfloat flockCenterY = 0.8f;

    glBegin(GL_TRIANGLES);
    // We will draw the more bird-like 'W' shape using two triangles per bird

    for (int i = 0; i < NUM_BIRDS; ++i){
        // i * 2 gives the index for the X offset
        GLfloat xOffset = birdOffsets[i * 2];
        // i * 2 + 1 gives the index for the Y offset
        GLfloat yOffset = birdOffsets[i * 2 + 1];

        // Base coordinates for the bird's center point
        GLfloat birdX = flockCenterX + xOffset;
        GLfloat birdY = flockCenterY + yOffset;

        // First Wing (Left)
        glVertex3f(birdX - BIRD_SCALE * 1.5f, birdY, 0.0f); // Left tip
        glVertex3f(birdX, birdY + BIRD_SCALE, 0.0f); // Peak of the 'V' / body center
        glVertex3f(birdX - BIRD_SCALE * 0.5f, birdY + BIRD_SCALE * 0.5f, 0.0f); // Inner point of left wing

        // Second Wing (Right)
        glVertex3f(birdX + BIRD_SCALE * 1.5f, birdY, 0.0f); // Right tip
        glVertex3f(birdX, birdY + BIRD_SCALE, 0.0f); // Peak of the 'V' / body center
        glVertex3f(birdX + BIRD_SCALE * 0.5f, birdY + BIRD_SCALE * 0.5f, 0.0f); // Inner point of right wing
    }
    glEnd();
}
