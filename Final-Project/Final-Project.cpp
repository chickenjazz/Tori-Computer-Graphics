#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

using namespace std;

// PROTOTYPES
void displayToriGate();

// ----------------------------------------------------------------
// DISPLAY CALLBACK
// ----------------------------------------------------------------
void Display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Reset transformations
    glLoadIdentity();

    displayToriGate();

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
    -0.42f,  0.32f,  0.0f,  // Top-Left
     0.42f,  0.32f,  0.0f,  // Top-Right
     0.36f,  0.25f,  0.0f,  // Bottom-Right
    -0.36f,  0.25f,  0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 2. UPPER RED BEAM (Shimaki)
    // ---------------------------------------------------------
    -0.35f,  0.25f,  0.0f,  // Top-Left
     0.35f,  0.25f,  0.0f,  // Top-Right
     0.3f,  0.19f,  0.0f,  // Bottom-Right
    -0.3f,  0.19f,  0.0f,  // Bottom-Left

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
     -0.28f,  0.08f,  0.0f,  // Top-Left
      0.28f,  0.08f,  0.0f,  // Top-Right
      0.28f, 0.0f,  0.0f,  // Bottom-Right
     -0.28f, 0.0f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 6. MIDDLE STRUT (Gakuzuka)
     // ---------------------------------------------------------
     -0.05f,  0.19f,  0.0f,  // Top-Left
      0.05f,  0.19f,  0.0f,  // Top-Right
      0.05f, 0.0f,  0.0f,  // Bottom-Right
     -0.05f, 0.0f,  0.0f,  // Bottom-Left

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

};

void displayToriGate() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, toriGateVertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    // Draw 32 vertices (8 parts * 4 vertices per quad)
    glDrawArrays(GL_QUADS, 0, 24);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}