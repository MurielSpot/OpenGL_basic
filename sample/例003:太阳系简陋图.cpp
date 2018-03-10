/*
点鼠标图会动.
*/
#include <glew/GL/glew.h>
#include<freeglut/GL/glut.h>
#include <math.h>  

#define GLUT_DISABLE_ATEXIT_HACK

// 太阳、地球和月亮
// 假设每个月都是30天
// 一年12个月，共是360天
static int day = 180; // day的变化：从0到359
void myDisplay()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_EQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1, 400000000);
	// gluPerspective(60.0, 1.0, 10000, 400000000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, -200000000, 200000000, 0, 0, 0, 0, 0, 1);

	// 绘制红色的“太阳”
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidSphere(69600000, 40, 40);

	// 绘制蓝色的“地球”
	glColor3f(0.0f, 0.0f, 1.0f);
	glRotatef(day / 360.0*360.0, 0.0f, 0.0f, -1.0f);
	glTranslatef(150000000, 0.0f, 0.0f);
	glutSolidSphere(15945000, 40, 40);

	// 绘制黄色的“月亮”
	glColor3f(1.0f, 1.0f, 0.0f);
	glRotatef(day / 30.0*360.0 - day / 360.0*360.0, 0.0f, 0.0f, -1.0f);
	glTranslatef(38000000, 0.0f, 0.0f);
	glutSolidSphere(4345000, 20, 20);

	glFlush();
}

void mymouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		day++;
	if (day > 360)
		day = 0;

	myDisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("OpenGL学习");
	glutDisplayFunc(&myDisplay);
	glutMouseFunc(&mymouse);
	glutMainLoop();
	return 0;
}
