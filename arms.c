#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>   
#include <stdbool.h>  
#include "local.h"
#define MAX_PLAYERS 100
#define MAX_TEAMS 50
bool isPaperVisible = true;    // To control the visibility of the file
float paperX = 0.2f;           // Initial X position of the file (near yellow human)
float paperY = -0.2f;          // Initial Y position of the file (same level as yellow human)
float destinationX = 0.7f;     // X position of the yellow building
float destinationY = -0.2f;    // Y position of the yellow building

bool isBlueFileVisible = true;  // Visibility for the blue file
float blueFileX = -0.2f;        // Initial X position near the blue human
float blueFileY = -0.5f;        // Initial Y position near the blue human
float blueFileDestX = -0.7f;    // X position of the Espionage Agency
float blueFileDestY = -0.6f;    // Y position of the Espionage Agency

// Function to read data from the text file
void readDataFromFile() {
    const char *filename = "tables_data.txt";

    // Acquire the semaphore
    sem_wait(file_semaphore);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "# Table 1", 9) == 0) {
            // Read teams
            // Read Table 1
            while (fgets(line, sizeof(line), file) && line[0] != '#') {
                ResistanceGroup group;
                char destiny[50];
                float percentage;

                if (sscanf(line, "%d,%49[^,],%f%%", &group.group_id, destiny, &percentage) == 3) {
                    group.spy_target_probability = percentage / 100;
                    group.group_type = (strcmp(destiny, "Alpha") == 0) ? TYPE_ALPHA : TYPE_BETA;
                    groups[(*group_count)++] = group;
                }
            }
        } else if (strncmp(line, "# Table 2:", 10) == 0) {
            // Read Table 2
            while (fgets(line, sizeof(line), file) && line[0] != '#') {
                int res_id, citizen_id, duration;

                if (sscanf(line, "%d,%d,%d minutes", &res_id, &citizen_id, &duration) == 3) {
                    members[*member_count].id = citizen_id;
                    members[*member_count].status = ACTIVE; // Set default status
                    (*member_count)++;
                }
            }
        }
    }

    fclose(file);
    // Release the semaphore
    sem_post(file_semaphore);
    renderPlayerTable();
    renderTeamTable();
}


// Function to display a single table row as text
void drawText(float x, float y, const char *text, void *font) {
        glColor3f(0.0, 0.0, 1.0); // Blue text color
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

// Function to render Player Information Table
void renderPlayerTable() {
    float y = 0.8f; // Start Y position
    drawText(-0.9f, y, "Player Information Table:", GLUT_BITMAP_HELVETICA_18); // Title
    y -= 0.1f;

    // Draw headers
    drawText(-0.9f, y, "Res ID, Res Destiny, Percentage", GLUT_BITMAP_HELVETICA_18);
    y -= 0.1f;

    // Draw data
    for (int i = 0; i < player_count; i++) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%d, %s, %s", players[i].id, players[i].name, players[i].state); // Assuming 'state' maps to percentage
        drawText(-0.9f, y, buffer, GLUT_BITMAP_HELVETICA_18);
        y -= 0.1f;
    }
}



void renderTeamTable() {
    float y = 0.0f; // Start Y position
    drawText(-0.9f, y, "Team Statistics Table:", GLUT_BITMAP_HELVETICA_18); // Title
    y -= 0.1f;

    // Draw headers
    drawText(-0.9f, y, "Res ID, Citizen ID, Duration, State", GLUT_BITMAP_HELVETICA_18);
    y -= 0.1f;

    // Draw data
    for (int i = 0; i < team_count; i++) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%d, %s, %s, %s", teams[i].team_id, teams[i].name, teams[i].duration, teams[i].state);
        drawText(-0.9f, y, buffer, GLUT_BITMAP_HELVETICA_18);
        y -= 0.1f;
    }
}

