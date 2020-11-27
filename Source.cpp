
#pragma warning( disable: 4838 )
#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <glaux.h>		// Header File For The Glaux Library
#include <gl/glaux.h>
#include <glut.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include "debug/gltexture.h"
#include"debug/Model_3DS.h"
#include <irrKlang.h>
// include console I/O methods (conio.h for windows, our wrapper in linux)
#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#define CAMERASPEED	0.5f
#define PI	3.14f

int mouseX = 0, mouseY = 0;
bool isClicked = 0, isRClicked = 0;
GLfloat h = 0, v = 0;
GLdouble I = 0, K = 0;
GLdouble R = 0, R2 = 0;
float rtfan = 0,rtchar=-90;

float jumpLimit = 4;
enum state {
	jump_up, jump_down, walk
};
state s = walk;
float eyey = 0;
float carstr = 0,carstr2=0;
enum doorstate {
	roll_up, roll_down, stop
};
doorstate door = stop;
float rolldoor=0;

enum malldoorstate {
	close, open, stopped
};
malldoorstate malldoor = stopped;
float rightdoor = 0,leftdoor=0;

float Light_Ambient[] = { 0.3,0.3,0.3,1 };
float Light_Diffuse[] = { 1.0,1.0,1.0,1 };
float Light_Specular[] = { 1.0,1.0,1.0,1 };
float Light_Position[] = { 0,20,300,1 };
float Light_Spot_direction[] = { 0, -20, -500 };
float Light_CutOff = 10;

float Light_Ambient2[] = { 1.0,1.0,1.0,1.0,1 };
float Light_Diffuse2[] = { 1.0,1.0,1.0,1 };
float Light_Specular2[] = { 1.0,1.0,0.1,1 };
float Light_Position2[] = { -1,3.2,0,1 };
float Light_Spot_direction2[] = { 0.0, -5.0, 0 };
float Light_CutOff2 = 20;

float Light_Ambient1[] = { 1.0,1.0,1.0,1.0,1 };
float Light_Diffuse1[] = { 1.0,1.0,1.0,1 };
float Light_Specular1[] = { 1.0,1.0,0.1,1 };
float Light_Position1[] = { 0, 7, 0, 1 };
float Light_Spot_direction1[] = { 0.0, -100.0, 0 };
float Light_CutOff1 = 700;

float Cone_Ambient[] = { 0.1,0.1,0.1,1 };
float Cone_Diffuse[] = { 0.2,0.1,0.9,1 };
float Cone_Specular[] = { 1,1,1,1 };
float Cone_Shininess = 100;

GLfloat asp_Specular[] = { 1.0,1.0,1.0,1.0 };
GLfloat asp_Shininess[] = { 90.0 };

GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat mat_amb_diff_color_red[] = { 1.0, 0.5, 0.0, 0.5 };
GLfloat mat_amb_diff_color_green[] = { 0.0, 1.0, 0.0, 0.5 };
static GLfloat spin = 0.0;
static GLfloat speed = 0.0;
static int running = 0;
static int night = 0;

// defining Sphere properties
float Sphere_Ambient[] = { 0.2,0.2,0.1,1 };
float Sphere_Diffuse[] = { 0.2,0.3,1,1 };
float Sphere_Specular[] = { 1,1,1,1 };
float Sphere_Shininess = 120;

float asphalt_Ambient[] = { 0,0,0,1 };
float asphalt_Diffuse[] = { 0,0,0,1 };
float asphalt_Specular[] = { 1,1,1,1 };
float asphalt_Shininess = 120;

float angle = 0;
float angle2 = 0.6;
float angle3 = 2;
int hours, minutes, seconds;

UINT texture[50];
UINT skybox1[6];
UINT skybox2[6];
int Image;
Model_3DS *model;
bool day = true;

int num_texture = -1; //Counter to keep track of the last loaded texture

int LoadTexture(char *filename, int alpha)
{
	int i, j = 0; //Index variables
	FILE *l_file; //File pointer
	unsigned char *l_texture; //The pointer to the memory zone in which we will load the texture

							  // windows.h gives us these types to work with the Bitmap files
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	RGBTRIPLE rgb;

	num_texture++; // The counter of the current texture is increased

	if ((l_file = fopen(filename, "rb")) == NULL) return (-1); // Open the file for reading

	fread(&fileheader, sizeof(fileheader), 1, l_file); // Read the fileheader

	fseek(l_file, sizeof(fileheader), SEEK_SET); // Jump the fileheader
	fread(&infoheader, sizeof(infoheader), 1, l_file); // and read the infoheader

													   // Now we need to allocate the memory for our image (width * height * color deep)
	l_texture = (byte *)malloc(infoheader.biWidth * infoheader.biHeight * 4);
	// And fill it with zeros
	memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);

	// At this point we can read every pixel of the image
	for (i = 0; i < infoheader.biWidth*infoheader.biHeight; i++)
	{
		// We load an RGB value from the file
		fread(&rgb, sizeof(rgb), 1, l_file);

		// And store it
		l_texture[j + 0] = rgb.rgbtRed; // Red component
		l_texture[j + 1] = rgb.rgbtGreen; // Green component
		l_texture[j + 2] = rgb.rgbtBlue; // Blue component
		l_texture[j + 3] = alpha; // Alpha value
		j += 4; // Go to the next position
	}

	fclose(l_file); // Closes the file stream

	glBindTexture(GL_TEXTURE_2D, num_texture); // Bind the ID texture specified by the 2nd parameter

											   // The next commands sets the texture parameters
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //The minifying function

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECR);

	// Finally we define the 2d texture
	glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

	// And create 2d mipmaps for the minifying function
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

	free(l_texture); // Free the memory we used to load the texture

	return (num_texture); // Returns the current texture OpenGL ID
}


HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active = TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen = TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

							//////////////////////////////////////
							//The tVector3 Struct
							//////////////////////////////////////
typedef struct tVector3					// expanded 3D vector struct
{
	tVector3() {}	// constructor
	tVector3(float new_x, float new_y, float new_z) // initialize constructor	 
	{
		x = new_x; y = new_y; z = new_z;
	}
	// overload + operator so that we easier can add vectors
	tVector3 operator+(tVector3 vVector) { return tVector3(vVector.x + x, vVector.y + y, vVector.z + z); }
	// overload - operator that we easier can subtract vectors
	tVector3 operator-(tVector3 vVector) { return tVector3(x - vVector.x, y - vVector.y, z - vVector.z); }
	// overload * operator that we easier can multiply by scalars
	tVector3 operator*(float number) { return tVector3(x*number, y*number, z*number); }
	// overload / operator that we easier can divide by a scalar
	tVector3 operator/(float number) { return tVector3(x / number, y / number, z / number); }

	float x, y, z;						// 3D vector coordinates
}tVector3;


//////////////////////////////////////
//The CCamera Class
//////////////////////////////////////
class CCamera
{
public:

	tVector3 mPos;
	tVector3 mView;
	tVector3 mUp;

	// This function let you control the camera with the mouse
	void Mouse_Move(int wndWidth, int wndHeight) {
		POINT mousePos;
		int mid_x = wndWidth >> 1;
		int mid_y = wndHeight >> 1;
		float angle_y = 0.0f;
		float angle_z = 0.0f;

		GetCursorPos(&mousePos);	// Get the 2D mouse cursor (x,y) position					

		if ((mousePos.x == mid_x) && (mousePos.y == mid_y)) return;

		SetCursorPos(mid_x, mid_y);	// Set the mouse cursor in the center of the window						

									// Get the direction from the mouse cursor, set a resonable maneuvering speed
		angle_y = (float)((mid_x - mousePos.x)) / 1000;
		angle_z = (float)((mid_y - mousePos.y)) / 1000;

		// The higher the value is the faster the camera looks around.
		mView.y += angle_z * 3;

		// limit the rotation around the x-axis
		if ((mView.y - mPos.y) > 8)  mView.y = mPos.y + 8;
		if ((mView.y - mPos.y) <-8)  mView.y = mPos.y - 8;

		Rotate_View(-angle_y); // Rotate
	}

	void Strafe_Camera(float speed)
	{
		tVector3 vVector = mView - mPos;	// Get the view vector
		tVector3 vOrthoVector;              // Orthogonal vector for the view vector

		vOrthoVector.x = -vVector.z;
		vOrthoVector.z = vVector.x;

		// left positive cameraspeed and right negative -cameraspeed.
		mPos.x = mPos.x + vOrthoVector.x * speed;
		mPos.z = mPos.z + vOrthoVector.z * speed;
		mView.x = mView.x + vOrthoVector.x * speed;
		mView.z = mView.z + vOrthoVector.z * speed;
	}
	void Move_Camera(float speed) {
		tVector3 vVector = mView - mPos;	// Get the view vector

											// forward positive cameraspeed and backward negative -cameraspeed.
		mPos.x = mPos.x + vVector.x * speed;
		mPos.z = mPos.z + vVector.z * speed;
		mView.x = mView.x + vVector.x * speed;
		mView.z = mView.z + vVector.z * speed;
	}
	//my edit
	void updown(float speed) {
		tVector3 vVector = mView - mPos;	// Get the view vector
											//my edit
		mPos.y = mPos.y + vVector.y * speed;
		mView.y = mView.y + vVector.y  * speed;
	}

	void Rotate_View(float speed) {
		tVector3 vVector = mView - mPos;	// Get the view vector

		mView.z = (float)(mPos.z + sin(speed)*vVector.x + cos(speed)*vVector.z);
		mView.x = (float)(mPos.x + cos(speed)*vVector.x - sin(speed)*vVector.z);
	}

	void Position_Camera(float pos_x, float pos_y, float pos_z,
		float view_x, float view_y, float view_z,
		float up_x, float up_y, float up_z) {
		mPos = tVector3(pos_x, pos_y, pos_z); // set position
		mView = tVector3(view_x, view_y, view_z); // set view
		mUp = tVector3(up_x, up_y, up_z); // set the up vector	
	}
};

