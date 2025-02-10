#include <GL/glut.h>
#include <array>
#include <map>
#include <string>
#include <iostream>

static std::string cubeString;
static float angle = 0.0f; 

static std::map<char, std::array<float,3>> colorMap {
    {'W', {1.0f, 1.0f, 1.0f}}, 
    {'Y', {1.0f, 1.0f, 0.0f}},
    {'G', {0.0f, 1.0f, 0.0f}}, 
    {'B', {0.0f, 0.0f, 1.0f}},
    {'O', {1.0f, 0.5f, 0.0f}},
    {'R', {1.0f, 0.0f, 0.0f}} 
};


void drawFaceCube(float x, float y, float z, const std::array<float,3>& colorRGB, char face) {
    float baseHalf = 0.5f;
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.3f, 0.3f, 0.3f);
    glutSolidCube(1.0);

    float off    = 0.0001f;  
    float shrink = 0.8f;  
    float h      = baseHalf * shrink;

    glColor3f(colorRGB[0], colorRGB[1], colorRGB[2]);
    glBegin(GL_QUADS);

    if (face == 'U') { 
        glVertex3f(-h,  baseHalf + off,  h);
        glVertex3f( h,  baseHalf + off,  h);
        glVertex3f( h,  baseHalf + off, -h);
        glVertex3f(-h,  baseHalf + off, -h);
    }
    else if (face == 'D') {
        glVertex3f(-h, -baseHalf - off, -h);
        glVertex3f( h, -baseHalf - off, -h);
        glVertex3f( h, -baseHalf - off,  h);
        glVertex3f(-h, -baseHalf - off,  h);
    }
    else if (face == 'F') { 
        glVertex3f(-h,  h,  baseHalf + off);
        glVertex3f(-h, -h,  baseHalf + off);
        glVertex3f( h, -h,  baseHalf + off);
        glVertex3f( h,  h,  baseHalf + off);
    }
    else if (face == 'B') { 
        glVertex3f( h,  h, -baseHalf - off);
        glVertex3f( h, -h, -baseHalf - off);
        glVertex3f(-h, -h, -baseHalf - off);
        glVertex3f(-h,  h, -baseHalf - off);
    }
    else if (face == 'L') { 
        glVertex3f(-baseHalf - off,  h, -h);
        glVertex3f(-baseHalf - off, -h, -h);
        glVertex3f(-baseHalf - off, -h,  h);
        glVertex3f(-baseHalf - off,  h,  h);
    }
    else if (face == 'R') { 
        glVertex3f(baseHalf + off,  h,  h);
        glVertex3f(baseHalf + off, -h,  h);
        glVertex3f(baseHalf + off, -h, -h);
        glVertex3f(baseHalf + off,  h, -h);
    }

    glEnd();
    glPopMatrix();
}


std::array<float,3> computeCenter(char face, int row, int col)
{
    float offset = 1.5f;
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

void idle()
{
    angle += 0.3f;         
    if (angle > 360.0f) {
        angle -= 360.0f;   
    }
    glutPostRedisplay();   
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(8.0, 6.0, 10.0,  
              0.0, 0.0, 0.0,   
              0.0, 1.0, 0.0);  
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

    static const char faces[6] = {'U', 'L', 'F', 'R', 'B', 'D'};
    static const int faceOffsets[6] = { 0, 16, 32, 48, 64, 80 }; 

    for (int f = 0; f < 6; ++f) {
        char face = faces[f];
        int baseIndex = faceOffsets[f];

        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                char c = cubeString[baseIndex + row * 4 + col]; 
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

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("4x4 Rubik's Cube");

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 4.0/3.0, 1.0, 100.0);

    glutDisplayFunc(display);

    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
