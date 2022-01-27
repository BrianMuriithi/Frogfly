#include <math.h>

#include <GL/glut.h>

#include <vector>

#include "../Library/loadpng.h"
#include "../Library/process_image.h"
#include "../Library/gl_texture.h"

#define WIDTH 720

#define HEIGHT 384

#define INTERVAL 15

#define SCALE 3

Rect Rct_Background = { 0, 720, 48, 384 }, Rct_Ground = { 0, 720, 0, 48 };

Image Img_Background, Img_Ground;


void Display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	Map_Texture(&Img_Background);
	Draw_Rect(&Rct_Background);
	Map_Texture(&Img_Ground);
	Draw_Rect(&Rct_Ground);

	glutSwapBuffers();
}

void Init_Game() {
	Load_Texture_Swap(&Img_Background, "Images/Background.png");
	Zoom_Image(&Img_Background, SCALE);
	Load_Texture_Swap(&Img_Ground, "Images/Ground.png");
	Zoom_Image(&Img_Ground, SCALE);
}

void Init_GL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, WIDTH, HEIGHT);
	gluOrtho2D(0, WIDTH, 0, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glEnable(GL_TEXTURE_2D);

	Init_Game();
}


void Timer(int value) {
	
	glutPostRedisplay();
	glutTimerFunc(INTERVAL, Timer, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	int POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
	int POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
	glutInitWindowPosition(POS_X, POS_Y);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Catch Flies");

	Init_GL();
	glutDisplayFunc(Display);
	glutTimerFunc(0, Timer, 0);

	glutMainLoop();
	return 0;
}

