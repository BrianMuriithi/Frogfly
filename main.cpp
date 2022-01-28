﻿#include <math.h>

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

#define PI 3.141592
#define RAD 57.295780

Rect Rct_Background = { 0, 720, 48, 384 }, Rct_Ground = { 0, 720, 0, 48 };

Image Img_Background, Img_Ground;

int Map[MAX_Y][MAX_X];

float Gravity = -1.2f;

float BOUNDARY_LEFT = 30.0f, BOUNDARY_RIGHT = 690.0f;

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
		y = _y;
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

class c_Line {
public:
	static Image Img_Save[2];
	static Rect Rct;
	Image* Img;
	float x, y, Angle;
	int Player;
	c_Line(int _Player, float _x, float _y, float _Angle) {
		Player = _Player;
		x = _x;
		y = _y;
		Angle = _Angle;
		Img = &Img_Save[Player];
	}

	void Draw() {
		glTranslatef(x, y, 0.0f);
		glRotatef(Angle, 0.0f, 0.0f, 1.0f);
		Map_Texture(Img);
		Draw_Rect(&Rct);
		glLoadIdentity();
	}

	static void Load_Image() {
		Image Img;
		Load_Texture(&Img, "Images/Lines.png");
		Crop_Image(&Img, &Img_Save[0], 0, 0, 8, 4);
		Crop_Image(&Img, &Img_Save[1], 0, 4, 8, 4);
		Zoom_Image(&Img_Save[0], SCALE);
		Zoom_Image(&Img_Save[1], SCALE);
		Delete_Image(&Img);
		Rct.Left = -12.0f;
		Rct.Right = 12.0f;
		Rct.Bottom = -8.0f;
		Rct.Top = 8.0f;
	}
};

Image c_Line::Img_Save[2];
Rect c_Line::Rct;

std::vector<c_Line> Lines;




class c_Frog {
public:
	static Image Img_Save[2][2][2];
	static float Map_Offset[2];
	static float Map_Base_Angle[2];
	Rect Rct;
	Image* Img;
	float x, y, vx, vy;
	int Player;
	int Drt, Anim;
	int Prepare_Stt;
	bool Is_Jumping;
	bool Is_Jump_Pressed;
	float Angle;
	int Angle_Drt;
	int Score;

	void Init(int _Player) {
		Player = _Player;
		Drt = 1 - Player;
		Anim = 0;
		float Offset = 11.0f * CELL_SIZE * (Player == 0 ? -1 : 1);
		x = WIDTH / 2 + Offset;
		y = CELL_SIZE * 2.0f;
		Prepare_Stt = 0;
		Is_Jumping = false;
		Is_Jump_Pressed = false;
		Update_Image();
		Update_Rect();
	}

	void Update_Image() { Img = &Img_Save[Player][Drt][Anim]; }

	void Update_Rect() {
		Rct.Left = x - Img->w / 2;
		Rct.Right = Rct.Left + Img->w;
		Rct.Bottom = y;
		Rct.Top = Rct.Bottom + Img->h;
	}

	void Draw() {
		Map_Texture(Img);
		Draw_Rect(&Rct);
	}

	void Jump() {
		if (!Is_Jumping) {
			Is_Jumping = true;
			Anim = 1;
			Update_Image();
		}
	}

	void Key_Down() {
		Is_Jump_Pressed = true;
	}

	void Key_Up() {
		Is_Jump_Pressed = false;
		
	}

	void Prepare_Start() {
		if (Prepare_Stt == 0) {
			Prepare_Stt = 1;
			Angle_Drt = Drt;
			Angle = Map_Base_Angle[Drt];
		}
	}

	void Prepare_End() {
		if (Prepare_Stt == 1)
			Prepare_Stt = 2;
	}

	void Update() {

		if (!Is_Jumping) {
			if (Is_Jump_Pressed)
				Prepare_Start();
			else
				Prepare_End();
			if (Prepare_Stt > 0) {
				if (Prepare_Stt == 2) {
					Prepare_Stt = 0;
					Jump();
				}
				else {
					Angle += Map_Offset[Angle_Drt];
					if (Check_Angle[Drt][Angle_Drt](Angle))
						Angle_Drt = 1 - Angle_Drt;
					float Angle2 = Angle / RAD;
					float x2 = x, y2 = y + 4.0f, vx2, vy2;
					vx2 = cos(Angle2) * 4 + (Drt == 0 ? Angle2 - PI : Angle2) * 9;
					vy2 = sin(Angle2) * 21;
					vx = vx2;
					vy = vy2;
					for (int i = 0; i < 18; i++) {
						x2 += vx2;
						y2 += vy2;
						if (i % 3 == 2) {
							Angle2 = atan2(vy2, vx2) * RAD;
							Lines.push_back(c_Line(Player, x2, y2, Angle2));
						}
						vy2 += Gravity;
					}
				}
			}
		}
		else {

			float y_old = y;
			x += vx;
			y += vy;
			vy += Gravity;
			if (vy < -24.0f)
				vy = -24.0f;
			if (vy <= 0.0f) {
				int col1 = (x - 9.0f) / CELL_SIZE;
				int col2 = (x + 9.0f) / CELL_SIZE;
				int row_old = y_old / CELL_SIZE;
				int row = y / CELL_SIZE;
				if ((!Map[row_old][col1] && !Map[row_old][col2]) &&
					(Map[row][col1] || Map[row][col2])) {
					Is_Jumping = false;
					y = (row + 1) * CELL_SIZE;
					vx = 0.0f;
					vy = 0.0f;
					Anim = 0;
					Update_Image();
				}
			}

			if (Check_Boundary[Drt](x)) {
				Drt = 1 - Drt;
				vx = -vx;
				Update_Image();
			}

			Update_Rect();
		}
		
	}