void Keyboard_Input();
void Draw_Grid();
void draw_electron();
void jump();
void draw_colored_cube(GLfloat);
CCamera objCamera;
GLTexture BARK, Leaf;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

														// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	seconds = tstruct.tm_sec;
	minutes = tstruct.tm_min;
	hours = tstruct.tm_hour;
	strftime(buf, sizeof(buf), "%d", &tstruct);
	return buf;
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	glEnable(GL_COLOR_MATERIAL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/*glEnable(GL_TEXTURE_2D);
	model = new Model_3DS();
	model->Load("skybox//M_TREE5.3DS");
	Leaf.LoadBMP("skybox//123.bmp");
	model->Materials[0].tex = Leaf;
	model->Materials[0].textured = false;*/
	//BARK.LoadBMP("skybox//leaf.bmp");
	//model->Materials[1].tex = BARK;
	//model->Materials[1].textured = false;
	

	//load the texture
	skybox1[3] = LoadTexture("sky/Front.bmp", 256);
	skybox1[2] = LoadTexture("sky/Back.bmp", 256);
	skybox1[0] = LoadTexture("sky/Left.bmp", 256);
	skybox1[1] = LoadTexture("sky/Right.bmp", 256);
	skybox1[4] = LoadTexture("sky/Bottom.bmp", 256);
	skybox1[5] = LoadTexture("sky/Top.bmp", 256);

	skybox2[3] = LoadTexture("skynight/Front.bmp", 256);
	skybox2[2] = LoadTexture("skynight/Back.bmp", 256);
	skybox2[0] = LoadTexture("skynight/Left.bmp", 256);
	skybox2[1] = LoadTexture("skynight/Right.bmp", 256);
	skybox2[4] = LoadTexture("skynight/Bottom.bmp", 256);
	skybox2[5] = LoadTexture("skynight/Top.bmp", 256);

	texture[6] = LoadTexture("skybox/asp.bmp", 256);
	texture[7] = LoadTexture("skybox/grass.bmp", 256);
	texture[8] = LoadTexture("skybox/mall.bmp", 256);
	texture[9] = LoadTexture("skybox/RoadSides.bmp", 256);
	texture[10] = LoadTexture("skybox/building.bmp", 256);
	texture[11] = LoadTexture("skybox/building2.bmp", 256);
	texture[12] = LoadTexture("skybox/building4.bmp", 256);
	texture[13] = LoadTexture("skybox/building5.bmp", 256);
	texture[14] = LoadTexture("skybox/building6.bmp", 256);
	texture[15] = LoadTexture("skybox/clock.bmp", 256);
	texture[16] = LoadTexture("skybox/bricks.bmp", 256);
	texture[17] = LoadTexture("skybox/rolldoor.bmp", 256);
	texture[18] = LoadTexture("skybox/wall1.bmp", 256);
	texture[19] = LoadTexture("skybox/wood1.bmp", 256);
	texture[20] = LoadTexture("skybox/metal1.bmp", 256);
	texture[21] = LoadTexture("skybox/parking.bmp", 256);
	texture[22] = LoadTexture("skybox/ceramic.bmp", 256);
	texture[23] = LoadTexture("skybox/glassface1.bmp", 256);
	texture[24] = LoadTexture("skybox/glassface2.bmp", 256);
	texture[25] = LoadTexture("skybox/rightglass.bmp", 256);
	texture[26] = LoadTexture("skybox/floor2.bmp", 256);
	texture[27] = LoadTexture("skybox/pizza.bmp", 256);
	texture[28] = LoadTexture("skybox/bar.bmp", 256);
	texture[29] = LoadTexture("skybox/parksign.bmp", 256);
	texture[30] = LoadTexture("skybox/shop1.bmp", 256);
	texture[31] = LoadTexture("skybox/shop2.bmp", 256);
	texture[32] = LoadTexture("skybox/shop3.bmp", 256);
	texture[33] = LoadTexture("skybox/shop4.bmp", 256);
	texture[34] = LoadTexture("skybox/shop5.bmp", 256);
	texture[35] = LoadTexture("skybox/arrow.bmp", 256);
	texture[36] = LoadTexture("skybox/name.bmp", 256);

	currentDateTime();


	objCamera.Position_Camera(0, 2.5f, 5, 0, 2.5f, 0, 0, 1, 0);  // Position      View(target)  Up

	ISoundEngine* engine = createIrrKlangDevice();

	if (!engine)
	{
		return 0; // error starting up the engine
	}

	engine->play2D("skybox/park.ogg", true);

																 //glFrontFace(GL_CCW);
																 //glPolygonMode(GL_FRONT, GL_FILL);
																 //glEnable(GL_CULL_FACE);
																 //glCullFace(GL_BACK);

	return TRUE;										// Initialization Went OK
}


