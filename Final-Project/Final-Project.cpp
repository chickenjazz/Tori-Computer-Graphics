#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

using namespace std;

//PROTOTYPES
void displayToriGate();
//INITIALIZATIONS

// The display callback function for rendering
void Display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
    displayToriGate();
    glutSwapBuffers();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); 
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Manuzon - Sandiego - Computer Graphics");

    glClearColor(1, 1, 1, 1);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }

    glutDisplayFunc(Display);
    glutMainLoop();

    return 0;
}

GLfloat toriGateVertices[] = {
    // ---------------------------------------------------------
    // 1. TOP BLACK ROOF (Kasagi) - Wide rectangle at the very top
    // ---------------------------------------------------------
    // Triangle 1
    -0.6f,  0.9f, 0.0f,  // Top-Left
     0.6f,  0.9f, 0.0f,  // Top-Right
    -0.6f,  0.8f, 0.0f,  // Bottom-Left
    // Triangle 2
     0.6f,  0.9f, 0.0f,  // Top-Right
     0.6f,  0.8f, 0.0f,  // Bottom-Right
    -0.6f,  0.8f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 2. UPPER RED BEAM (Shimaki) - Just below the roof
    // ---------------------------------------------------------
    // Triangle 1
    -0.55f, 0.8f, 0.0f,  // Top-Left
     0.55f, 0.8f, 0.0f,  // Top-Right
    -0.55f, 0.65f, 0.0f, // Bottom-Left
    // Triangle 2
     0.55f, 0.8f, 0.0f,  // Top-Right
     0.55f, 0.65f, 0.0f, // Bottom-Right
    -0.55f, 0.65f, 0.0f, // Bottom-Left

    // ---------------------------------------------------------
    // 3. LEFT PILLAR (Hashira)
    // ---------------------------------------------------------
    // Triangle 1
    -0.4f,  0.65f, 0.0f, // Top-Left
    -0.3f,  0.65f, 0.0f, // Top-Right
    -0.4f, -0.8f, 0.0f,  // Bottom-Left
    // Triangle 2
    -0.3f,  0.65f, 0.0f, // Top-Right
    -0.3f, -0.8f, 0.0f,  // Bottom-Right
    -0.4f, -0.8f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 4. RIGHT PILLAR (Hashira)
    // ---------------------------------------------------------
    // Triangle 1
     0.3f,  0.65f, 0.0f, // Top-Left
     0.4f,  0.65f, 0.0f, // Top-Right
     0.3f, -0.8f, 0.0f,  // Bottom-Left
     // Triangle 2
      0.4f,  0.65f, 0.0f, // Top-Right
      0.4f, -0.8f, 0.0f,  // Bottom-Right
      0.3f, -0.8f, 0.0f,  // Bottom-Left

      // ---------------------------------------------------------
      // 5. LOWER RED BEAM (Nuki) - Connects the pillars
      // ---------------------------------------------------------
      // Triangle 1
      -0.55f, 0.45f, 0.0f, // Top-Left
       0.55f, 0.45f, 0.0f, // Top-Right
      -0.55f, 0.35f, 0.0f, // Bottom-Left
      // Triangle 2
       0.55f, 0.45f, 0.0f, // Top-Right
       0.55f, 0.35f, 0.0f, // Bottom-Right
      -0.55f, 0.35f, 0.0f, // Bottom-Left

      // ---------------------------------------------------------
      // 6. MIDDLE STRUT (Gakuzuka) - Small vertical bar in center
      // ---------------------------------------------------------
      // Triangle 1
      -0.05f, 0.65f, 0.0f, // Top-Left
       0.05f, 0.65f, 0.0f, // Top-Right
      -0.05f, 0.45f, 0.0f, // Bottom-Left
      // Triangle 2
       0.05f, 0.65f, 0.0f, // Top-Right
       0.05f, 0.45f, 0.0f, // Bottom-Right
      -0.05f, 0.45f, 0.0f  // Bottom-Left
};

GLfloat colors[] = {
    // 1. ROOF (Black) - 6 vertices
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,

    // 2. UPPER BEAM (Deep Red) - 6 vertices
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,

    // 3. LEFT PILLAR (Deep Red) - 6 vertices
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,

    // 4. RIGHT PILLAR (Deep Red) - 6 vertices
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,

    // 5. LOWER BEAM (Deep Red) - 6 vertices
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,

    // 6. CENTER STRUT (Deep Red) - 6 vertices
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,
    0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f,  0.8f, 0.1f, 0.1f
};

void displayToriGate() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(4, GL_FLOAT, 0, toriGateVertices);
    glColorPointer(4, GL_FLOAT, 0, colors);

    // Draw 36 vertices (6 parts * 2 triangles * 3 vertices)
    glDrawArrays(GL_POLYGON, 0, 36);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}