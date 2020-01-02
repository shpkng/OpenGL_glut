#include <GL/glew.h>
#include <GL/freeglut.h>
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma comment(lib, "glew32.lib")

using namespace std;
using namespace glm;

#pragma region CONSTANT_VALUES
const int WIDTH = 800, HEIGHT = 800;
const char* WINDOW_NAME = "古力娜扎天下第一好看可郑秀晶就是天";
//const char* filename = "krystal.bmp";
const char* filename = "container.jpg";
const char* vertex = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec2 aTexCoord;\n"
"uniform mat4 transform;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"	gl_Position = transform * vec4(aPos, 1.0);\n"
"	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
"}";

const char* constantFragmentShaderString = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D tex;\n"
"void main()\n"
"{\n"
"	FragColor = texture(tex, TexCoord);\n"
"	//FragColor = vec4(1,0,1,1);\n"
"}";

//
//float vertices[] =
//{
//	-0.8f,-0.8f,0.0f,
//	0.8f,-0.8f,0.0f,
//	0.0f,0.8f,0.0f,
//};
//
//float indices[] = {
//	1,2,3 ,
//};
//
//float colors[] =
//{
//	1.0f,0.0f,0.0f,
//	0.0f,1.0f,0.0f,
//	0.0f,0.0f,1.0f,
//};

float vertices[] = {
	0.5f, 0.5f, 0.5f,  1.0f, 1.0f,   // 右上角
	0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // 右下角
	-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,// 左下角
	-0.5f, 0.5f, 0.0f ,  0.0f, 1.0f // 左上角
};

unsigned int indices[] = { // 注意索引从0开始! 
	0, 1, 3, // 第一个三角形
	1, 2, 3  // 第二个三角形
};

unsigned int VBO, VAO, EBO;
GLuint program;
#pragma endregion

#pragma region INPUT_VARS
bool MOUSE_LEFT_BUTTON_DOWN, MOUSE_RIGHT_BUTTON_DOWN, MOUSE_MID_BUTTON_DOWN;
#pragma endregion

#pragma region PREDEFINEDS
GLuint tex;
time_t lastFrameTime;

bool showLinesOnly = true;
GLuint vertShader, fragShader;
float deltaX, deltaY, deltaZ, speedRot = 20, scale = 1;
#pragma endregion

int main(int argc, char** argv);

