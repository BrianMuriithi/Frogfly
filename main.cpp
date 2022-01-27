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

#define PLATFORMER_COUNT 6

#define CLOUD_COUNT 3

#define CELL_SIZE 24
#define	MAX_X 30
#define MAX_Y 24

Rect Rct_Background = { 0, 720, 48, 384 }, Rct_Ground = { 0, 720, 0, 48 };

Image Img_Background, Img_Ground;

int Map[MAX_Y][MAX_X];

class c_Platformer {
public:
	static Image Img_Save;
	static void Load_Image() {
		Load_Texture_Swap(&Img_Save, "Images/Platformer.png");
		Zoom_Image(&Img_Save, SCALE);
	}

	Rect Rct;
	Image* Img;

	void Init(int _x, int _y) {
		Map[_y][_x] = Map[_y][_x + 1] = Map[_y][_x + 2] = Map[_y][_x + 3] = 1;
		Img = &Img_Save;
		float x = (_x + 2) * CELL_SIZE;
		float y = _y * CELL_SIZE;
		Rct.Left = x - Img->w / 2;
		Rct.Right = Rct.Left + Img->w;
		Rct.Bottom = y;
		Rct.Top = Rct.Bottom + Img->h;
	}


	void Draw() {
		Map_Texture(Img);
		Draw_Rect(&Rct);
	}

};

Image c_Platformer::Img_Save;

c_Platformer Platformers[PLATFORMER_COUNT];

class c_Cloud {
public:
	static Image Img_Save;
	static void Load_Image() {
		Load_Texture_Swap(&Img_Save, "Images/Cloud.png");
		Zoom_Image(&Img_Save, SCALE);
	}

	Rect Rct;
	Image *Img;

	float x, y;

	void Init(float _x, float _y) {
		Img = &Img_Save;
		x = _x;
		y = -y;
		Update_Rect();
		Rct.Bottom = y;
		Rct.Top = Rct.Bottom + Img->h;
	}
	void Draw() {
		Map_Texture(Img);
		Draw_Rect(&Rct);
	}
	void Update_Rect() {
		Rct.Left = x - Img->w / 2;
		Rct.Right = Rct.Left + Img->w;
	}
	void Update() {
		x -= 0.3f;
		if (x < -120.0f)
			x += 1080.0f;
		Update_Rect();
	}
};
Image c_Cloud::Img_Save;

c_Cloud Clouds[CLOUD_COUNT];


void Display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	Map_Texture(&Img_Background);
	Draw_Rect(&Rct_Background);
	Map_Texture(&Img_Ground);
	Draw_Rect(&Rct_Ground);

	for (int i = 0; i < CLOUD_COUNT; i++)
		Clouds[i].Draw();
	


	for (int i = 0; i < PLATFORMER_COUNT; i++)
		Platformers[i].Draw();
	glutSwapBuffers();
}

void Init_Game() {
	Load_Texture_Swap(&Img_Background, "Images/Background.png");
	Zoom_Image(&Img_Background, SCALE);
	Load_Texture_Swap(&Img_Ground, "Images/Ground.png");
	Zoom_Image(&Img_Ground, SCALE);

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < MAX_X; j++)
			Map[i][j] = 1;
	for (int i = 2; i < MAX_Y; i++)
		for (int j = 0; j < MAX_X; j++)
			Map[i][j] = 0;

	c_Platformer::Load_Image();
	c_Cloud::Load_Image();

	Platformers[0].Init(7, 5);
	Platformers[1].Init(19, 5);
	Platformers[2].Init(4, 9);
	Platformers[3].Init(22, 9);
	Platformers[4].Init(9, 13);
	Platformers[5].Init(17, 13);


	Clouds[0].Init(570.0f, 210.0f);
	Clouds[1].Init(930.0f, 300.0f);
	Clouds[2].Init(240.0f, 255.0f);




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
	for (int i = 0; i < CLOUD_COUNT; i++)
		Clouds[i].Update();

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