void drawBlueFile() {
    if (!isBlueFileVisible)
        return;

    // Draw the file (white rectangle)
    glColor3f(0.8f, 0.8f, 1.0f); // Light blue file
    glBegin(GL_POLYGON);
    glVertex2f(-0.05f + blueFileX, -0.1f + blueFileY);
    glVertex2f(0.05f + blueFileX, -0.1f + blueFileY);
    glVertex2f(0.05f + blueFileX, 0.1f + blueFileY);
    glVertex2f(-0.05f + blueFileX, 0.1f + blueFileY);
    glEnd();

    // Draw lines on the file
    glColor3f(0.0f, 0.0f, 0.0f);
    float lineSpacing = 0.015f;
    for (float i = 0.05f; i > -0.05f; i -= lineSpacing) {
        glBegin(GL_LINES);
        glVertex2f(-0.04f + blueFileX, i + blueFileY);
        glVertex2f(0.04f + blueFileX, i + blueFileY);
        glEnd();
    }

    // Optionally, display file name as text
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow text color
    drawText(blueFileX - 0.02f, blueFileY, "ID", GLUT_BITMAP_HELVETICA_18);
}

// Function to draw the paper file
void drawPaperFile() {
    if (!isPaperVisible)
        return;

    // Draw the file (white rectangle)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(-0.05f + paperX, -0.1f + paperY);
    glVertex2f(0.05f + paperX, -0.1f + paperY);
    glVertex2f(0.05f + paperX, 0.1f + paperY);
    glVertex2f(-0.05f + paperX, 0.1f + paperY);
    glEnd();

    // Draw lines on the file
    glColor3f(0.0f, 0.0f, 0.0f);
    float lineSpacing = 0.015f;
    for (float i = 0.05f; i > -0.05f; i -= lineSpacing) {
        glBegin(GL_LINES);
        glVertex2f(-0.04f + paperX, i + paperY);
        glVertex2f(0.04f + paperX, i + paperY);
        glEnd();
    }

    // Optionally, display file name as text
    glColor3f(0.0f, 0.0f, 1.0f); // Blue text color
    drawText(paperX - 0.02f, paperY, "ID", GLUT_BITMAP_HELVETICA_18);
}

void moveFile(int value) {
    // Slow down file movement
    float movementSpeed = 0.015f; // Decrease movement speed for slower motion

    // Move the file towards the destination
    if (paperX < destinationX) {
        paperX += movementSpeed; // Increment X position
    }
    if (paperY < destinationY) {
        paperY += movementSpeed; // Increment Y position (if necessary)
    }

    // Check if the file reached its destination
    if (paperX >= destinationX) {
        // Reset file position and make it visible again for the loop
        paperX = 0.2f; // Reset to the initial position near the yellow human
        paperY = -0.2f;
        isPaperVisible = true;
    }

    glutPostRedisplay();             // Request a redraw
    glutTimerFunc(100, moveFile, 0); // Call this function again after 100 ms
}



// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}



void drawSign(float buildingX, float buildingWidth, float y, const char *text) {
    float charWidth = 0.03;  // Approximate width of a single character
    float textWidth = strlen(text) * charWidth; // Calculate total text width
    float textX = buildingX + (buildingWidth / 2) - (textWidth / 2); // Center text horizontally above the building
    float textY = y + 0.05; // Place text slightly above the building

    glColor3f(1.0, 0.0, 0.0); // Red text color for visibility
    drawText(textX, textY, text, GLUT_BITMAP_HELVETICA_18); // Centered text
}




// Function to draw a building
void drawBuilding(float x, float y, float width, float height, float r, float g, float b) {
    drawRectangle(x, y, width, height, r, g, b); // Building base

    // Windows
    glColor3f(0.8, 0.8, 0.9); // Light gray windows
    for (float wx = x + 0.05; wx < x + width - 0.05; wx += 0.1) {
        for (float wy = y + 0.1; wy < y + height - 0.1; wy += 0.15) {
            drawRectangle(wx, wy, 0.07, 0.1, 0.8, 0.8, 0.9);
        }
    }

    // Door
    drawRectangle(x + width / 3, y, width / 3, 0.2, 0.4, 0.2, 0.1); // Brown door

    // Rooftop
    drawRectangle(x - 0.02, y + height, width + 0.04, 0.05, 0.3, 0.3, 0.3); // Dark gray rooftop
}

