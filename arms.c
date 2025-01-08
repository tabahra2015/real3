#include <GL/glut.h>

void drawSquare(float x, float y, float size, const char *label) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();

    glRasterPos2f(x + size / 4, y + size / 2);
    for (const char *c = label; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawStickFigure(float x, float y, const char *label) {
    // Head
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 360; i += 30) {
        float theta = i * 3.14159 / 180;
        glVertex2f(x + 0.05 * cos(theta), y + 0.05 * sin(theta));
    }
    glEnd();

    // Body
    glBegin(GL_LINES);
    glVertex2f(x, y - 0.05);
    glVertex2f(x, y - 0.15);
    glEnd();

    // Arms
    glBegin(GL_LINES);
    glVertex2f(x - 0.05, y - 0.1);
    glVertex2f(x + 0.05, y - 0.1);
    glEnd();

    // Legs
    glBegin(GL_LINES);
    glVertex2f(x, y - 0.15);
    glVertex2f(x - 0.05, y - 0.2);
    glVertex2f(x, y - 0.15);
    glVertex2f(x + 0.05, y - 0.2);
    glEnd();

    // Label
    glRasterPos2f(x - 0.05, y - 0.25);
    for (const char *c = label; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void drawArrow(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    // Arrowhead
    float angle = atan2(y2 - y1, x2 - x1);
    float headLength = 0.05;
    float headAngle = 3.14159 / 6;

    glBegin(GL_TRIANGLES);
    glVertex2f(x2, y2);
    glVertex2f(x2 - headLength * cos(angle - headAngle), y2 - headLength * sin(angle - headAngle));
    glVertex2f(x2 - headLength * cos(angle + headAngle), y2 - headLength * sin(angle + headAngle));
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw squares
    glColor3f(0.0, 0.0, 1.0);
    drawSquare(-0.8, 0.8, 0.2, "Agency");
    drawSquare(0.6, 0.8, 0.2, "Enemy");

    // Draw stick figures
    glColor3f(0.0, 1.0, 0.0);
    drawStickFigure(-0.8, -0.5, "Citizens");
    drawStickFigure(-0.3, 0.4, "Resistance");
    drawStickFigure(0.5, 0.4, "Spy");

    // Draw arrows
    glColor3f(1.0, 0.0, 0.0);
    drawArrow(-0.8, -0.3, -0.7, 0.8); // Citizens -> Agency
    drawArrow(-0.3, 0.3, -0.6, 0.8);  // Resistance -> Agency
    drawArrow(-0.3, 0.3, 0.5, 0.4);  // Resistance -> Spy
    drawArrow(0.5, 0.4, 0.7, 0.8);   // Spy -> Enemy

    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Diagram");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
