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

GLuint boyVBO[2];

// ----------------------------------------------------------------
// DISPLAY CALLBACK
// ----------------------------------------------------------------
void Display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Reset transformations
    glLoadIdentity();

    displayToriGate();
    //displayGodzilla();
    displayBoy();
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
    glutDisplayFunc(Display);
    glutMainLoop();

    return 0;
}

// ----------------------------------------------------------------
// DATA SECTION
// ----------------------------------------------------------------
GLfloat toriGateVertices[] = {
    // ---------------------------------------------------------
    // 1. TOP BLACK ROOF (Kasagi)
    // ---------------------------------------------------------
    -0.39f,  0.3f,  0.0f,  // Top-Left
     0.39f,  0.3f,  0.0f,  // Top-Right
     0.37f,  0.25f,  0.0f,  // Bottom-Right
    -0.37f,  0.25f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 2. UPPER RED BEAM (Shimaki)
    // ---------------------------------------------------------
    -0.35f,  0.25f,  0.0f,  // Top-Left
     0.35f,  0.25f,  0.0f,  // Top-Right
     0.33f,  0.19f,  0.0f,  // Bottom-Right
    -0.33f,  0.19f,  0.0f,  // Bottom-Left

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



};

GLfloat colors[] = {
    // 1. ROOF (Black)
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

    // 2. UPPER BEAM (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 3. LEFT PILLAR (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 4. RIGHT PILLAR (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 5. LOWER BEAM (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 6. MIDDLE STRUT (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 7. SMALL LEFT PILLAR (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 8. SMALL RIGHT PILLAR (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 9. CONNECTOR (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,
    // 10. CONNECTOR (Red)
    0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

    // 11. CONNECTOR (Red)
   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,
   // 12. CONNECTOR (Red)
   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,   0.8f, 0.1f, 0.1f,

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
    glDrawArrays(GL_QUADS, 0, 48);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

GLfloat godzillaVertices[] = {
    // ---------------------------------------------------------
    // 1. MAIN BODY (Torso) - Dark Grey/Purple
    // ---------------------------------------------------------
     0.45f,  0.6f,  0.0f,  // Top-Left
     0.7f,  0.6f,  0.0f,  // Top-Right
     0.7f,  0.0f,  0.0f,  // Bottom-Right
     0.45f,  0.0f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
   // 2. HEAD - Dark Grey/Purple
   // ---------------------------------------------------------
    0.4f,  0.63f,  0.0f,  // Top-Left
    0.6f,  0.63f,  0.0f,  // Top-Right
    0.6f,  0.4f,  0.0f,  // Bottom-Right
    0.4f,  0.4f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
   // 3. BACK (Torso) - Dark Grey/Purple
   // ---------------------------------------------------------
    0.6f,  0.55f,  0.0f,  // Top-Left
    0.7f,  0.0f,  0.0f,  // Top-Right
    0.85f,  0.0f,  0.0f,  // Bottom-Right

    // ---------------------------------------------------------
  // 4. TAIL - Dark Grey/Purple
  // ---------------------------------------------------------
   0.78f,  0.34f,  0.0f,   // Tip of tail (top)
   0.9f,   0.26f, 0.0f,   // Upper right
   0.94f,  0.0f,  0.0f,   // 
   0.84f,  0.0f,  0.0f,   // 
   0.8f,   0.1f,  0.0f    //



};

GLfloat godzillaColors[] = {
    // 1. BODY (Dark Grey/Purple)
    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,

    // 2. HEAD (Dark Grey/Purple)
    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,

    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,

    0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f, 0.25f, 0.2f, 0.25f,   0.25f, 0.2f, 0.25f,

};

void displayGodzilla() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, godzillaVertices);
    glColorPointer(3, GL_FLOAT, 0, godzillaColors);

    // Draw 36 vertices (9 parts * 4 vertices per quad)
    glDrawArrays(GL_QUADS, 0, 8);
    glDrawArrays(GL_TRIANGLES, 8, 3); //for the back

    glDrawArrays(GL_TRIANGLES, 11, 3); //tail
    glDrawArrays(GL_TRIANGLES, 12, 3); //tail


    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
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