void skybox(void) {
	float x = 0;
	float y = 0;
	float z = 0;
	float width = 2000;
	float height = 700;
	float length = 2000;
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);

	// Center the skybox
	x = x - width / 2;
	y = y - height / 2;
	z = z - length / 2;
	if (!day) {
		glBindTexture(GL_TEXTURE_2D, skybox2[0]);
	}
	else if (day) {
		glBindTexture(GL_TEXTURE_2D, skybox1[0]);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	if (!day) {
		glBindTexture(GL_TEXTURE_2D, skybox2[1]);
	}
	else if (day) {
		glBindTexture(GL_TEXTURE_2D, skybox1[1]);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glEnd();

	if (!day) {
		glBindTexture(GL_TEXTURE_2D, skybox2[4]);
	}
	else if (day) {
		glBindTexture(GL_TEXTURE_2D, skybox1[4]);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, -20, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, -20, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, -20, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, -20, z);
	glEnd();

	if (!day) {
		glBindTexture(GL_TEXTURE_2D, skybox2[5]);
	}
	else if (day) {
		glBindTexture(GL_TEXTURE_2D, skybox1[5]);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
	glEnd();

	if (!day) {
		glBindTexture(GL_TEXTURE_2D, skybox2[2]);
	}
	else if (day) {
		glBindTexture(GL_TEXTURE_2D, skybox1[2]);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
	glEnd();

	if (!day) {
		glBindTexture(GL_TEXTURE_2D, skybox2[3]);
	}
	else if (day) {
		glBindTexture(GL_TEXTURE_2D, skybox1[3]);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}
void Fan_Physics(void);
void Fan_Render(void);
void box(float lngh, float width, float depth);
void Fan_Physics(void)
{
	/*
	if (running == 1)
	speed = speed + 50;
	if (speed > 360.0)
	speed = 360.0;
	if (running == 0)
	speed = speed - 30;
	if (speed < 0)
	speed = 0;
	spin = spin + speed / 100;*/
	if (running == 1)
		spin = spin + 50;

}
void Fan_Render(void)
{
	glPushMatrix();
	/* Fan*/
	glPushMatrix();
	GLfloat mat_amb_diff_color_red[] = { 1.0, 0.5, 0.0, 0.5 };
	GLfloat mat_amb_diff_color_green[] = { 0.0, 1.0, 0.0, 0.5 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_color_red);
	//**********************************************
	// Fan Base:
	GLUquadric *qobj;
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricDrawStyle(qobj, GLU_FILL); /* smooth shaded */
	gluQuadricNormals(qobj, GLU_SMOOTH);
	glScalef(4, 0.5, 4);
	glEnable(GL_NORMALIZE);
	//Sphere with radius 0.25 then scaled
	gluSphere(qobj, 0.25, 20, 20);
	glPopMatrix();
	// Fan stand:
	glPushMatrix();
	gluQuadricDrawStyle(qobj, GLU_FILL); /* flat shaded */
	gluQuadricNormals(qobj, GLU_FLAT);
	glRotatef(-90, 1.0, 0.0, 0.0);
	gluCylinder(qobj, 0.125, 0.125, 3, 16, 10);

	glPopMatrix();
	// Fan Motor:
	glPushMatrix();
	gluQuadricDrawStyle(qobj, GLU_FILL); /* smooth shaded */
	gluQuadricNormals(qobj, GLU_SMOOTH);
	glTranslatef(0.0, 3.0, 0.0);
	glScalef(0.5, 0.5, 1);
	glEnable(GL_NORMALIZE);
	gluSphere(qobj, 1, 20, 20);
	glPopMatrix();

	//**********************************************
	// Fan Blades:
	glTranslatef(0.0, 3.0, 0.5);
	glRotatef(spin, 0.0, 0.0, 1.0);
	for (int i = 1; i <= 360; i = i + 60)

	{
		glPushMatrix();
		glRotatef(i, 0.0, 0.0, 1.0);
		glTranslatef(1.5, 0.0, 0.0);
		glRotatef(-45, 1.0, 0.0, 0.0);
		glShadeModel(GL_FLAT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		//glMaterialfv_p(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_color);
		glPushMatrix();
		/*calling Box ie: drawing the Blade of the fan*/
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_color_green);
		box(1.0, 0.3, 0.01);
		//glEnable(GL_LIGHTING);
		glPopMatrix();
		glPopMatrix();

	}
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}
void box(float lngh, float width, float depth)
{
	float a = lngh; float b = width; float c = depth;


	glBegin(GL_QUADS);

	/* Top face of box*/
	//glColor3f(1.0,0.0,0.0);
	glVertex3f(a, b, -c);     // Top right vertex (Top of cube)
	glVertex3f(-a, b, -c);    //Top left vertex (Top of cube)
	glVertex3f(-a, b, c);     // Bottom left vertex (Top of cube)
	glVertex3f(a, b, c);      // Bottom right vertex (Top of cube)

							  // Bottom face of box
							  // glColor3f(1.0,0.0,0.0);
	glVertex3f(a, -b, -c);     // Top right vertex (Bottom of cube)
	glVertex3f(-a, -b, -c);    // Top left vertex (Bottom of cube)
	glVertex3f(-a, -b, c);   // Bottom left vertex (Bottom of cube)
	glVertex3f(a, -b, c);   // Bottom right vertex (Bottom of cube)
	glColor3f(1.0, 0.0, 0.0);
	// Front of box
	//glColor3f(1.0,0.0,0.0);
	glVertex3f(a, b, c);      // Top right vertex (Front)
	glVertex3f(-a, b, c);     // Top left vertex (Front)
	glVertex3f(-a, -b, c);    // Bottom left vertex (Front)
	glVertex3f(a, -b, c);     // Bottom right vertex (Front)
	glColor3f(1.0, 0.0, 0.0);
	// Back of box
	// glColor3f(1.0,0.0,0.0);
	glVertex3f(a, -b, -c);    // Bottom right vertex (Back)
	glVertex3f(-a, -b, -c);   // Bottom left vertex (Back)
	glVertex3f(-a, b, -c);    // top left vertex (Back)
	glVertex3f(a, b, -c);     // Top right vertex (Back)
	glColor3f(1.0, 0.0, 0.0);
	// Left of box
	//glColor3f(1.0,0.0,0.0);
	glVertex3f(-a, b, c);     // Top right vertex (Left)
	glVertex3f(-a, b, -c);    // Top left vertex (Left)
	glVertex3f(-a, -b, -c);   // Bottom left vertex (Left)
	glVertex3f(-a, -b, c);    // Bottom vertex (Left)
	glColor3f(1.0, 0.0, 0.0);
	// Right of box
	//glColor3f(1.0,0.0,0.0);
	glVertex3f(a, b, -c);     // Top right vertex (Right)
	glVertex3f(a, b, c);      // Top left vertex (Right)
	glVertex3f(a, -b, c);     // Bottom left vertex (Right)
	glVertex3f(a, -b, -c);    // Bottom right vertex (Right)
	glColor3f(1.0, 0.0, 0.0);
	// End drawing the box
	glEnd();
	//return TRUE;

}
void Keyboard_Input()
{
	if ((GetKeyState('W') & 0x80))
	{
		objCamera.Move_Camera(CAMERASPEED);
	}

	if ((GetKeyState('S') & 0x80))
	{
		objCamera.Move_Camera(-CAMERASPEED);
	}

	if ((GetKeyState(VK_LEFT) & 0x80) || (GetKeyState('A') & 0x80))
	{
		objCamera.Strafe_Camera(-CAMERASPEED); // or   objCamera.Rotate_View(-CAMERASPEED); to rotate
	}

	if ((GetKeyState(VK_RIGHT) & 0x80) || GetKeyState('D') & 0x80)
	{
		objCamera.Strafe_Camera(CAMERASPEED); //or   objCamera.Rotate_View(CAMERASPEED); to rotate
	}

	if (GetKeyState('E') & 0x80)
	{
		objCamera.Rotate_View(CAMERASPEED);
	}
	if (GetKeyState('Q') & 0x80)
	{
		objCamera.Rotate_View(-CAMERASPEED);
	}

	if (GetKeyState('L') & 0x80) {
		K++;
		if (night == 0)
		{
			night = 1;
			day = false;

		}
		else if (night == 1) {
			night = 0;
			day = true;
		}
	}

	if (GetKeyState('U') & 0x80)
	{
		objCamera.updown(1);
	}

	if (GetKeyState('I') & 0x80)
	{
		objCamera.updown(-1);
	}

	if ((GetKeyState(' ') & 0x80) && s == walk)
	{
		s = jump_up;

	}

	if ((GetKeyState('O') & 0x80) && door == stop && rolldoor<2)
	{
		door = roll_up;
	}

	if ((GetKeyState('O') & 0x80) && door == stop && rolldoor>3)
	{
		door = roll_down;
	}

	if ((GetKeyState('M') & 0x80) && malldoor == stopped && rightdoor>0)
	{
		malldoor = close;
	}
	if ((GetKeyState('M') & 0x80) && malldoor == stopped && (leftdoor==0||rightdoor==0))
	{
		malldoor = open;
	}

	if ((GetKeyState('1') & 0x80))
	{
		rtfan += 5;
		rtchar += 5;
	}

	if ((GetKeyState('2') & 0x80))
	{
		rtfan -= 5;
		rtchar -= 5;
	}

	if ((GetKeyState('3') & 0x80))
	{
		if (running == 0)
		{
			running = 1;
		}
		else if (running == 1) {
			running = 0;
		}
	}

}
void mouse(int mouseX, int mouseY, bool isClicked, bool isRClicked)
{
	if (isClicked) {
		objCamera.Move_Camera(CAMERASPEED);
		//h = float((mouseX - 320) * 10) / 640;
	}
	if (isClicked) {
		//v = float((mouseY - 320) * 10) / 640;
	}
}
void Draw_Grid()
{

	for (float i = -500; i <= 500; i += 5)
	{
		glBegin(GL_LINES);
		glColor3ub(150, 190, 150);
		glVertex3f(-500, 0, i);
		glVertex3f(500, 0, i);
		glVertex3f(i, 0, -500);
		glVertex3f(i, 0, 500);
		glEnd();
	}
}
void draw_tree() {
	GLUquadric *qobj;
	qobj = gluNewQuadric();

	glPushMatrix();
	glColor3f(0.1, 0.3, 0);
	glTranslated(0, 4, 0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(qobj, 2.0, 0, 3, 32, 70);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 1, 0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(qobj, 3.0, 0, 4, 32, 70);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, -1, 0);
	glRotatef(-90, 1, 0, 0);
	gluCylinder(qobj, 1.0, 1, 2, 32, 70);
	glPopMatrix();
}
void draw_circle(float x, float y, float z, float r, int s) {
	glEnable(GL_POINT_SMOOTH);
	glPointSize(s);
	glBegin(GL_POINTS);

	for (float i = 0; i <= 2 * PI; i += 0.01)
		glVertex3f(x + r*sin(i), y + r*cos(i), z);
	glEnd();
	glDisable(GL_POINT_SMOOTH);
}
void draw_sphere(float x, float y, float z, float r) {
	glEnable(GL_POINT_SMOOTH);
	glPointSize(5);
	glBegin(GL_POINTS);
	for (float theta = 0; theta <= 2 * PI; theta += 0.2)
	{
		for (float alpha = -PI; alpha <= PI; alpha += 0.3)
		{
			glVertex3f(x + (r*cos(alpha))*cos(theta),
				y + (r*cos(alpha))*sin(theta),
				z + r*sin(alpha));
		}
	}
	glEnd();
	glDisable(GL_POINT_SMOOTH);
}
void draw_electron() {


	glRotatef(R, 0, 0, 1);
	if (GetKeyState('R'))
	{
		R += 0.3;
		R2--;
	}



	GLUquadric *quadric;
	quadric = gluNewQuadric();


	glPushMatrix();
	glRotatef(R2, 0, 0, 1);
	glColor3f(0.2, 0.2, 0.2);
	glTranslatef(0, 0, 0.25);
	gluSphere(quadric, 0.5, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glRotatef(R2, 0, 0, 1);
	glColor3f(0.7, 0.04, 0.04);
	glTranslatef(-0.25, 0.25, 0);
	gluSphere(quadric, 0.5, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glRotatef(R2, 0, 0, 1);
	glColor3f(0.27, 0.27, 0.27);
	glTranslatef(-0.25, -0.25, 0);
	gluSphere(quadric, 0.5, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glRotatef(R2, 0, 0, 1);
	glColor3f(0.7, 0.02, 0.02);
	glTranslatef(0.27, 0, 0);
	gluSphere(quadric, 0.5, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glRotatef(R2, 0, 0, 1);
	glColor3f(0.3, 0.3, 0.3);
	glTranslatef(0.25, 0.27, 0);
	gluSphere(quadric, 0.5, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glRotatef(R2, 0, 0, 1);
	glColor3f(0.7, 0, 0);
	glTranslatef(0.25, -0.27, 0);
	gluSphere(quadric, 0.5, 32, 32);
	glPopMatrix();


	glColor3f(1, 1, 1);

	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	draw_circle(0, 0, 0, 3, 3);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, 1, 1, 0);
	draw_circle(0, 0, 0, 3, 3);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90, -1, 1, 0);
	draw_circle(0, 0, 0, 3, 3);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 0);
	glRotatef(angle, 0, 1, 0);
	glTranslatef(3, 0, 0);
	gluSphere(quadric, 0.2, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 0);
	glRotatef(angle, 1, 1, 0);
	glTranslatef(0, 0, 3);
	gluSphere(quadric, 0.2, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1, 1, 0);
	glRotatef(angle, -1, 1, 0);
	glTranslatef(0, 0, -3);
	gluSphere(quadric, 0.2, 32, 32);
	glPopMatrix();



	angle += 2;
}
void Draw_Pyramid(float r, float g, float b)
{
	glScalef(0.3f, 1.0f, 0.3f);
	glTranslatef(0, 2.0f, 0);
	glBegin(GL_TRIANGLES);
	glColor3f(r, g, b);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glVertex3f(-4.0f, -4.0f, 4.0f);
	glVertex3f(4.0f, -4.0f, 4.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glVertex3f(4.0f, -4.0f, 4.0f);
	glVertex3f(4.0f, -4.0f, -4.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glVertex3f(4.0f, -4.0f, -4.0f);
	glVertex3f(-4.0f, -4.0f, -4.0f);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glVertex3f(-4.0f, -4.0f, -4.0f);
	glVertex3f(-4.0f, -4.0f, 4.0f);
	glEnd();
}
void draw_colored_cube(GLfloat size) {

	glEnable(GL_SMOOTH);
	glBegin(GL_QUADS);
	glVertex3f(size, size, -size);
	glVertex3f(-size, size, -size);
	glVertex3f(-size, size, size);
	glVertex3f(size, size, size);

	glVertex3f(size, -size, size);
	glVertex3f(-size, -size, size);
	glVertex3f(-size, -size, -size);
	glVertex3f(size, -size, -size);

	glVertex3f(size, size, size);
	glVertex3f(-size, size, size);
	glVertex3f(-size, -size, size);
	glVertex3f(size, -size, size);

	glVertex3f(size, -size, -size);
	glVertex3f(-size, -size, -size);
	glVertex3f(-size, size, -size);
	glVertex3f(size, size, -size);

	glVertex3f(-size, size, size);
	glVertex3f(-size, size, -size);
	glVertex3f(-size, -size, -size);
	glVertex3f(-size, -size, size);

	glVertex3f(size, size, -size);
	glVertex3f(size, size, size);
	glVertex3f(size, -size, size);
	glVertex3f(size, -size, -size);
	glEnd();
	glDisable(GL_SMOOTH);
}
void jump() {
	if (s == jump_up)
	{
		eyey += 1.5;
		if (eyey > jumpLimit) { s = jump_down; }
	}

	else if (s == jump_down)
	{
		eyey -= 1.5;
		if (eyey < 0.1) { s = walk; }
	}
}
void draw_light() {

	GLUquadric *qobj;
	qobj = gluNewQuadric();
	glPushMatrix();
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glColor3f(0.1, 0.1, 0.1);
	gluCylinder(qobj, 0.3, 0.3, 10, 32, 70);
	glPopMatrix();
	glTranslatef(0, 10, 0);
	glColor3f(1, 1, 1);

	if (night == 1) {
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Diffuse1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, Light_Specular1);
		glLightfv(GL_LIGHT1, GL_POSITION, Light_Position1);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Light_Spot_direction1);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, Light_CutOff1);

		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
	}
	else if (night == 0) {
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT0);
	}

	draw_sphere(0, 0, 0, 0.7);

	glPopMatrix();
}
void draw_traffic_signal() {
	GLUquadric *qobj;
	qobj = gluNewQuadric();
	glPushMatrix();

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glColor3f(0.1, 0.1, 0.1);
	gluCylinder(qobj, 0.3, 0.3, 10, 32, 70);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 10, 0);
	glRotatef(90, 0, 1, 0);
	glColor3f(0, 0, 0);
	gluCylinder(qobj, 0.3, 0.3, 6, 32, 70);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(6, 10.5, 0);
	glRotatef(90, 1, 0, 0);
	gluCylinder(qobj, 0.3, 0.3, 2, 32, 70);

	glColor3f(1, 0, 0);
	glTranslatef(0, -0.3, 0.3);
	gluSphere(qobj, 0.25, 32, 70);

	glColor3f(1, 0.35, 0);
	glTranslatef(0, 0, 0.6);
	gluSphere(qobj, 0.25, 32, 70);

	glColor3f(0, 1, 0);
	glTranslatef(0, 0, 0.6);
	gluSphere(qobj, 0.25, 32, 70);

	glPopMatrix();

	if (night == 1) {


		glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Diffuse1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, Light_Specular1);
		glLightfv(GL_LIGHT1, GL_POSITION, Light_Position1);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Light_Spot_direction1);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, Light_CutOff1);

		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
	}
	else if (night == 0) {
		glDisable(GL_LIGHT1);
		glEnable(GL_LIGHT0);
	}


	glPopMatrix();
}
void draw_road(float width,float dis) {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTranslated(0, -3, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0, dis);
	glTexCoord2f(1.0f, 3.0f); glVertex3f(width, 0, -dis);
	glTexCoord2f(0.0f, 3.0f); glVertex3f(-width, 0, -dis);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0, dis);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void draw_floor2(float width, float dis) {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTranslated(0, -3, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, texture[26]);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(40.0f, 0.0f); glVertex3f(width, 0, dis);
	glTexCoord2f(40.0f, 60.0f); glVertex3f(width, 0, -dis);
	glTexCoord2f(0.0f, 60.0f); glVertex3f(-width, 0, -dis);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0, dis);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void draw_grass() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTranslated(10, -3, 0);
	glColor3f(1, 1, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glBegin(GL_QUADS);
	glTexCoord2f(25.0f, 0.0f); glVertex3f(1000, 0, 1000);
	glTexCoord2f(25.0f, 25.0f); glVertex3f(1000, 0, -1000);
	glTexCoord2f(0.0f, 25.0f); glVertex3f(0, 0, -1000);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 1000);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void draw_floor() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTranslated(-10, -3, 0);
	glColor3f(1, 1, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glBegin(GL_QUADS);
	glTexCoord2f(30.0f, 0.0f); glVertex3f(-1000, 0, 1000);
	glTexCoord2f(30.0f, 35.0f); glVertex3f(-1000, 0, -1000);
	glTexCoord2f(0.0f, 35.0f); glVertex3f(0, 0, -1000);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 1000);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void draw_car(float r, float g, float b) {
	glPushMatrix();

	glBegin(GL_QUADS);
	GLUquadric *t;
	t = gluNewQuadric();
	/* top */
	glColor3f(r, g, b);
	glVertex3f(0.2, 0.4, 0.6);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.2, 0.4, 0.2);

	/* bottom*/
	glVertex3f(0.2, 0.4, 0.6);
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(0.2, 0.2, 0.2);

	/* front of cube*/
	glVertex3f(0.2, 0.2, 0.6);
	glVertex3f(0.2, 0.4, 0.6);
	glVertex3f(0.2, 0.4, 0.2);
	glVertex3f(0.2, 0.2, 0.2);

	/* back*/
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.6, 0.2, 0.2);

	/* left*/
	glVertex3f(0.2, 0.2, 0.6);
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.2, 0.4, 0.6);

	/* Right*/
	glVertex3f(0.2, 0.2, 0.2);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.2, 0.4, 0.2);
	//****************************************************************************
	glVertex3f(0.7, 0.65, 0.6);
	glVertex3f(0.7, 0.65, 0.2);
	glVertex3f(1.7, 0.65, 0.2);        //top cover
	glVertex3f(1.7, 0.65, 0.6);
	//***************************back guard******************************
	glColor3f(0, 0, 0);
	glVertex3f(1.8, 0.5, 0.6);
	glVertex3f(1.8, 0.5, 0.2);
	glVertex3f(2., 0.4, 0.2);
	glVertex3f(2., 0.4, 0.6);

	glColor3f(r, g, b);

	/* bottom*/
	glVertex3f(2, 0.2, 0.6);
	glVertex3f(2, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.6);
	glVertex3f(1.8, 0.2, 0.6);

	/* back*/
	glVertex3f(2, 0.4, 0.6);
	glVertex3f(2, 0.4, 0.2);
	glVertex3f(2, 0.2, 0.2);
	glVertex3f(2, 0.2, 0.6);

	/* left*/
	glVertex3f(1.8, 0.2, 0.2);
	glVertex3f(1.8, 0.5, 0.2);
	glVertex3f(2, 0.4, 0.2);
	glVertex3f(2, 0.2, 0.2);

	/* Right */
	glVertex3f(1.8, 0.2, 0.6);
	glVertex3f(1.8, 0.5, 0.6);
	glVertex3f(2, 0.4, 0.6);
	glVertex3f(2, 0.2, 0.6);

	//******************MIDDLE BODY************************************
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(1.8, 0.2, 0.6);
	glVertex3f(1.8, 0.5, 0.6);

	/* bottom*/
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.6);

	/* back */
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.2);
	glVertex3f(1.8, 0.5, 0.2);

	//*********************ENTER WINDOW**********************************
	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.77, 0.63, 0.2);
	glVertex3f(0.75, 0.5, 0.2);        //front window
	glVertex3f(1.2, 0.5, 0.2);
	glVertex3f(1.22, 0.63, 0.2);

	glVertex3f(1.27, 0.63, .2);
	glVertex3f(1.25, 0.5, 0.2);        //back window
	glVertex3f(1.65, 0.5, 0.2);
	glVertex3f(1.67, 0.63, 0.2);

	glColor3f(r, g, b);
	glVertex3f(0.7, 0.65, 0.2);
	glVertex3f(0.7, 0.5, .2);       //first separation
	glVertex3f(0.75, 0.5, 0.2);
	glVertex3f(0.77, 0.65, 0.2);

	glVertex3f(1.2, 0.65, 0.2);
	glVertex3f(1.2, 0.5, .2);       //second separation
	glVertex3f(1.25, 0.5, 0.2);
	glVertex3f(1.27, 0.65, 0.2);

	glVertex3f(1.65, 0.65, 0.2);
	glVertex3f(1.65, 0.5, .2);     //3d separation
	glVertex3f(1.7, 0.5, 0.2);
	glVertex3f(1.7, 0.65, 0.2);

	glVertex3f(0.75, 0.65, 0.2);
	glVertex3f(0.75, 0.63, 0.2);        //line strip
	glVertex3f(1.7, 0.63, 0.2);
	glVertex3f(1.7, 0.65, 0.2);

	glVertex3f(0.75, 0.65, 0.6);
	glVertex3f(0.75, 0.63, 0.6);        //line strip
	glVertex3f(1.7, 0.63, 0.6);
	glVertex3f(1.7, 0.65, 0.6);

	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.77, 0.63, 0.6);
	glVertex3f(0.75, 0.5, 0.6);        //front window
	glVertex3f(1.2, 0.5, 0.6);
	glVertex3f(1.22, 0.63, 0.6);

	glVertex3f(1.27, 0.63, .6);
	glVertex3f(1.25, 0.5, 0.6);        //back window
	glVertex3f(1.65, 0.5, 0.6);
	glVertex3f(1.67, 0.63, 0.6);

	glColor3f(r, g, b);
	glVertex3f(0.7, 0.65, 0.6);
	glVertex3f(0.7, 0.5, .6);       //first separation
	glVertex3f(0.75, 0.5, 0.6);
	glVertex3f(0.77, 0.65, 0.6);

	glVertex3f(1.2, 0.65, 0.6);
	glVertex3f(1.2, 0.5, .6);       //second separation
	glVertex3f(1.25, 0.5, 0.6);
	glVertex3f(1.27, 0.65, 0.6);

	glVertex3f(1.65, 0.65, 0.6);
	glVertex3f(1.65, 0.5, .6);
	glVertex3f(1.7, 0.5, 0.6);
	glVertex3f(1.7, 0.65, 0.6);
	glEnd();

	//**************************************************************
	glBegin(GL_QUADS);
	/* top */
	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.5, 0.2);        //front window
	glVertex3f(0.7, 0.65, 0.2);
	glVertex3f(0.7, 0.65, 0.6);

	glVertex3f(1.7, 0.65, .6);
	glVertex3f(1.7, 0.65, 0.2);        //back window
	glVertex3f(1.8, 0.5, 0.2);
	glVertex3f(1.8, 0.5, 0.6);


	glBegin(GL_TRIANGLES);

	/* top*/
	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.7, 0.65, 0.6);       //tri front window
	glVertex3f(0.7, 0.5, 0.6);

	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.7, 0.65, 0.2);       //tri front window
	glVertex3f(0.7, 0.5, 0.2);

	glVertex3f(1.7, 0.65, 0.2);
	glVertex3f(1.8, 0.5, 0.2);       //tri back window
	glVertex3f(1.7, 0.5, 0.2);

	glVertex3f(1.7, 0.65, 0.6);
	glVertex3f(1.8, 0.5, 0.6);       //tri back window
	glVertex3f(1.7, 0.5, 0.6);

	glEnd();
	//************Ashtman
	glPushMatrix();
	glColor3f(0.1, 0.1, 0.1);
	glTranslatef(1.55, 0.2, 0.3);
	glRotatef(90.0, 0, 1, 0);
	gluCylinder(t, 0.02, 0.03, .5, 10, 10);
	glPopMatrix();
	//********************WHEELS

	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	for (int theta = 0; theta<360; theta = theta + 40)
	{
		glVertex3f(0.6, 0.2, 0.62);
		glVertex3f(0.6 + (0.08*(cos(((theta + angle)*3.14) / 180))), 0.2 + (0.08*(sin(((theta + angle)*3.14) / 180))), 0.62);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int theta = 0; theta<360; theta = theta + 40)
	{
		glVertex3f(0.6, 0.2, 0.18);
		glVertex3f(0.6 + (0.08*(cos(((theta + angle)*3.14) / 180))), 0.2 + (0.08*(sin(((theta + angle)*3.14) / 180))), 0.18);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int theta = 0; theta<360; theta = theta + 40)
	{
		glVertex3f(1.7, 0.2, 0.18);
		glVertex3f(1.7 + (0.08*(cos(((theta + angle)*3.14) / 180))), 0.2 + (0.08*(sin(((theta + angle)*3.14) / 180))), 0.18);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int theta = 0; theta<360; theta = theta + 40)
	{
		glVertex3f(1.7, 0.2, 0.62);
		glVertex3f(1.7 + (0.08*(cos(((theta + angle)*3.14) / 180))), 0.2 + (0.08*(sin(((theta + angle)*3.14) / 180))), 0.62);
	}
	glEnd();

	//lights
	glPushMatrix();

	glColor3f(1, 1, 1);
	glTranslatef(0.2, 0.4, 0.5);
	gluSphere(t, 0.02, 11, 13);
	glTranslatef(0, 0, -0.2);
	gluSphere(t, 0.02, 11, 13);

	glColor3f(1, 0, 0);
	glTranslatef(1.81, -0.1, 0);
	glutSolidSphere(0.02, 11, 13);
	glTranslatef(0, 0, 0.2);
	glutSolidSphere(0.02, 11, 13);

	glPopMatrix();

	//Wheels
	glColor3f(0, 0, 0);
	glTranslatef(0.6, 0.2, 0.6);
	glutSolidTorus(0.025, 0.07, 10, 25);

	glTranslatef(0, 0, -0.4);
	glutSolidTorus(0.025, 0.07, 10, 25);

	glTranslatef(1.1, 0, 0);
	glutSolidTorus(0.025, 0.07, 10, 25);

	glTranslatef(0, 0, 0.4);
	glutSolidTorus(0.025, 0.07, 10, 25);
	glPopMatrix();
	//*************************************************************
	glPopMatrix();

}
void mall2(float width,float height,UINT t) {
	glPushMatrix();
	glEnable(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, t);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);

	//top
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width, height, -width); //1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, height, -width); //0 0
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, height, width);// 0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, width);//1 1

	//bottom
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width, 0, width);//1 1
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, 0, width);//0 1
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0, -width);//0 0
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0, -width);//1 0

	//back
	glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, width);//0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-width, height, width);// 1 1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-width, 0, width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0, width);//0 0

	
	//front
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0, -width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0, -width);//0 0
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, height, -width);// 0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, -width);// 1 1

	//left
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, height, width);//0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-width, height, -width);//1 1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-width, 0, -width);// 1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0, width);//0 0

	//right
	glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, -width);//0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, width);//1 1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0, width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0, -width);//0 0

	glEnd();

	glDisable(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void draw_garage(int width, int height, UINT building,UINT roof,UINT front,UINT lights,UINT land) {
	glPushMatrix();

	//top roof
	glPushMatrix();
	glTranslated(0, height, 0);
	mall2(width + 4, 2, roof);
	glPopMatrix();

	if (night == 1) {
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Diffuse1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, Light_Specular1);
		glLightfv(GL_LIGHT1, GL_POSITION, Light_Position1);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Light_Spot_direction1);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, Light_CutOff1);
	}

	//side lights
	{
		glPushMatrix();
		glTranslated(width - 2, (height/2)+6, -width - 1);
		glRotated(90, 1, 0, 0);
		mall2(1.5, 2, lights);
		glColor3f(1, 1, 1);
		draw_sphere(0, 0, 0, 0.5);
		glPopMatrix();

		glPushMatrix();
		glTranslated(-width + 2, (height/2)+6, -width -1);
		glRotated(90, 1, 0, 0);
		mall2(1.5, 2, lights);
		glColor3f(1, 1, 1);
		draw_sphere(0, 0, 0, 0.5);
		glPopMatrix();

	}



	glEnable(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);


	//glBindTexture(GL_TEXTURE_2D, t2);
	//glColor3f(1, 1, 1);
	//glBegin(GL_QUADS);
	////top
	//glTexCoord2f(1.0f, 0.0f); glVertex3f(width+3, height, -width); //1 0
	//glTexCoord2f(0.0f, 0.0f); glVertex3f(-width-3, height, -width); //0 0
	//glTexCoord2f(0.0f, 1.0f); glVertex3f(-width-3, height, width);// 0 1
	//glTexCoord2f(1.0f, 1.0f); glVertex3f(width+3, height, width);//1 1
	//glEnd();

	glBindTexture(GL_TEXTURE_2D, front);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	//front roll door
	
	if (door == roll_up)
	{
		rolldoor += 0.37;
		if (rolldoor > height) { door = stop; }
	}

	else if (door == roll_down)
	{
		rolldoor = rolldoor - 0.37;
		if (rolldoor < 0.1) { door = stop; }
	}
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width-4, rolldoor, -width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width+4, rolldoor, -width);//0 0
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width+4, height, -width);// 0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width-4, height, -width);// 1 1
	glEnd();

	glBindTexture(GL_TEXTURE_2D, building);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	//front edge1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0, -width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(width-4, 0, -width);//0 0
	glTexCoord2f(0.0f, 1.0f); glVertex3f(width-4, height, -width);// 0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, -width);// 1 1
	glEnd();

	glBindTexture(GL_TEXTURE_2D, building);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glPushMatrix();
	//front edge2
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-width+4, 0, -width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0, -width);//0 0
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, height, -width);// 0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-width+4, height, -width);// 1 1
	glPopMatrix();
	glEnd();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, land);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	//bottom
	glTexCoord2f(2.0f, 2.0f); glVertex3f(width, 0.2, width);//1 1
	glTexCoord2f(0.0f, 2.0f); glVertex3f(-width, 0.2, width);//0 1
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0.2, -width);//0 0
	glTexCoord2f(2.0f, 0.0f); glVertex3f(width, 0.2, -width);//1 0
	glEnd();


	glBindTexture(GL_TEXTURE_2D, building);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	//								 //bottom
	//glVertex3f(width, 0.2, width);//1 1
	//glVertex3f(-width, 0.2, width);//0 1
	//glVertex3f(-width, 0.2, -width);//0 0
	//glVertex3f(width, 0.2, -width);//1 0

								 //back
	glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, width);//0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-width, height, width);// 1 1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-width, 0, width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0, width);//0 0

																//left
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, height, width);//0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-width, height, -width);//1 1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-width, 0, -width);// 1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, 0, width);//0 0

														   //right
	glTexCoord2f(0.0f, 1.0f); glVertex3f(width, height, -width);//0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width, height, width);//1 1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0, width);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(width, 0, -width);//0 0

	glEnd();
	glDisable(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void drawTime(float s)
{
	glPushMatrix();
	hours = hours % 12;
	glScalef(s, s, s);
	double w = 17;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[15]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2f(0, 0); glVertex3f(-w, -w, 0);
	glTexCoord2f(1, 0); glVertex3f(w, -w, 0);
	glTexCoord2f(1, 1); glVertex3f(w, w, 0);
	glTexCoord2f(0, 1); glVertex3f(-w, w, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.01, 0, 0);
	glColor3d(0.2, .2, .3);
	glutSolidTorus(5*s, 22*s, 30, 40);
	//glutSolidTorus(4*s, 23*s, 10, 6);
	glPopMatrix();

	glPushMatrix();
	//draw_circle(0,0,0,5,5);
	//glNormal3d(0, 0, 1);
	glRotated(seconds / 60.0 * 360, 0, 0, -1);
	glColor3d(.5, .5, 1);
	glScalef(s, s, s);
	glBegin(GL_QUADS);
	glVertex3d(0, 14, 0);
	glVertex3d(-.5, 0, 0);
	glVertex3d(0, -.5, 0);
	glVertex3d(.5, 0, 0);
	glEnd();
	//glRotated(-seconds / 60.0 * 360, 0, 0, -1);
	glPopMatrix();

	glPushMatrix();
	glColor3d(0, 0, 0);
	glRotated(minutes / 60.0 * 360, 0, 0, -1);
	glColor3d(0, 0, 0);
	//glNormal3d(0, 0, 1);
	glScalef(s, s, s);
	glBegin(GL_QUADS);
	glVertex3d(0, 14, 0);
	glVertex3d(-1, 0, 0);
	glVertex3d(0, -1, 0);
	glVertex3d(1, 0, 0);
	glEnd();
	//glRotated(-minutes / 60.0 * 360 + hours / 12.0 * 360 + minutes / 60.0 * 30, 0, 0, -1);
	glPopMatrix();

	glPushMatrix();
	glRotated(hours / 12.0 * 360, 0, 0, -1);
	glColor3d(0, 0, 0);
	//glNormal3d(0, 0, 1);
	glScalef(s, s, s);
	glBegin(GL_QUADS);
	glVertex3d(0, 8, 0);
	glVertex3d(-1, 0, 0);
	glVertex3d(0, -1, 0);
	glVertex3d(1, 0, 0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3d(.7, .7, .7);
	glTranslated(0, 0, -1*s);
	glutSolidCone(1*s, 2*s, 10, 2);

	glPopMatrix();
}
class mall
{
public:
	void firstfloor(float loc_x, float loc_y, float loc_z,UINT roof,UINT ground,UINT sides,UINT front, UINT glassdoor)
	{
		glPushMatrix();
		glEnable(GL_SMOOTH);
		glEnable(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, roof);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		//Top face
		glTexCoord2f(0, 7);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(7, 0);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(7, 7);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, ground);
		glBegin(GL_QUADS);
		// Bottom face
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 10);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(10, 0);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(10, 10);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glEnd();

		// Back face
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, front);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 5);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(5, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(5, 5);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glEnd();

		// Left face
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, sides);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 7);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 7);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();

		// Right face
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, sides);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 7);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 0);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 7);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();

		if (malldoor == open)
		{
			leftdoor -= 2.5;
			rightdoor += 2.5;
			if (rightdoor >= loc_x/2-25) { malldoor = stopped; }
			if (leftdoor <= -loc_x / 2 + 25) { malldoor = stopped; }
		}

		else if (malldoor == close)
		{
			rightdoor -= 2.5;
			leftdoor += 2.5;
			if (rightdoor <= 0) { malldoor = stopped; }
			if (leftdoor >= 0) { malldoor = stopped; }
		}


		//front door right
		glBindTexture(GL_TEXTURE_2D, glassdoor);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 1);
		glVertex3f(loc_x / 2 -25, loc_y / 2, loc_z / 2);
		glTexCoord2f(1, 1);
		glVertex3f(rightdoor, loc_y / 2, loc_z / 2);
		glTexCoord2f(1, 0);
		glVertex3f(rightdoor, -loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(loc_x / 2 -25, -loc_y / 2, loc_z / 2);
		glEnd();

		//front door left
		glBindTexture(GL_TEXTURE_2D, glassdoor);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(1, 1);
		glVertex3f(leftdoor, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 1);
		glVertex3f(-loc_x / 2 + 25, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2 + 25, -loc_y / 2, loc_z / 2);
		glTexCoord2f(1, 0);
		glVertex3f(leftdoor, -loc_y / 2, loc_z / 2);
		glEnd();

		glPushMatrix();
		glTranslatef(loc_x / 2 -12.5 ,-loc_y/2 ,loc_z/2);
		mall2(12.5, loc_y , roof);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-loc_x / 2+12.5, -loc_y/2, loc_z / 2);
		mall2(12.5, loc_y, roof);
		glPopMatrix();

		

		glDisable(GL_SMOOTH);
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	}
	void scndfloor(float loc_x, float loc_y, float loc_z, UINT roof, UINT ground, UINT sides, UINT front, UINT glassdoor) {
		glPushMatrix();
		glEnable(GL_SMOOTH);
		glEnable(GL_TEXTURE_2D);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, roof);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		//Top face
		glTexCoord2f(0, 7);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(7, 0);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(7, 7);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, ground);
		glBegin(GL_QUADS);
		// Bottom face
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 10);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(10, 0);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(10, 10);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glEnd();

		// Back face
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, front);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 5);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(5, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(5, 5);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glEnd();
		glBegin(GL_QUADS);

		// Left face
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, sides);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 7);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 7);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, sides);
		glBegin(GL_QUADS);
		// Right face
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 7);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 0);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(7, 7);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();
		//front face
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, front);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 5);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(5, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(5, 5);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();

		glDisable(GL_SMOOTH);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
};
class room
{
public:
	room(float loc_x, float loc_y, float loc_z,UINT roof,UINT sides,UINT front)
	{
		glPushMatrix();
		glEnable(GL_SMOOTH);
		glEnable(GL_TEXTURE_2D);

		//Top face
		glBindTexture(GL_TEXTURE_2D, roof);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 10);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(10, 0);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(10, 10);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glEnd();