// Function to draw a street with lanes
void drawStreet(float x, float y, float width, float height) {
    drawRectangle(x, y, width, height, 0.1, 0.1, 0.1); // Dark gray street

    // Lane dividers
    glColor3f(1.0, 1.0, 1.0); // White for dividers
    for (float lx = x + 0.1; lx < x + width; lx += 0.2) {
        drawRectangle(lx, y + height / 2 - 0.01, 0.1, 0.02, 1.0, 1.0, 1.0);
    }
}
void drawHuman(float x, float y, float scale, float r, float g, float b) {
    // Head (circle)
    glColor3f(r, g, b); // Use the color passed to the function
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y + scale * 0.15); // Center of the head
    for (int angle = 0; angle <= 360; angle += 10) {
        float theta = angle * M_PI / 180.0;
        glVertex2f(x + cos(theta) * scale * 0.05, y + scale * 0.15 + sin(theta) * scale * 0.05);
    }
    glEnd();

    // Body (rectangle)
    glColor3f(r, g * 0.8, b * 0.8); // Slightly darker body
    drawRectangle(x - scale * 0.03, y, scale * 0.06, scale * 0.1, r, g * 0.8, b * 0.8);

    // Arms (lines)
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex2f(x - scale * 0.05, y + scale * 0.08); // Left arm
    glVertex2f(x + scale * 0.05, y + scale * 0.08); // Right arm
    glEnd();

    // Legs (lines)
    glBegin(GL_LINES);
    glColor3f(r, g, b);
    glVertex2f(x - scale * 0.02, y);              // Left leg
    glVertex2f(x - scale * 0.02, y - scale * 0.1); // End of left leg
    glVertex2f(x + scale * 0.02, y);              // Right leg
    glVertex2f(x + scale * 0.02, y - scale * 0.1); // End of right leg
    glEnd();
}
void drawDataTable(float x, float y, float totalWidth, float rowHeight, int rows, int cols, const char *headers[]) {
    // Calculate the total width dynamically based on the text length in headers
    float colWidths[cols];
    float charWidth = 0.03; // Approximate width of a single character
    float totalCalculatedWidth = 0.0;

    // Calculate column widths based on header text length
    for (int i = 0; i < cols; i++) {
        colWidths[i] = strlen(headers[i]) * charWidth + 0.05; // Add padding to fit text
        totalCalculatedWidth += colWidths[i];
    }

    // Scale the column widths proportionally to the total width
    for (int i = 0; i < cols; i++) {
        colWidths[i] = colWidths[i] / totalCalculatedWidth * totalWidth;
    }

    // Draw table header with dynamic column widths
    float currentX = x;
    for (int i = 0; i < cols; i++) {
        drawRectangle(currentX, y, colWidths[i], rowHeight, 0.7, 0.7, 0.7); // Header cell background
        drawText(currentX + colWidths[i] / 2 - strlen(headers[i]) * charWidth / 2, 
                 y + rowHeight / 3, 
                 headers[i], 
                 GLUT_BITMAP_HELVETICA_18); // Center text in the cell
        currentX += colWidths[i];
    }

    // Draw table rows with borders and dynamic column widths
    for (int row = 1; row <= rows; row++) {
        currentX = x;
        for (int col = 0; col < cols; col++) {
            float rowY = y - row * rowHeight;
            drawRectangle(currentX, rowY, colWidths[col], rowHeight, 0.9, 0.9, 0.9); // Cell background

            // Draw cell border
            glColor3f(0.0, 0.0, 0.0); // Black borders
            glBegin(GL_LINE_LOOP);
            glVertex2f(currentX, rowY);                          // Bottom-left corner
            glVertex2f(currentX + colWidths[col], rowY);         // Bottom-right corner
            glVertex2f(currentX + colWidths[col], rowY + rowHeight); // Top-right corner
            glVertex2f(currentX, rowY + rowHeight);              // Top-left corner
            glEnd();

            currentX += colWidths[col];
        }
    }
}
void drawTree(float x, float y, float trunkWidth, float trunkHeight, float foliageRadius) {
    // Draw the trunk (rectangle)
    drawRectangle(x - trunkWidth / 2, y, trunkWidth, trunkHeight, 0.5, 0.25, 0.1); // Brown trunk

    // Draw the foliage (circle)
    glColor3f(0.0, 0.6, 0.0); // Green foliage
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y + trunkHeight); // Center of the foliage
    for (int angle = 0; angle <= 360; angle += 10) {
        float theta = angle * M_PI / 180.0;
        glVertex2f(x + cos(theta) * foliageRadius, y + trunkHeight + sin(theta) * foliageRadius);
    }
    glEnd();
}

