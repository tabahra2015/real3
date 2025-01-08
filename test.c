#include <GL/glut.h>
#include <math.h> // Include the math header for sin and cos functions

// Function to initialize the OpenGL environment
void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0); // Set background color to white
    glColor3f(0.0, 0.0, 0.0); // Set drawing color to black
}

// Function to draw the shapes
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    // Draw Counter Espionage Agency Members (Circles)
    glColor3f(0.0, 0.0, 1.0); // Blue color for agency members
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 10) {
        float angle = i * 3.14159 / 180; // Convert degrees to radians
        glVertex2f(0.2 * cos(angle) + 0.5, 0.2 * sin(angle) + 0.5); // Circle position
    }
    glEnd();

    // Draw Resistance Group (Square)
    glColor3f(0.0, 1.0, 0.0); // Green color for resistance group
    glBegin(GL_QUADS);
    glVertex2f(0.2, 0.2); // Bottom left
    glVertex2f(0.2, 0.4); // Top left
    glVertex2f(0.4, 0.4); // Top right
    glVertex2f(0.4, 0.2); // Bottom right
    glEnd();

    // Draw Spy (Triangle)
    glColor3f(1.0, 0.0, 0.0); // Red color for spy
    glBegin(GL_TRIANGLES);
    glVertex2f(0.3, 0.3); // Top vertex
    glVertex2f(0.25, 0.2); // Bottom left vertex
    glVertex2f(0.35, 0.2); // Bottom right vertex
    glEnd();

    // Draw Civilian (Star)
    glColor3f(1.0, 1.0, 0.0); // Yellow color for civilian
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.6, 0.6); // Center of the star
    for (int i = 0; i < 360; i += 72) { // Create a star shape
        float angle = i * 3.14159 / 180;
        glVertex2f(0.05 * cos(angle) + 0.6, 0.05 * sin(angle) + 0.6);
    }
    glEnd();

    glFlush(); // Render now
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv); // Initialize GLUT
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); // Set display mode
    glutInitWindowSize(800, 600); // Set window size
    glutInitWindowPosition(100, 100); // Set window position
    glutCreateWindow("Counter Espionage Agency Simulation"); // Create window
    init(); // Initialize OpenGL
    glutDisplayFunc(display); // Register display function
    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}