void onMouseButton(int button, int state, int x, int y);
void onMouseWheel(int wheel, int direction, int x, int y);
void onKey(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onRegisterCallbacks();
void onDisplay();
void onReshapeWindow(int w, int h);
void drawCube(double size, double x, double y, double z);
GLuint setShader();
void Move(int axe, bool add);

//const char* loadShaderAsString(const char* file);



int main(int argc, char** argv) {

	cout << "Trying to start GeoDrawer..." << endl;


	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow(WINDOW_NAME);
	glPolygonMode(GL_BACK, GL_LINE);
	glutReshapeFunc(onReshapeWindow);
	glutDisplayFunc(onDisplay);

	glClearColor(1, 1, 1, 1);

	GLenum error = glewInit();
	if (error) {
		cout << "Failed to init.";
		return error;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	int req_channels = 3;
	int width, height, channels;
	auto image = stbi_load(filename, &width, &height, &channels, 3);
	if (image != nullptr) {
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image);
	}
	else {
		glutExit();
	}

	program = setShader();
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	onRegisterCallbacks();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUseProgram(program);
	glBindVertexArray(VAO);

	// Udpate循环
	glutMainLoop();

	return 0;
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
	case 'w':Move(3, true); break;
	case 's':Move(3, false); break;
	case 'a':Move(1, false); break;
	case 'd':Move(1, true); break;
	case 'q':Move(2, false); break;
	case 'e':Move(2, true); break;
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

	mat4 trans = mat4(1.0f);
	trans = translate(trans, vec3(deltaX, deltaY, deltaZ));
	trans = rotate(trans, radians((float)glutGet(GLUT_ELAPSED_TIME) / 20), vec3(0.0, 1.0, 1.0));

	auto tloc = glGetUniformLocation(program, "transform");
	glUniformMatrix4fv(tloc, 1, GL_FALSE, value_ptr(trans));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
	glVertex3d(x - hs, y - hs, z + hs); // bottom left
	glTexCoord2f(0.0, 1.0);
	glVertex3d(x - hs, y + hs, z + hs); // top left
	glTexCoord2f(1, 1.0);
	glVertex3d(x + hs, y + hs, z + hs); // top right
	glTexCoord2f(0.0, 0);
	glVertex3d(x + hs, y - hs, z + hs); // bottom right
	glTexCoord2f(1, 1.0);


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
	glTexCoord2f(0.0, 0);
	glVertex3d(x - hs, y + hs, z + hs); // top left
	glTexCoord2f(0.0, 1);
	glVertex3d(x - hs, y + hs, z - hs); // top right
	glTexCoord2f(1, 1);
	glVertex3d(x - hs, y - hs, z - hs); // bottom right
	glTexCoord2f(1, 0);
	glVertex3d(x - hs, y - hs, z + hs); // bottom left
// right face
	glTexCoord2f(0.0, 0);
	glVertex3d(x + hs, y + hs, z + hs); // top left
	glTexCoord2f(0.0, 1);
	glVertex3d(x + hs, y + hs, z - hs); // top right
	glTexCoord2f(1, 1);
	glVertex3d(x + hs, y - hs, z - hs); // bottom right
	glTexCoord2f(1, 0);
	glVertex3d(x + hs, y - hs, z + hs); // bottom left

	// top face				
	glTexCoord2f(0.0, 0);
	glVertex3d(x - hs, y + hs, z + hs); // top left
	glTexCoord2f(0.0, 1);
	glVertex3d(x - hs, y + hs, z - hs); // top right
	glTexCoord2f(1, 1);
	glVertex3d(x + hs, y + hs, z - hs); // bottom right
	glTexCoord2f(1, 0);
	glVertex3d(x + hs, y + hs, z + hs); // bottom left
	// top face				
	glTexCoord2f(0.0, 0);
	glVertex3d(x - hs, y - hs, z + hs); // top left
	glTexCoord2f(0.0, 1);
	glVertex3d(x - hs, y - hs, z - hs); // top right
	glTexCoord2f(1, 1);
	glVertex3d(x + hs, y - hs, z - hs); // bottom right
	glTexCoord2f(1, 0);
	glVertex3d(x + hs, y - hs, z + hs); // bottom left

	glEnd();
}

GLuint setShader() {

	char* res = (char*)malloc(2048 * sizeof(char));
	int len;

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//glShaderSource(vertShader, 1, VERTEX_SHADER, NULL);
	//auto vert = loadShaderAsString("vertex.shader");
	const GLchar* vertCode[] = { vertex };
	glShaderSource(vertShader, 1, vertCode, NULL);
	glCompileShader(vertShader);
	glGetShaderInfoLog(vertShader, 2048, &len, res);
	cout << res;

	//auto frag = loadShaderAsString("frag.shader");
	const GLchar* fragCode[] = { constantFragmentShaderString };

	glShaderSource(fragShader, 1, fragCode, NULL);
	glCompileShader(fragShader);
	glGetShaderInfoLog(fragShader, 2048, &len, res);
	cout << res;

	auto program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);
	//glUseProgram(program);

	glGetProgramInfoLog(program, 2048, &len, res);
	cout << res;


	return program;
}

void Move(int axe, bool add) {
	switch (axe)
	{
	case 0: {
		deltaX = max(min(1.0, deltaX + add ? 0.1 : -0.1), -1.0); break;
	}case 1: {
		deltaY = clamp(deltaY + add ? 0.1 : -1, -10.0, 1.0); break;
	}case 2: {
		deltaZ = clamp(deltaZ + add ? 0.1 : -1, -10.0, 1.0); break;
	}
	default:
		break;
	}

	cout << deltaX << " " << deltaY << " " << deltaZ << endl;
}