#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

// Function to initialize OpenGL settings
void InitializeGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    // Optionally enable depth testing if you plan to draw 3D objects
    // glEnable(GL_DEPTH_TEST);
}

// The display callback function for rendering
void Display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth if depth testing is enabled

    // --- Drawing a Red Triangle ---
    glMatrixMode(GL_PROJECTION); // Set projection matrix mode
    glLoadIdentity();            // Reset the projection matrix
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0); // Define a 2D orthographic projection (left, right, bottom, top, near, far)

    glMatrixMode(GL_MODELVIEW);  // Set modelview matrix mode
    glLoadIdentity();            // Reset the modelview matrix

    glColor3f(1.0f, 0.0f, 0.0f); // Set the drawing color to red (RGB)

    glBegin(GL_TRIANGLES);       // Start drawing triangles
    glVertex2f(0.0f, 0.5f);  // Top vertex
    glVertex2f(-0.5f, -0.5f); // Bottom-left vertex
    glVertex2f(0.5f, -0.5f); // Bottom-right vertex
    glEnd();                     // End drawing triangles
    // ----------------------------

    glutSwapBuffers(); // Swap the front and back buffers to display the rendered image
}

// The reshape callback function (called when the window is resized)
void Reshape(int width, int height) {
    glViewport(0, 0, width, height); // Set the viewport to cover the new window area
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("GLUT & GLEW OpenGL Example - Red Triangle");

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    InitializeGL();
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);

    glutMainLoop();

    return 0;
}