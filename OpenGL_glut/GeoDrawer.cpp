#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <map>
#include "stb_image.h"

#pragma comment(lib, "glew32.lib")

using namespace std;

#pragma region CONSTANT_VALUES
const int WIDTH = 800, HEIGHT = 800;
const char* WINDOW_NAME = "古力娜扎天下第一好看可郑秀晶就是天";
const char* filename = "krystal.bmp";
#pragma endregion

#pragma region INPUT_VARS
bool MOUSE_LEFT_BUTTON_DOWN, MOUSE_RIGHT_BUTTON_DOWN, MOUSE_MID_BUTTON_DOWN;
#pragma endregion

#pragma region PREDEFINEDS
vector<GLuint> textures;
map<const char*, GLuint> texturesMap;
time_t lastFrameTime;

bool showLinesOnly;
#pragma endregion

int main(int argc, char** argv);

void onInit();
void onMouseButton(int button, int state, int x, int y);
void onMouseWheel(int wheel, int direction, int x, int y);
void onKey(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onRegisterCallbacks();
void onDisplay();
void onReshapeWindow(int w, int h);
bool loadTexture(const char* filename);
bool registerCallback(char key, void* callback);
void drawCube(double size, double x, double y, double z);

int main(int argc, char** argv) {

	cout << "Trying to start GeoDrawer..." << endl;


	glutInit(&argc, argv);

	onInit();

	GLenum error = glewInit();
	if (error) {
		cout << "Failed to init.";
		return error;
	}

	onRegisterCallbacks();

	// Udpate循环
	glutMainLoop();

	return 0;
}

void onInit()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow(WINDOW_NAME);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glutReshapeFunc(onReshapeWindow);
	glutDisplayFunc(onDisplay);

	glClearColor(1, 1, 1, 1);
	glColor3f(0, 0, 0);



	int req_channels = 3;
	int width, height, channels;
	auto image = stbi_load(filename, &width, &height, &channels, 3);
	GLuint tex;
	if (image != nullptr) {
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // default

		stbi_image_free(image);
	}
	else {
		glutExit();
	}

}

void onMouseButton(int button, int state, int x, int y)
{
	cout << "onMouseButton " << button << ' ' << state << ' ' << x << ' ' << y << endl;
}

void onMouseWheel(int wheel, int direction, int x, int y)
{
	cout << "onMouseButton " << wheel << ' ' << direction << ' ' << x << ' ' << y << endl;
}

void onKey(unsigned char key, int x, int y)
{
	cout << "onMouseButton " << key << ' ' << x << ' ' << y << endl;
	switch (key)
	{
	case 'q':glColor3d(0, 0, 0); break;
	case 'w':glColor3d(1, 1, 1); break;
	case 't': {

	}break;
	case 'f': {
		if (showLinesOnly) {
			glPolygonMode(GL_BACK, GL_FILL);
		}
		else
		{
			glPolygonMode(GL_BACK, GL_LINE);
		}
		showLinesOnly = !showLinesOnly;
	}break;
	case 27:glutExit(); break;
	default:
		break;
	}
}

void onKeyUp(unsigned char key, int x, int y)
{
	cout << "onMouseButton " << key << ' ' << x << ' ' << y << endl;
}

void onRegisterCallbacks() {
	glutMouseFunc(onMouseButton);
	glutMouseWheelFunc(onMouseWheel);
	glutKeyboardFunc(onKey);
	glutKeyboardUpFunc(onKeyUp);
}

void onReshapeWindow(int w, int h) {
	cout << "Viewport changed to " << w << "*" << h << endl;
	//glutReshapeWindow(w, h);
	glViewport(0, 0, w, h);
}

void onDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	lastFrameTime += glutGet(GLUT_ELAPSED_TIME);
	glRotatef(45, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	glRotatef(glutGet(GLUT_ELAPSED_TIME) / 15, 0, 1, 0);
	drawCube(0.5, .5, 0, .5);

	glutSwapBuffers();
	glutPostRedisplay();
}


void drawCube(double size, double x, double y, double z)
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	auto hs = size / 2;
	auto fZ = z - hs, bZ = z + hs, uY = y + hs, dY = y - hs, lX = x - hs, rX = x + hs;

	// front face
	glTexCoord2f(0.0, 1.0);
	glVertex3d(x - hs, y + hs, z + hs); // top left
	glTexCoord2f(1, 1.0);
	glVertex3d(x + hs, y + hs, z + hs); // top right
	glTexCoord2f(0.0, 0);
	glVertex3d(x + hs, y - hs, z + hs); // bottom right
	glTexCoord2f(1, 1.0);
	glVertex3d(x - hs, y - hs, z + hs); // bottom left

	// back face																		
	glTexCoord2f(0.0, 0);
	glVertex3d(x - hs, y + hs, z - hs); // top left
	glTexCoord2f(0.0, 1);
	glVertex3d(x + hs, y + hs, z - hs); // top right
	glTexCoord2f(1, 1);
	glVertex3d(x + hs, y - hs, z - hs); // bottom right
	glTexCoord2f(1, 0);
	glVertex3d(x - hs, y - hs, z - hs); // bottom left

	// left face																		 
	glVertex3d(x - hs, y + hs, z + hs); // top left
	glVertex3d(x - hs, y + hs, z - hs); // top right
	glVertex3d(x - hs, y - hs, z - hs); // bottom right
	glVertex3d(x - hs, y - hs, z + hs); // bottom left
// right face																		 
	glVertex3d(x + hs, y + hs, z + hs); // top left
	glVertex3d(x + hs, y + hs, z - hs); // top right
	glVertex3d(x + hs, y - hs, z - hs); // bottom right
	glVertex3d(x + hs, y - hs, z + hs); // bottom left

	// top face																			 
	glVertex3d(x - hs, y + hs, z + hs); // top left
	glVertex3d(x - hs, y + hs, z - hs); // top right
	glVertex3d(x + hs, y + hs, z - hs); // bottom right
	glVertex3d(x + hs, y + hs, z + hs); // bottom left
	// top face																			 
	glVertex3d(x - hs, y - hs, z + hs); // top left
	glVertex3d(x - hs, y - hs, z - hs); // top right
	glVertex3d(x + hs, y - hs, z - hs); // bottom right
	glVertex3d(x + hs, y - hs, z + hs); // bottom left

	glEnd();
}