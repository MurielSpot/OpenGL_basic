#include <glew/GL/glew.h>
#include<freeglut/GL/glut.h>
#include <math.h>  

#define C_PI 3.14159265358979  

GLdouble distanceSunToEarth = 70;//日地距离  
GLdouble distanceMoonToEarth = 35;//月地距离  

GLdouble gzAngleEarth = 0;//地球绕太阳的公转角度  
GLdouble gzAngleMoon = 0;//月球绕地球的公转角度  
GLsizei zzAngle = 0;//自转角度  

int earthX = distanceSunToEarth;
int earthY = 0;
int earthZ = 0;

int moonX = distanceSunToEarth + distanceMoonToEarth;
int moonY = 0;
int moonZ = 0;

void drawPlanet(int r, GLdouble x0, GLdouble y0, GLdouble z0)
{
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	glRotatef(70.0, 1.0, 0.0, 0.0);
	glTranslatef(x0, y0, z0);
	glRotatef(zzAngle, 0.0, 0.0, 1.0);

	glutWireSphere(r, 30, 10);
	glPopMatrix();
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, distanceSunToEarth + 100, 0.0, 0.0, 0.0, 0.0, 1.0, distanceSunToEarth + 100);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		gzAngleEarth += C_PI / 180;
		earthX = (GLdouble)distanceSunToEarth * cos(gzAngleEarth);
		earthY = distanceSunToEarth * sin(gzAngleEarth);

		zzAngle = (zzAngle + 10) % 360;

		gzAngleMoon += C_PI / 45;
		moonX = earthX + distanceMoonToEarth * cos(gzAngleMoon);
		moonY = earthY + distanceMoonToEarth * sin(gzAngleMoon);

		glutPostRedisplay();
		break;
	case 's':
		glutPostRedisplay();
		break;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	//draw sun  
	drawPlanet(30, 0, 0, 0);

	//draw earth  
	drawPlanet(20, earthX, earthY, earthZ);

	//draw moon  
	drawPlanet(10, moonX, moonY, moonZ);

	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Nehe 01");

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	//glutTimerFunc(100, timer, );  
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