		// Bottom face
		glBindTexture(GL_TEXTURE_2D, texture[22]);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(10, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(10, 10);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 10);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glEnd();

		// Back face
		glBindTexture(GL_TEXTURE_2D, texture[23]);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glTexCoord2f(5, 5);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 5);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(5, 0);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glEnd();

		//Front Face
		glBindTexture(GL_TEXTURE_2D, front);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 1);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glTexCoord2f(1, 0);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();


		// Left face
		glBindTexture(GL_TEXTURE_2D, sides);
		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(5, 0);
		glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(5, 5);
		glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 5);
		glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();

		// Right face
		glBindTexture(GL_TEXTURE_2D, sides);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 0);
		glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
		glTexCoord2f(5, 0);
		glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
		glTexCoord2f(5, 5);
		glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
		glTexCoord2f(0, 5);
		glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
		glEnd();

		glDisable(GL_SMOOTH);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}


};
void draw_sign(UINT t) {
	glPushMatrix();
	GLUquadric *qobj;
	qobj = gluNewQuadric();

	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glColor3f(0.1, 0.1, 0.1);
	gluCylinder(qobj, 0.1, 0.1, 10, 32, 70);
	glPopMatrix();

	glTranslatef(0, 10, 0); 

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_SMOOTH);
	glBindTexture(GL_TEXTURE_2D, t);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(2, 0, 0);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);//0 0
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0, 2, 0);// 0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(2, 2, 0);// 1 1
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_SMOOTH);

	glPopMatrix();
}
float xx, yy, zz;
class mas3ad {
public:
	mas3ad(float loc_x, float loc_y, float loc_z);
	void upmas3ad();
};

