#include <GL/glut.h>
#include <array>
#include <map>
#include <string>
#include <iostream>

// Global to store the 96-character Rubik’s state.
static std::string cubeString;
static float angle = 0.0f; // global rotation angle

// Maps single-letter color codes to an RGB triplet.
static std::map<char, std::array<float,3>> colorMap {
    {'W', {1.0f, 1.0f, 1.0f}}, // White
    {'Y', {1.0f, 1.0f, 0.0f}}, // Yellow
    {'G', {0.0f, 1.0f, 0.0f}}, // Green
    {'B', {0.0f, 0.0f, 1.0f}}, // Blue
    {'O', {1.0f, 0.5f, 0.0f}}, // Orange
    {'R', {1.0f, 0.0f, 0.0f}}  // Red
};

// Draw a 1x1 cube, then color a single face. 
// 'face' sets which face (U, D, F, B, L, R) gets the color.
void drawFaceCube(float x, float y, float z,
                  const std::array<float,3>& colorRGB,
                  char face)
{
    float baseHalf = 0.5f;
    glPushMatrix();
    glTranslatef(x, y, z);

    // Draw the gray base cube
    glColor3f(0.3f, 0.3f, 0.3f);
    glutSolidCube(1.0);

    // We'll draw a smaller colored quad on the chosen face
    float off    = 0.0001f;  // tiny push outward
    float shrink = 0.8f;     // make the square 80% of the face so you see gaps
    float h      = baseHalf * shrink;

    glColor3f(colorRGB[0], colorRGB[1], colorRGB[2]);
    glBegin(GL_QUADS);

    if (face == 'U') { // Up (y+)
        glVertex3f(-h,  baseHalf + off,  h);
        glVertex3f( h,  baseHalf + off,  h);
        glVertex3f( h,  baseHalf + off, -h);
        glVertex3f(-h,  baseHalf + off, -h);
    }
    else if (face == 'D') { // Down (y-)
        glVertex3f(-h, -baseHalf - off, -h);
        glVertex3f( h, -baseHalf - off, -h);
        glVertex3f( h, -baseHalf - off,  h);
        glVertex3f(-h, -baseHalf - off,  h);
    }
    else if (face == 'F') { // Front (z+)
        glVertex3f(-h,  h,  baseHalf + off);
        glVertex3f(-h, -h,  baseHalf + off);
        glVertex3f( h, -h,  baseHalf + off);
        glVertex3f( h,  h,  baseHalf + off);
    }
    else if (face == 'B') { // Back (z-)
        glVertex3f( h,  h, -baseHalf - off);
        glVertex3f( h, -h, -baseHalf - off);
        glVertex3f(-h, -h, -baseHalf - off);
        glVertex3f(-h,  h, -baseHalf - off);
    }
    else if (face == 'L') { // Left (x-)
        glVertex3f(-baseHalf - off,  h, -h);
        glVertex3f(-baseHalf - off, -h, -h);
        glVertex3f(-baseHalf - off, -h,  h);
        glVertex3f(-baseHalf - off,  h,  h);
    }
    else if (face == 'R') { // Right (x+)
        glVertex3f(baseHalf + off,  h,  h);
        glVertex3f(baseHalf + off, -h,  h);
        glVertex3f(baseHalf + off, -h, -h);
        glVertex3f(baseHalf + off,  h, -h);
    }

    glEnd();
    glPopMatrix();
}


// Roughly figure out where to place each 1x1 sub-square, depending on face/row/col.
std::array<float,3> computeCenter(char face, int row, int col)
{
    float offset = 1.5f; // for a 4x4 face, centers range ~±1.5
    switch(face) {
        case 'U': return {col - offset,  offset,      (3-row) - offset};
        case 'D': return {col - offset, -offset,      (3-row) - offset};
        case 'F': return {col - offset,  (3-row) - offset,  offset};
        case 'B': return {(3-col) - offset, (3-row) - offset, -offset};
        case 'L': return {-offset,      (3-row) - offset, (3-col) - offset};
        case 'R': return { offset,      (3-row) - offset, col - offset};
    }
    return {0.0f,0.0f,0.0f};
}

// Add an idle callback:
void idle()
{
    angle += 0.3f;         // rotate 0.3 degrees per idle cycle
    if (angle > 360.0f) {
        angle -= 360.0f;   // wrap around
    }
    glutPostRedisplay();   // re-draw
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(8.0, 6.0, 10.0,  // position
              0.0, 0.0, 0.0,   // looking at origin
              0.0, 1.0, 0.0);  // up direction
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    // Correct index mapping to match the print() function layout
    static const char faces[6] = {'U', 'L', 'F', 'R', 'B', 'D'};
    static const int faceOffsets[6] = { 0, 16, 32, 48, 64, 80 }; // Corrected mapping

    for (int f = 0; f < 6; ++f) {
        char face = faces[f];
        int baseIndex = faceOffsets[f];

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                char c = cubeString[baseIndex + row * 4 + col]; // Corrected indexing
                auto colorRGB = (colorMap.count(c) ? colorMap[c] : std::array<float,3>{0.5f,0.5f,0.5f});
                auto center   = computeCenter(face, row, col);
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(-1.0f, -1.0f);
                drawFaceCube(center[0], center[1], center[2], colorRGB, face);
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
        }
    }

    glutSwapBuffers();
}


int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <96-char-string>\n";
        return 1;
    }
    cubeString = argv[1];
    if (cubeString.size() != 96) {
        std::cerr << "Error: Input must be 96 characters long.\n";
        return 1;
    }

    // Basic GLUT init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("4x4 Rubik's Cube");

    glEnable(GL_DEPTH_TEST);

    // A basic perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 4.0/3.0, 1.0, 100.0);

    // Register display callback
    glutDisplayFunc(display);

    glutIdleFunc(idle);

    // Enter main loop
    glutMainLoop();
    return 0;
}