// OpenGL Display Function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

// Draw the sky
    drawRectangle(-1.0, -1.0, 2.0, 2.0, 0.7, 0.9, 1.0); // Light blue sky

    // Draw the ground
    drawRectangle(-1.0, -1.0, 2.0, 0.2, 0.3, 0.7, 0.3); // Green ground

    // Front street
    drawStreet(-1.0, -0.8, 2.0, 0.2);

    // Backyard
    drawRectangle(-1.0, -0.6, 2.0, 0.2, 0.5, 0.8, 0.5); // Grass for backyard

    // Back street
    drawStreet(-1.0, -0.4, 2.0, 0.2);

    // Connecting road between front and back streets
    drawRectangle(-0.3, -0.8, 0.6, 0.4, 0.1, 0.1, 0.1); // Wider vertical connecting road

    // Lane dividers on the connecting road
    glColor3f(1.0, 1.0, 1.0); // White for dividers
    for (float ly = -0.7; ly < -0.4; ly += 0.1) {
        drawRectangle(-0.01, ly, 0.02, 0.05, 1.0, 1.0, 1.0); // Dashed lines
    }

    // Draw the blue building (Espionage Agency) on the front street
    drawBuilding(-0.9, -0.6, 0.4, 0.9, 0.2, 0.4, 0.8);

    // Draw the yellow building (Enemy) on the back street
    drawBuilding(0.5, -0.2, 0.4, 0.5, 0.8, 0.8, 0.2);

    // Add labels for the buildings
    drawSign(-0.9, 0.4, 0.32, "Espionage Agency"); // Blue building label
    drawSign(0.5, 0.4, 0.32, "Enemy");             // Yellow building label moved higher

     // Draw the data table between the buildings, above the streets
    const char *headers[] = {"Res ID", "Res Destiny", "percentage"};
    drawDataTable(-0.99, 0.89, 0.8, 0.1, 4, 3, headers); // Table with 4 rows and 3 columns
    // drawDataTable(-0.10, 0.89, 0.8, 0.1, 4, 3, headers); // Table with 4 rows and 3 columns
    
    const char *headers2[] = {"Res ID", "citizen ID", "Duration"};
    drawDataTable(-0.10, 0.89, 0.8, 0.1, 4, 3, headers2);
   // Draw a garden with smaller trees
// First row of trees
// First row of trees (lowered further)
for (float x = -0.9; x <= 0.9; x += 0.3) {
    drawTree(x, -1.0, 0.03, 0.1, 0.05); // Lowered y-axis for the first row
}

// Second row of trees (lowered further)
for (float x = -0.8; x <= 0.8; x += 0.4) {
    drawTree(x, -0.95, 0.03, 0.1, 0.05); // Lowered y-axis for the second row
}
    drawPaperFile();
        drawBlueFile();   // File from Blue Human to Espionage Agency


    // Draw humans
    drawHuman(-0.2, -0.6, 0.9, 0.0, 0.0, 1.0); // Blue human for Espionage Agency
    drawHuman(0.2, -0.2, 0.9, 1.0, 1.0, 0.0);  // Yellow human for Enemy
    
    drawHuman(-0.1, -0.7, 0.9, 1.0, 0.3, 0.0);  // Orange1 human
    drawHuman(0.09, -0.2, 0.9, 1.0, 0.3, 0.0);  // Orange2 human
    drawHuman(-0.1, -0.4, 0.9, 1.0, 1.0, 0.0);  // Yellow2 human for Enemy


    // Render the two tables
    renderPlayerTable();
    renderTeamTable();

    glutSwapBuffers();
}

// OpenGL Initialization
void initOpenGL() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glFlush();

}


void initGraphics(int argc, char **argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL Table Animation");

    // Initialize OpenGL settings
    initOpenGL();


    // Set up the display and timer callbacks
    glutDisplayFunc(display);
    // glutTimerFunc(1000, checkFile, 0);  // Check the file every 1000 milliseconds

    // Start the main GLUT loop
    glutMainLoop();

}