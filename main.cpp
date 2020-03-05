#include<Windows.h> 
#include<stdio.h>    
#include<gl/glut.h>   
#include<conio.h>   
#include<stdlib.h> 
#include<stdio.h>    
#include<math.h>    
#include<string.h>  
#include <vector>
#include<iostream>

using namespace std;

int window, val = 1, clip = 0, fill = 0, viewport = 0;
int width = 750, height = 550, start = -1, pSize = 0;
int x_min = 250, x_max = 500, y_min = 450, y_max = 200;
vector<pair<int, int>> poly, points;

void init() {
    glClearColor((GLclampf)0.9, (GLclampf)0.9, (GLclampf)0.9, (GLclampf)0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, width, 0, height);
}

void makePolygon() {
    pSize = poly.size();
    points.resize(pSize);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < (int)poly.size(); i++) {
        points[pSize - (i + 1)].first = poly[i].first;
        points[pSize - (i + 1)].second = poly[i].second;
        glVertex2i(poly[i].first, poly[i].second);
    }

    glEnd();
    glFlush();
}

int inner(int x, int y, int edge) {
    int ret = 0;
    switch (edge) {
    case 1:
        if (x < x_max) ret = 1;
        else ret = 0;
        break;
    case 2:
        if (y > y_max) ret = 1;
        else ret = 0;
        break;
    case 3:
        if (x > x_min) ret = 1;
        else ret = 0;
        break;
    case 4:
        if (y < y_min) ret = 1;
        else ret = 0;
        break;
    default:
        ret = 0;
    }
    return ret;
}

void boundaryFill(int x, int y, GLfloat* fill, GLfloat* bound) {
    GLfloat c[3];
    glReadPixels(x, y, width, height, GL_RGB, GL_FLOAT, c);
    if (c[0] != bound[0] || c[1] != bound[1] || c[2] != bound[2]) {
        if (c[0] != fill[0] || c[1] != fill[1] || c[2] != fill[2]) {
            glColor3f(fill[0], fill[1], fill[2]);
            glBegin(GL_POINTS);
            glVertex2i(x, y);
            glEnd();
            glFlush();
            boundaryFill(x + 1, y, fill, bound);
            boundaryFill(x - 1, y, fill, bound);
            boundaryFill(x, y + 2, fill, bound);
            boundaryFill(x, y - 2, fill, bound);
        }
    }
}

pair<int, int> intersection(int x1, int y1, int x2, int y2, int edge) {
    float m = 0;
    if (x2 - x1 == 0)
        m = 0;
    else
        m = (float)(y2 - y1) / (x2 - x1);

    float b = (float)(y1 - m * x1);
    int x, y;
    if (edge == 1) {
        y = (m * x_max) + b;
        x = x_max;
    }
    else if (edge == 2) {
        y = y_max;
        if (m == 0) x = x2;
        else x = (y - b) / m;
    }
    else if (edge == 3) {
        x = x_min;
        y = (m * x) + b;
    }
    else {
        y = y_min;
        if (m == 0) x = x2;
        else x = (y - b) / m;
    }
    return make_pair(x, y);
}

int sh_clip(int edge) {

    if (pSize == 0) {
        return 1;
    }
    pair<int, int> a = points[pSize - 1];

    vector<pair<int, int>> next;
    for (int i = 0; i < pSize; i++) {
        pair<int, int> b = points[i];
        if (inner(b.first, b.second, edge)) {
            if (inner(a.first, a.second, edge)) {
                next.insert(next.begin(), b);
            }
            else {
                pair<int, int> inter = intersection(a.first, a.second, b.first, b.second, edge);
                next.insert(next.begin(), inter);
                next.insert(next.begin(), b);
            }
        }
        else if (inner(a.first, a.second, edge)) {
            pair<int, int> inter = intersection(a.first, a.second, b.first, b.second, edge);
            next.insert(next.begin(), inter);
        }
        a = b;
    }

    pSize = next.size();
    points.resize(pSize);
    for (int c = 0; c < pSize; c++) {
        points[pSize - c - 1] = next[c];
    }
    //for (int c = 0; c < pSize; c++) {
    //    cout << points[c].first << ", " << points[c].second << "\n";
    //}
    next.empty();
    return 0;
}

void mouse(int button, int state, int x, int y) {
    int real_y = height - y;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        poly.insert(poly.begin(),make_pair(x, real_y));
    }
}

void key(unsigned char key_t, int x, int y) {
    points.empty();
    val = 1;
    clip = 0;
    if (key_t == ' ') {
        makePolygon();
        poly.resize(0);
    }
}

void draw() {
    glPointSize(1);
    //glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < pSize; i++) {
        glVertex2i(points[i].first, points[i].second);
    }
    glEnd();
    glFlush();
}

void display() {
    glPointSize(1);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f((GLfloat)0.7, (GLfloat)0.4, (GLfloat)0.3);
    if ((val == 2 || val == 3) && clip == 0) { //clipping option
        sh_clip(1);
        sh_clip(2);
        sh_clip(3);
        sh_clip(4);
        clip = 1;
    }
    if (val == 3) {
        GLfloat red[3] = { 1.0,0.0,0.0 };
        GLfloat bound[3] = { 0.7,0.4,0.3 };
        if (points.size() > 0) {
            boundaryFill(points[0].first - 1, points[0].second -1, red, bound);
        }
    }
    if (val == 4) {
        cout << val << "\n";
    }

    draw();
    glColor3f(0, 0, 0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x_min, y_min);
    glVertex2i(x_min, y_max);
    glVertex2i(x_max, y_max);
    glVertex2i(x_max, y_min);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glFlush();
    glColor3f((GLfloat)0.7, (GLfloat)0.4, (GLfloat)0.3);
}

void menu(int n) {
    if (n == 0) {
        glutDestroyWindow(window);
        exit(0);
    }
    else {
        val = n;
        glutPostRedisplay();
    }
}

void startMenu() {
    glutCreateMenu(menu);
    glutAddMenuEntry("polygon clipping", 2);
    glutAddMenuEntry("region filling", 3);
    glutAddMenuEntry("Window-to-Viewport Mapping", 4);
    glutAddMenuEntry("Exit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width,height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Project 2");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(key);
    startMenu();
    init();
    glutMainLoop();
    return 0;
}