mas3ad::mas3ad(float loc_x, float loc_y, float loc_z) {
	glPushMatrix();
	upmas3ad();
	glBegin(GL_QUADS);
	//Top face
	glColor3f(0.0f, 0.5f, 0.0f);
	glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
	glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
	glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
	glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);

	// Bottom face
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
	glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);
	glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
	glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);

	// Back face
	glColor3f(.7f, .1f, 0.1f);     // Yellow
	glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
	glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
	glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
	glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);

	// Left face
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-loc_x / 2, loc_y / 2, loc_z / 2);
	glVertex3f(-loc_x / 2, loc_y / 2, -loc_z / 2);
	glVertex3f(-loc_x / 2, -loc_y / 2, -loc_z / 2);
	glVertex3f(-loc_x / 2, -loc_y / 2, loc_z / 2);

	// Right face
	glColor3f(0.5, 0.2, 1.0f);     // Pink
	glVertex3f(loc_x / 2, loc_y / 2, loc_z / 2);
	glVertex3f(loc_x / 2, loc_y / 2, -loc_z / 2);
	glVertex3f(loc_x / 2, -loc_y / 2, -loc_z / 2);
	glVertex3f(loc_x / 2, -loc_y / 2, loc_z / 2);
	glEnd();

	glPopMatrix();
}
void mas3ad::upmas3ad() {
	if (keys[VK_UP] && yy<90)
		yy += 1;
	if (keys[VK_DOWN] && yy>0)
		yy -= 1;
	glTranslatef(0, yy, 0);
}
void draw_square() {
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glVertex2d(0, 0.5);
	glVertex2d(-0.5, 0.5);
	glVertex2d(-0.5, 0);
	glVertex2d(0, 0);
	glEnd();
}

