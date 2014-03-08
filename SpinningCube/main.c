/* ============================================================================
**
** Demonstration of spinning cube
** Copyright (C) 2005  Julien Guertault
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
** ========================================================================= */

#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

float xRot = 0.0, yRot = 0.0, zRot = 0.0;

/*
** Function called to update rendering
*/
void DisplayFunc(void) {
	/* Clear the buffer, clear the matrix */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/* A step backward*/
	glTranslatef(0, 0, -10);

	//Scale down a bit to leave more space around the cube
	glScalef(0.8, 0.8, 0.8);

	//X rotation is first, followed by Y and Z
	glRotatef(xRot, 1, 0, 0);
	glRotatef(yRot, 0, 1, 0);
	glRotatef(zRot, 0, 0, 1);

	//Draw lines for the axes.
	glBegin(GL_LINES);

	//X axis
	glColor3f(1, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(2, 0, 0);

	//Y axis
	glColor3f(1, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 2, 0);

	//Z axis
	glColor3f(0, 1, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 2);
	
	glEnd();

	//Switch to drawing squares.
	glBegin(GL_QUADS);

	//Left side
	glColor3f(0, 0, 0); glVertex3f(-1, -0.2, -1);
	glColor3f(0, 0, 1); glVertex3f(-1, -0.2,  1);
	glColor3f(0, 1, 1); glVertex3f(-1,  0.2,  1);
	glColor3f(0, 1, 0); glVertex3f(-1,  0.2, -1);

	//Right side
	glColor3f(1, 0, 0); glVertex3f( 1, -0.2, -1);
	glColor3f(1, 0, 1); glVertex3f( 1, -0.2,  1);
	glColor3f(1, 1, 1); glVertex3f( 1,  0.2,  1);
	glColor3f(1, 1, 0); glVertex3f( 1,  0.2, -1);

	//Bottom side
	glColor3f(0, 0, 0); glVertex3f(-1, -0.2, -1);
	glColor3f(0, 0, 1); glVertex3f(-1, -0.2,  1);
	glColor3f(1, 0, 1); glVertex3f( 1, -0.2,  1);
	glColor3f(1, 0, 0); glVertex3f( 1, -0.2, -1);

	//Top side
	glColor3f(0, 1, 0); glVertex3f(-1,  0.2, -1);
	glColor3f(0, 1, 1); glVertex3f(-1,  0.2,  1);
	glColor3f(1, 1, 1); glVertex3f( 1,  0.2,  1);
	glColor3f(1, 1, 0); glVertex3f( 1,  0.2, -1);

	//Back side
	glColor3f(0, 0, 0); glVertex3f(-1, -0.2, -1);
	glColor3f(0, 1, 0); glVertex3f(-1,  0.2, -1);
	glColor3f(1, 1, 0); glVertex3f( 1,  0.2, -1);
	glColor3f(1, 0, 0); glVertex3f( 1, -0.2, -1);

	//Front side
	glColor3f(0, 0, 1); glVertex3f(-1, -0.2,  1);
	glColor3f(0, 1, 1); glVertex3f(-1,  0.2,  1);
	glColor3f(1, 1, 1); glVertex3f( 1,  0.2,  1);
	glColor3f(1, 0, 1); glVertex3f( 1, -0.2,  1);

	/* No more quads */
	glEnd();

	/* End */
	glFlush();
	glutSwapBuffers();
}

/*
** Function called when the window is created or resized
*/
void ReshapeFunc(int width, int height) {
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluPerspective(20, width / (float) height, 5, 15);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

/*
** Function called when a key is hit
*/
void KeyboardFunc(unsigned char key, int x, int y) {
	int foo;

	foo = x + y; /* Has no effect: just to avoid a warning */
	if ('q' == key || 'Q' == key || 27 == key)
		exit(0);
}

void mainLoop() {
	float x, y, z;
	if (scanf("%f %f %f", &x, &y, &z)) {
		xRot = x;
		yRot = y;
		zRot = z;
		glutPostRedisplay();
	}
}

//Can be used instead of mainLoop to look at the glorious pancake spinning.
void autoRot() {
	xRot += 0.01;
	yRot += 0.1;
	zRot += 0.2;
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	/* Creation of the window */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitContextVersion(1, 2);  
	glutCreateWindow("Spinning cube");

	/* OpenGL settings */
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3);
	
	/* Declaration of the callbacks */
	glutDisplayFunc(&DisplayFunc);
	glutReshapeFunc(&ReshapeFunc);
	glutKeyboardFunc(&KeyboardFunc);

	//change mainLoop to autoRot to demo settings
	glutIdleFunc(mainLoop);
	
	/* Loop */
	glutMainLoop();
	
	return 0;
}