	static bool Check_Boundary_Left(float x) { return x < BOUNDARY_LEFT; }
	static bool Check_Boundary_Right(float x) { return x > BOUNDARY_RIGHT; }
	static bool (*Check_Boundary[2])(float x);
	static bool Check_Angle_Left_Decrease(float Angle) {
		return Angle <= 110.0f;
	}
	static bool Check_Angle_Left_Increase(float Angle) {
		return Angle >= 160.0f;
	}
	static bool Check_Angle_Right_Decrease(float Angle) {
		return Angle <= 20.0f;
	}
	static bool Check_Angle_Right_Increase(float Angle) {
		return Angle >= 70.0f;
	}
	static bool (*Check_Angle[2][2])(float Angle);


	static void Load_Image() {
		Image Img;
		Load_Texture(&Img, "Images/Frogs.png");
		Crop_Image(&Img, &Img_Save[0][1][0], 0, 0, 18, 16);
		Crop_Image(&Img, &Img_Save[0][1][1], 0, 16, 18, 16);
		Crop_Image(&Img, &Img_Save[1][1][0], 18, 0, 18, 16);
		Crop_Image(&Img, &Img_Save[1][1][1], 18, 16, 18, 16);
		Swap_Image(Img_Save[0][1][0].img, 18, 16);
		Swap_Image(Img_Save[0][1][1].img, 18, 16);
		Swap_Image(Img_Save[1][1][0].img, 18, 16);
		Swap_Image(Img_Save[1][1][1].img, 18, 16);
		Zoom_Image(&Img_Save[0][1][0], SCALE);
		Zoom_Image(&Img_Save[0][1][1], SCALE);
		Zoom_Image(&Img_Save[1][1][0], SCALE);
		Zoom_Image(&Img_Save[1][1][1], SCALE);
		Flip_Horizontal(&Img_Save[0][1][0], &Img_Save[0][0][0]);
		Flip_Horizontal(&Img_Save[0][1][1], &Img_Save[0][0][1]);
		Flip_Horizontal(&Img_Save[1][1][0], &Img_Save[1][0][0]);
		Flip_Horizontal(&Img_Save[1][1][1], &Img_Save[1][0][1]);
		Delete_Image(&Img);
	}
};
Image c_Frog::Img_Save[2][2][2];
float c_Frog::Map_Offset[2] = { -1.0f, 1.0f };
float c_Frog::Map_Base_Angle[2] = { 160.0f, 20.0f };
bool (*c_Frog::Check_Boundary[2])(float x) = { c_Frog::Check_Boundary_Left, c_Frog::Check_Boundary_Right };
bool (*c_Frog::Check_Angle[2][2])(float Angle) = {
	{Check_Angle_Left_Decrease, Check_Angle_Left_Increase},
	{Check_Angle_Right_Decrease, Check_Angle_Right_Increase} };


c_Frog Frogs[2];

void Display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	Map_Texture(&Img_Background);
	Draw_Rect(&Rct_Background);

	for (c_Line Line : Lines)
		Line.Draw();



	Map_Texture(&Img_Ground);
	Draw_Rect(&Rct_Ground);

	for (int i = 0; i < CLOUD_COUNT; i++)
		Clouds[i].Draw();
	


	for (int i = 0; i < PLATFORMER_COUNT; i++)
		Platformers[i].Draw();

	Frogs[0].Draw();
	Frogs[1].Draw();

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
	c_Frog::Load_Image();
	c_Line::Load_Image();
	

	Platformers[0].Init(7, 5);
	Platformers[1].Init(19, 5);
	Platformers[2].Init(4, 9);
	Platformers[3].Init(22, 9);
	Platformers[4].Init(9, 13);
	Platformers[5].Init(17, 13);


	Clouds[0].Init(570.0f, 210.0f);
	Clouds[1].Init(930.0f, 300.0f);
	Clouds[2].Init(240.0f, 255.0f);

	Frogs[0].Init(0);
	Frogs[1].Init(1);
	



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

	Lines.clear();

	Frogs[0].Update();
	Frogs[1].Update();

	glutPostRedisplay();
	glutTimerFunc(INTERVAL, Timer, 0);
}

void Keyboard_Down(GLubyte key, int x, int y) {
	switch (key) {
	case 32:
		Frogs[0].Key_Down();
		break;
	case 13:
		Frogs[1].Key_Down();
		break;
	}
}

void Keyboard_Up(GLubyte key, int x, int y) {
	switch (key) {
	case 32:
		Frogs[0].Key_Up();
		break;
	case 13:
		Frogs[1].Key_Up();
		break;
	}
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

	glutKeyboardFunc(Keyboard_Down);
	glutKeyboardUpFunc(Keyboard_Up);
	

	glutMainLoop();
	return 0;
}