class character {
public:
	character() {
		draw_head();
		draw_body();
		//draw_right_arm();
		//draw_left_arm();
		//draw_left_leg();
		//draw_right_leg();
	}
	void draw_head();
	void draw_head_front();
	void draw_head_up();
	void draw_head_bottom();
	void draw_head_left();
	void draw_head_right();
	void draw_head_back();
	void draw_body();
	void draw_body_front();
	void draw_body_left();
	void draw_body_right();
	void draw_body_back();
	void draw_right_arm();
	void draw_left_arm();
	void draw_right_leg();
	void draw_left_leg();
	void walk();
	void not_walk();
};
void character::draw_head() {
	glPushMatrix();
	glTranslated(0, 4, 0);

	draw_head_front();
	draw_head_up();
	draw_head_bottom();
	draw_head_left();
	draw_head_right();
	draw_head_back();

	glPopMatrix();
}

void character::draw_head_front() {
	int sum = 1;
	for (float y = 1.5; y >= -2.0; y -= 0.5) {

		for (float x = -1.5; x <= 2.0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if (sum == 51 || sum == 54 || (sum >= 59 && sum <= 62) || (sum >= 1 && sum <= 17) || sum == 24) {
				glColor3ub(89, 45, 0);
			}
			else if (sum == 34 || sum == 39) {
				glColor3ub(255, 255, 255);
			}
			else if (sum == 35 || sum == 38) {
				glColor3ub(0, 0, 255);
			}
			else if (sum == 44 || sum == 45) {
				glColor3ub(181, 90, 0);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			sum += 1;
		}
	}
}

void character::draw_head_up() {
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glTranslated(0, -1.5, -1.5);
	for (float y = 1.5; y >= -2.0; y -= 0.5) {

		for (float x = -1.5; x <= 2.0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(89, 45, 0);
			auxSolidCube(0.5);
			glPopMatrix();
		}
	}
	glPopMatrix();
}

void character::draw_head_bottom() {
	glPushMatrix();
	glTranslated(0, -2, -2);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= -1.5; y -= 0.5) {

		for (float x = -1.5; x <= 2.0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			auxSolidCube(0.5);
			glPopMatrix();
		}
	}

	glPopMatrix();
}

