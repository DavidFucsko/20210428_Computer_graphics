#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

float x = 0.0f;
float y = 0.0f;
float inc = 0.01f;

bool xDir = true;
bool yDir = true;

bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() {
	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string vertShaderStr = readShaderSource("vertexShader.glsl");
	string fragShaderStr = readShaderSource("fragmentShader.glsl");

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}


	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	// Shader program objektum létrehozása. Eltároljuk az ID értéket.
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		xDir = false;
		yDir = true;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		xDir = true;
		yDir = false;
	}
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
}

void display(GLFWwindow* window, double currentTime) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT); // fontos lehet minden egyes alkalommal törölni!

	// aktiváljuk a shader-program objektumunkat.
	glUseProgram(renderingProgram);

	//if (xDir) {
	//	x += inc; // mozgassuk a háromszöget az x-tengely mentén.
	//	if (x > 0.90f) inc = -0.01f; // váltsunk, balra mozogjon a háromszög.
	//	if (x < -0.90f) inc = 0.01f; // váltsunk, jobbra mozogjon a háromszög.
	//	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offsetX"); // mutató az "offsetX" értékre
	//	glProgramUniform1f(renderingProgram, offsetLoc, x); // küldjük el az "x" értékét az "offsetX" számára
	//}
	//else if (yDir) {
	//	y += inc; // mozgassuk a háromszöget az y-tengely mentén.
	//	if (y > 1.0f) inc = -0.01f; // váltsunk, lefele mozogjon a háromszög.
	//	if (y < -1.0f) inc = 0.01f; // váltsunk, felfele mozogjon a háromszög.
	//	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offsetY"); // mutató az "offsetY" értékre
	//	glProgramUniform1f(renderingProgram, offsetLoc, y); // küldjük el az "y" értékét az "offsetY" számára
	//}

	x += inc; // mozgassuk a háromszöget az x-tengely mentén.
	if (x > 0.90f) inc = -0.01f; // váltsunk, balra mozogjon a háromszög.
	if (x < -0.90f) inc = 0.01f; // váltsunk, jobbra mozogjon a háromszög.
	GLuint offsetLocX = glGetUniformLocation(renderingProgram, "offsetX"); // mutató az "offsetX" értékre
	glProgramUniform1f(renderingProgram, offsetLocX, x); // küldjük el az "x" értékét az "offsetX" számára

	y += inc; // mozgassuk a háromszöget az y-tengely mentén.
	if (y > 1.0f) inc = -0.01f; // váltsunk, lefele mozogjon a háromszög.
	if (y < -1.0f) inc = 0.01f; // váltsunk, felfele mozogjon a háromszög.
	GLuint offsetLocY = glGetUniformLocation(renderingProgram, "offsetY"); // mutató az "offsetY" értékre
	glProgramUniform1f(renderingProgram, offsetLocY, y); // küldjük el az "y" értékét az "offsetY" számára

	glBegin(GL_TRIANGLES);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f( 0.5f, -0.5f);
	glVertex2f( 0.5f,  0.5f);

	glVertex2f( 0.5f,  0.5f);
	glVertex2f(-0.5f,  0.5f);
	glVertex2f(-0.5f, -0.5f);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main(void) {
	/* Próbáljuk meg inicializálni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/* A kívánt OpenGL verzió (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/* Próbáljuk meg létrehozni az ablakunkat. */
	GLFWwindow* window = glfwCreateWindow(600, 600, "Moving Square", NULL, NULL);

	/* Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	//glfwSetCursorPosCallback(window, cursorPosCallback);
	//glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/* Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	/* Az alkalmazáshoz kapcsolódó elõkészítõ lépések, pl. hozd létre a shader objektumokat. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/* a kód, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display(window, glfwGetTime());
		/* double buffered */
		glfwSwapBuffers(window);
		/* események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás */
		glfwPollEvents();
	}

	/* töröljük a GLFW ablakot. */
	glfwDestroyWindow(window);
	/* Leállítjuk a GLFW-t */
	glfwTerminate();
	exit(EXIT_SUCCESS);
}