void character::draw_head_left() {
	int cnt = 1;
	glPushMatrix();
	glTranslated(2, -0.5, -2);
	glRotated(90, 0, 1, 0);
	for (float y = 1.5; y >= -1.5; y -= 0.5) {

		for (float x = -1.5; x <= 1.5; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if (cnt <= 21 || (cnt >= 24 && cnt <= 28) || (cnt >= 31 && cnt <= 35) || (cnt >= 40 && cnt <= 42)) {
				glColor3ub(89, 45, 0);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
	glPushMatrix();
	glTranslated(2.25, -0.25, -0.25);
	glRotated(90, 0, 1, 0);
	glColor3ub(89, 45, 0);
	draw_square();
	glPopMatrix();

	glPushMatrix();
	glTranslated(2.25, -0.75, -0.25);
	glRotated(90, 0, 1, 0);
	glColor3ub(89, 45, 0);
	draw_square();
	glPopMatrix();
}

void character::draw_head_right() {
	glPushMatrix();
	glTranslated(-3.5, 0, 0);
	int cnt = 1;
	glPushMatrix();
	glTranslated(2, -0.5, -2);
	glRotated(90, 0, 1, 0);
	for (float y = 1.5; y >= -1.5; y -= 0.5) {

		for (float x = -1.5; x <= 1.5; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if (cnt <= 21 || (cnt >= 24 && cnt <= 28) || (cnt >= 31 && cnt <= 35) || (cnt >= 40 && cnt <= 42)) {
				glColor3ub(89, 45, 0);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
	glPushMatrix();
	glTranslated(1.75, -0.25, -0.25);
	glRotated(90, 0, 1, 0);
	glColor3ub(89, 45, 0);
	draw_square();
	glPopMatrix();

	glPushMatrix();
	glTranslated(1.75, -0.75, -0.25);
	glRotated(90, 0, 1, 0);
	glColor3ub(89, 45, 0);
	draw_square();
	glPopMatrix();
	glPopMatrix();
}

void character::draw_head_back() {
	int cnt = 1;
	glPushMatrix();
	glTranslated(0, 0, -4);
	for (float y = 1.5; y >= -2.0; y -= 0.5) {

		for (float x = -1.5; x <= 2.0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(89, 45, 0);
			if (cnt == 57 || cnt == 58 || cnt == 63 || cnt == 64) {
				glColor3ub(255, 224, 166);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
}

void character::draw_body() {
	glPushMatrix();
	glTranslated(0, 0, -1);

	draw_body_front();
	draw_body_left();
	draw_body_right();
	draw_body_back();

	glPopMatrix();
}

void character::draw_body_front() {
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 2.0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(153, 217, 234);
			if ((cnt >= 3 && cnt <= 6) || cnt == 12 || cnt == 13) {
				glColor3ub(255, 224, 166);
			}
			else if ((cnt >= 81 && cnt <= 86) || (cnt >= 89 && cnt <= 95)) {
				glColor3ub(63, 72, 204);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
}

void character::draw_body_left() {
	glPushMatrix();
	glTranslated(-1.5, 0, -2);
	glRotated(90, 0, 1, 0);
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(153, 217, 234);
			if ((cnt >= 41 && cnt <= 48)) {
				glColor3ub(63, 72, 204);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
}

void character::draw_body_right() {
	glPushMatrix();
	glTranslated(2, 0, -2);
	glRotated(90, 0, 1, 0);
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(153, 217, 234);
			if ((cnt >= 41 && cnt <= 48)) {
				glColor3ub(63, 72, 204);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
}

void character::draw_body_back() {
	glPushMatrix();
	glTranslated(0, 0, -2);
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 2.0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(153, 217, 234);
			if ((cnt >= 81 && cnt <= 83) || (cnt >= 89 && cnt <= 92) || (cnt >= 94 && cnt <= 96) || (cnt >= 87 && cnt <= 88)) {
				glColor3ub(63, 72, 204);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
}

void character::draw_left_arm() {
	//front
	glPushMatrix();
	glTranslated(4, 0, -1);
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//right
	glPushMatrix();
	glTranslated(4, 0, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//back
	glPushMatrix();
	glTranslated(4, 0, -3);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//left
	glPushMatrix();
	glTranslated(2.5, 0, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//up
	glPushMatrix();
	glTranslated(4, 1.5, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(153, 217, 234);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//down
	glPushMatrix();
	glTranslated(4, -4, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
}

void character::draw_right_arm() {
	glPushMatrix();
	glTranslated(-6, 0, 0);

	glPushMatrix();
	glTranslated(4, 0, -1);
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	glPushMatrix();
	glTranslated(4, 0, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	glPushMatrix();
	glTranslated(4, 0, -3);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	glPushMatrix();
	glTranslated(2.5, 0, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			if ((cnt >= 1 && cnt <= 16)) {
				glColor3ub(153, 217, 234);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	glPushMatrix();
	glTranslated(4, 1.5, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(153, 217, 234);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	glPushMatrix();
	glTranslated(4, -4, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(255, 224, 166);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	glPopMatrix();
}

void character::draw_left_leg() {
	//front
	glPushMatrix();
	glTranslated(2, -6, -1);
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//right
	glPushMatrix();
	glTranslated(2, -6, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//back
	glPushMatrix();
	glTranslated(2, -6, -3);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//left
	glPushMatrix();
	glTranslated(0.5, -6, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//up
	glPushMatrix();
	glTranslated(2, -4.5, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(63, 72, 204);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//down
	glPushMatrix();
	glTranslated(2, -10, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(127, 127, 127);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();
}

void character::draw_right_leg() {
	glPushMatrix();
	glTranslated(-2, 0, 0);

	//front
	glPushMatrix();
	glTranslated(2, -6, -1);
	int cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//right
	glPushMatrix();
	glTranslated(2, -6, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//back
	glPushMatrix();
	glTranslated(2, -6, -3);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//left
	glPushMatrix();
	glTranslated(0.5, -6, -3);
	glRotated(90, 0, 1, 0);
	cnt = 1;
	for (float y = 1.5; y >= -4.0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			if ((cnt >= 1 && cnt <= 40)) {
				glColor3ub(63, 72, 204);
			}
			else {
				glColor3ub(127, 127, 127);
			}
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//up
	glPushMatrix();
	glTranslated(2, -4.5, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(63, 72, 204);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	//down
	glPushMatrix();
	glTranslated(2, -10, -2.5);
	glRotated(90, 1, 0, 0);
	for (float y = 1.5; y >= 0; y -= 0.5) {

		for (float x = -1.5; x <= 0; x += 0.5) {

			glPushMatrix();
			glTranslated((float)x, (float)y, 0);
			glColor3ub(127, 127, 127);
			auxSolidCube(0.5);
			glPopMatrix();
			cnt++;
		}
	}
	glPopMatrix();

	glPopMatrix();
}
float pi = (float)22 / 7;
float an = pi, an2 = 2 * pi;
void character::walk() {
	glPushMatrix();

	glRotated(10 * sin(an), 1, 0, 0);
	draw_left_arm();
	draw_right_leg();
	glPopMatrix();
	glPushMatrix();
	glRotated(10 * sin(an2), 1, 0, 0);
	draw_right_arm();
	draw_left_leg();
	an += 2;
	an2 += 2;
	glPopMatrix();
}
void character::not_walk() {
	draw_left_arm();
	draw_right_leg();
	draw_left_leg();
	draw_right_arm();
}
void banner(UINT t,float w,float h) {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_SMOOTH);
	glBindTexture(GL_TEXTURE_2D, t);
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0, 0);//1 0
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);//0 0
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0,h, 0);// 0 1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, 0);// 1 1
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_SMOOTH);
	glPopMatrix();
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Sphere_Ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Sphere_Diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Sphere_Specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, Sphere_Shininess);

	/*gluLookAt(0, 3, 10,
		0, 0, 0,
		0, 1, 0);*/

	gluLookAt(objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z,
		objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,
		objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);



	glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Light_Specular);
	glLightfv(GL_LIGHT0, GL_POSITION, Light_Position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, Light_Spot_direction);
	glLightfv(GL_LIGHT0, GL_AMBIENT, Light_Ambient);



	//clock
	{
		glPushMatrix();
		glTranslated(-470, 35, 0);
		glRotated(90, 0, 1, 0);
		drawTime(0.2);
		glPopMatrix();
	}

	//skybox
	{
		// Always keep the character in the view(SkyBox)
		glPushMatrix();
		glTranslatef(objCamera.mView.x, 0, objCamera.mView.z);
		skybox();
		glPushMatrix();
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glTranslatef(-10, eyey, 0);
		jump();
		glRotated(rtchar, 0, 1, 0);
		glScalef(0.3, 0.3, 0.3);
		if (!(GetKeyState('F')))
		{

			character steve;
			if (keys['W']) {
				steve.walk();
			}
			if (!keys['M']) {
				steve.not_walk();
			}
		}
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glPopMatrix();
		glPopMatrix();
	}

	//garage
	glPushMatrix();
	glTranslated(-11, 0, -100);
	glRotated(180, 0, 1, 0);
	draw_sign(texture[29]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-250, -3, -150);
	/*glPushMatrix();
	glTranslated(-18, -0.6, 0);
	glScaled(7, 7, 7);
	draw_car(0, 1, 0);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-18, -0.6, -9);
	glScaled(7, 7, 7);
	draw_car(0.5, 0.4, 0.6);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-18, -0.6, 9);
	glScaled(7, 7, 7);
	draw_car(1, 0.9, 0.3);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-2, -0.6, -13.5);
	glScaled(7, 7, 7);
	draw_car(0.2, 1, 0.7);
	glPopMatrix();*/
	glRotated(-90, 0, 1, 0);

	glPushMatrix();
	glTranslated(0, 3.5, -125);
	draw_road(18, 90);
	glPopMatrix();

	glScaled(2, 2, 2);
	draw_garage(18, 17, texture[16], texture[18], texture[17], texture[20], texture[21]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-423, 20, 0);

	glPushMatrix();
	glTranslated(225, -19.2, 0);
	glRotated(90, 0, 1, 0);
	draw_floor2(75, 170);
	for (int i = -100; i < 150; i += 20) {
		glPushMatrix();
		glTranslatef(-76, -1, i);
		draw_tree();
		glColor3f(1, 1, 1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(76, -1, i);
		draw_tree();
		glColor3f(1, 1, 1);
		glPopMatrix();

	}
	for (int i = -100; i < 150; i += 30) {
		glPushMatrix();
		glTranslatef(-76, -1, i);
		draw_light();
		glColor3f(1, 1, 1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(76, -1, i);
		draw_light();
		glColor3f(1, 1, 1);
		glPopMatrix();

	}
	glPopMatrix();
	
	glRotated(90, 0, 1, 0);

	glPushMatrix();

	glPushMatrix();
	glTranslatef(-90,22,85);
	banner(texture[27], 15,15);
	glTranslatef(180, 0, 0);
	banner(texture[28], 15,15);
	glTranslatef(-60, 45, -17);
	banner(texture[35], 25,10);
	glTranslatef(-45, 0, 0);
	banner(texture[36], 40, 15);
	glTranslatef(-30, 0, 0);
	banner(texture[35], 25, 10);
	glPopMatrix();

	glScaled(1.5, 1.5, 1.5);
	mall m;
	m.firstfloor(150, 30, 90.0, texture[22], texture[22], texture[24], texture[23], texture[25]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(80, 0, 17);
	glRotated(-90, 0, 1, 0);
	glScaled(1.2, 1.5, 1.2);
	room r1 = room(45.0f, 30.0f, 50.0f,texture[22],texture[18],texture[30]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(80, 0, -38);
	glRotated(-90, 0, 1, 0);
	glScaled(1.2, 1.5, 1.2);
	room r2 = room(45.0f, 30.0f, 50.0f, texture[22], texture[18], texture[31]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-80, 0, 15);
	glRotated(90, 0, 1, 0);
	glScaled(1.2, 1.5, 1.2);
	room r3 = room(45.0f, 30.0f, 50.0f, texture[22], texture[18], texture[32]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-80, 0, -40);
	glRotated(90, 0, 1, 0);
	glScaled(1.2, 1.5, 1.2);
	room r4 = room(45.0f, 30.0f, 50.0f, texture[22], texture[18], texture[33]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-80, 45, 15);
	glRotated(90, 0, 1, 0);
	glScaled(1.2, 1.5, 1.2);
	room r5 = room(45.0f, 30.0f, 50.0f, texture[16], texture[18], texture[34]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-80, 45, -40);
	glRotated(90, 0, 1, 0);
	glScaled(1.2, 1.5, 1.2);
	room r6 = room(45.0f, 30.0f, 50.0f, texture[16], texture[18], texture[30]);
	glPopMatrix();

	glPushMatrix();
	glTranslated(80, 45, -10);
	glRotated(-90, 0, 1, 0);
	glScaled(1.2, 1.5, 1.2);
	room r7 = room(90.0f, 30.0f, 50.0f, texture[16], texture[18], texture[32]);
	glPopMatrix();


	/*glPushMatrix();
	glTranslated(49.8, 0, -22.6);
	glRotated(-90, 0, 1, 0);
	room r2 = room(45.0f, 50.0f, 50.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-49.8, 0, 22.4);
	glRotated(90, 0, 1, 0);
	room r3 = room(45.0f, 50.0f, 50.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-49.8, 0, -22.6);
	glRotated(90, 0, 1, 0);
	room r4 = room(45.0f, 50.0f, 50.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslated(49.8, 50, 22.4);
	glRotated(-90, 0, 1, 0);
	room r5 = room(45.0f, 50.0f, 50.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslated(49.8, 50, -22.6);
	glRotated(-90, 0, 1, 0);
	room r6 = room(45.0f, 50.0f, 50.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-49.8, 50, 0);
	glRotated(90, 0, 1, 0);
	room r7 = room(90.0f, 50.0f, 50.0f);
	glPopMatrix();*/


	glPushMatrix();
	glScaled(1.5, 1.5, 1.5);
	glTranslated(0, 30, 0);
	mall m2;
	m2.scndfloor(150, 30, 90.0, texture[22], texture[22], texture[24], texture[23], texture[25]);
	glPopMatrix();
	//glTranslated(0, 0, -50);
	//mas3ad kl = mas3ad(30, 45, 15);
	glPopMatrix();

	//buildings
	{
		glPushMatrix();
		glTranslatef(-900, -3, -550);
		mall2(10, 50, texture[10]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-950, -3, 1000);
		glRotated(30, 0, 1, 0);
		mall2(15, 100, texture[13]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-185, -3, -350);
		mall2(15, 70, texture[11]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-85, -3, -800);
		mall2(15, 70, texture[13]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-155, -3, -250);
		mall2(10, 85, texture[11]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-105, -3, 300);
		mall2(10, 120, texture[11]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-500, -3, 700);
		glRotated(-30, 0, 1, 0);
		mall2(20, 40, texture[12]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-50, -3, 550);
		glRotated(-30, 0, 1, 0);
		mall2(20, 40, texture[12]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-550, -3, -700);
		glRotated(-30, 0, 1, 0);
		mall2(20, 40, texture[12]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-685, -3, 0);
		mall2(15, 70, texture[11]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-250, -3, 700);
		glRotated(30, 0, 1, 0);
		mall2(15, 100, texture[13]);
		glPopMatrix();
		
		glPopMatrix();
	}


	//road
	glPushMatrix();
	draw_road(10,1000);
	glPopMatrix();
	
	//grass
	glPushMatrix();
	draw_grass();
	glPopMatrix();

	//floor
	glPushMatrix();
	draw_floor();
	glPopMatrix();
	
	//Street Lights
	glPushMatrix();
	for (int i = -300; i < 300; i += 13) {
		glPushMatrix();
		glTranslatef(11, -3, 0);
		glTranslatef(0, 0, i);
		draw_light();
		glPopMatrix();

	}
	glPopMatrix();

	//lights
	glPushMatrix();
	glTranslatef(-11, -3, -20);
	draw_light();
	glTranslatef(0, 0, 40);
	draw_light();
	glPopMatrix();

	//traffic
	{
		glPushMatrix();
		glTranslatef(-11, -3, -30);
		draw_traffic_signal();
		glPopMatrix();

		//traffic signal 2
		glPushMatrix();
		glTranslatef(11, -3, 27);
		glRotatef(180, 0, 1, 0);
		draw_traffic_signal();
		glPopMatrix();
	}

	//trees
	glPushMatrix();
	for (int i = -300; i < 300; i += 20) {
		glPushMatrix();
		glTranslatef(15, -1, i);
		draw_tree();
		glColor3f(1, 1, 1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(35, -1, i + 10);
		draw_tree();
		glColor3f(1, 1, 1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(55, -1, i + 15);
		draw_tree();
		glColor3f(1, 1, 1);
		glPopMatrix();

	}
	glPopMatrix();

	//cars
	{
		glPushMatrix();
		//red cars
		for (int i = -50; i < 60; i += 10) {
			glPushMatrix();
			glTranslatef(0, 0, carstr);
			glTranslatef(-7, -3.6, 0);
			glScaled(7, 7, 7);
			glRotated(90, 0, 1, 0);
			glTranslatef(i, 0, 0);
			draw_car(1, 0.1, 0.2);
			glPopMatrix();
		}
		//yellow cars
		for (int i = -45; i < 55; i += 10) {
			glPushMatrix();
			glTranslatef(0, 0, carstr);
			glTranslatef(-7, -3.6, 0);
			glScaled(7, 7, 7);
			glRotated(90, 0, 1, 0);
			glTranslatef(i, 0, 0);
			draw_car(1, 1, 0);
			glPopMatrix();
		}
		//opposite side red cars
		for (int i = -50; i < 60; i += 10) {
			glPushMatrix();
			glTranslatef(0, 0, carstr2);
			glTranslatef(7, -3.6, 0);
			glScaled(7, 7, 7);
			glRotated(-90, 0, 1, 0);
			glTranslatef(i, 0, 0);
			draw_car(1, 0.1, 0.2);
			glPopMatrix();
		}
		//opposite side yellow cars
		for (int i = -45; i < 55; i += 10) {
			glPushMatrix();
			glTranslatef(0, 0, carstr2);
			glTranslatef(7, -3.6, 0);
			glScaled(7, 7, 7);
			glRotated(-90, 0, 1, 0);
			glTranslatef(i, 0, 0);
			draw_car(1, 1, 0);
			glPopMatrix();
		}
		glColor3f(1, 1, 1);
		glPopMatrix();
	}

	carstr += 0.7;
	carstr2 -= 1;
	if (carstr2 < -200) {
		carstr2 += 200;
	}
	if (carstr > 200) {
		carstr -= 200;
	}


	glFlush();											// Done Drawing The Quad	
														//DO NOT REMOVE THIS
	SwapBuffers(hDC);
	return TRUE;										// Keep Going
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, _T("Release Of DC And RC Failed."), _T("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, _T("Release Rendering Context Failed."), _T("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, _T("Release Device Context Failed."), _T("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, _T("Could Not Release hWnd."), _T("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass(_T("OpenGL"), hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL, _T("Could Not Unregister Class."), _T("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
*	title			- Title To Appear At The Top Of The Window				*
*	width			- Width Of The GL Window Or Fullscreen Mode				*
*	height			- Height Of The GL Window Or Fullscreen Mode			*
*	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
*	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;			// Set Left Value To 0
	WindowRect.right = (long)width;		// Set Right Value To Requested Width
	WindowRect.top = (long)0;				// Set Top Value To 0
	WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

	fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
	wc.cbClsExtra = 0;									// No Extra Window Data
	wc.cbWndExtra = 0;									// No Extra Window Data
	wc.hInstance = hInstance;							// Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground = NULL;									// No Background Required For GL
	wc.lpszMenuName = NULL;									// We Don't Want A Menu
	wc.lpszClassName = _T("OpenGL");								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL, _T("Failed To Register The Window Class."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}

	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL, _T("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?"), _T("NeHe GL"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, _T("Program Will Now Close."), _T("ERROR"), MB_OK | MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

																	// Create The Window
	if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
		_T("OpenGL"),							// Class Name
		_T("title"),								// Window Title
		dwStyle |							// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		WindowRect.right - WindowRect.left,	// Calculate Window Width
		WindowRect.bottom - WindowRect.top,	// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, _T("Window Creation Error."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, _T("Can't Create A GL Device Context."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, _T("Can't Find A Suitable PixelFormat."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, _T("Can't Set The PixelFormat."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, _T("Can't Create A GL Rendering Context."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, _T("Can't Activate The GL Rendering Context."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL, _T("Initialization Failed."), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
	UINT	uMsg,			// Message For This Window
	WPARAM	wParam,			// Additional Message Information
	LPARAM	lParam)			// Additional Message Information
{
	static PAINTSTRUCT ps;

	switch (uMsg)									// Check For Windows Messages
	{
	case WM_PAINT:
		DrawGLScene();
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_TIMER:
		DrawGLScene();
		return 0;

	case WM_ACTIVATE:							// Watch For Window Activate Message
	{
		if (!HIWORD(wParam))					// Check Minimization State
		{
			active = TRUE;						// Program Is Active
		}
		else
		{
			active = FALSE;						// Program Is No Longer Active
		}

		return 0;								// Return To The Message Loop
	}

	case WM_SYSCOMMAND:							// Intercept System Commands
	{
		switch (wParam)							// Check System Calls
		{
		case SC_SCREENSAVE:					// Screensaver Trying To Start?
		case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
			return 0;							// Prevent From Happening
		}
		break;									// Exit
	}

	case WM_CLOSE:								// Did We Receive A Close Message?
	{
		PostQuitMessage(0);						// Send A Quit Message
		return 0;								// Jump Back
	}

	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;					// If So, Mark It As TRUE
		return 0;								// Jump Back
	}

	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = FALSE;					// If So, Mark It As FALSE
		return 0;								// Jump Back
	}

	case WM_SIZE:								// Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}

	case WM_MOUSEMOVE:
	{
		mouseX = (int)LOWORD(lParam);   mouseY = (int)HIWORD(lParam);
		isClicked = (LOWORD(wParam) & MK_LBUTTON) ? true : false;
		isRClicked = (LOWORD(wParam) & MK_RBUTTON) ? true : false;
		break;
	}
	case WM_LBUTTONUP:
		isClicked = false; 	 break;
	case WM_RBUTTONUP:
		isRClicked = false;   break;
	case WM_LBUTTONDOWN:
		isClicked = true; 	break;
	case WM_RBUTTONDOWN:
		isRClicked = true;	break;

	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE	hInstance,			// Instance
	HINSTANCE	hPrevInstance,		// Previous Instance
	LPSTR		lpCmdLine,			// Command Line Parameters
	int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done = FALSE;								// Bool Variable To Exit Loop

														// Ask The User Which Screen Mode They Prefer
														//if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen = false;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("Madhat NeHe Template", 640, 480, 16, fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}


	//Set drawing timer to 20 frame per second
	UINT timer = SetTimer(hWnd, 0, 1, (TIMERPROC)NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		Keyboard_Input();
		objCamera.Mouse_Move(640, 480);


	}

	return 0;

}