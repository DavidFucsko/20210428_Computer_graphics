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

	// Shader program objektum l�trehoz�sa. Elt�roljuk az ID �rt�ket.
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
	glClear(GL_COLOR_BUFFER_BIT); // fontos lehet minden egyes alkalommal t�r�lni!

	// aktiv�ljuk a shader-program objektumunkat.
	glUseProgram(renderingProgram);

	//if (xDir) {
	//	x += inc; // mozgassuk a h�romsz�get az x-tengely ment�n.
	//	if (x > 0.90f) inc = -0.01f; // v�ltsunk, balra mozogjon a h�romsz�g.
	//	if (x < -0.90f) inc = 0.01f; // v�ltsunk, jobbra mozogjon a h�romsz�g.
	//	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offsetX"); // mutat� az "offsetX" �rt�kre
	//	glProgramUniform1f(renderingProgram, offsetLoc, x); // k�ldj�k el az "x" �rt�k�t az "offsetX" sz�m�ra
	//}
	//else if (yDir) {
	//	y += inc; // mozgassuk a h�romsz�get az y-tengely ment�n.
	//	if (y > 1.0f) inc = -0.01f; // v�ltsunk, lefele mozogjon a h�romsz�g.
	//	if (y < -1.0f) inc = 0.01f; // v�ltsunk, felfele mozogjon a h�romsz�g.
	//	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offsetY"); // mutat� az "offsetY" �rt�kre
	//	glProgramUniform1f(renderingProgram, offsetLoc, y); // k�ldj�k el az "y" �rt�k�t az "offsetY" sz�m�ra
	//}

	x += inc; // mozgassuk a h�romsz�get az x-tengely ment�n.
	if (x > 0.90f) inc = -0.01f; // v�ltsunk, balra mozogjon a h�romsz�g.
	if (x < -0.90f) inc = 0.01f; // v�ltsunk, jobbra mozogjon a h�romsz�g.
	GLuint offsetLocX = glGetUniformLocation(renderingProgram, "offsetX"); // mutat� az "offsetX" �rt�kre
	glProgramUniform1f(renderingProgram, offsetLocX, x); // k�ldj�k el az "x" �rt�k�t az "offsetX" sz�m�ra

	y += inc; // mozgassuk a h�romsz�get az y-tengely ment�n.
	if (y > 1.0f) inc = -0.01f; // v�ltsunk, lefele mozogjon a h�romsz�g.
	if (y < -1.0f) inc = 0.01f; // v�ltsunk, felfele mozogjon a h�romsz�g.
	GLuint offsetLocY = glGetUniformLocation(renderingProgram, "offsetY"); // mutat� az "offsetY" �rt�kre
	glProgramUniform1f(renderingProgram, offsetLocY, y); // k�ldj�k el az "y" �rt�k�t az "offsetY" sz�m�ra

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
	/* Pr�b�ljuk meg inicializ�lni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/* A k�v�nt OpenGL verzi� (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/* Pr�b�ljuk meg l�trehozni az ablakunkat. */
	GLFWwindow* window = glfwCreateWindow(600, 600, "Moving Square", NULL, NULL);

	/* V�lasszuk ki az ablakunk OpenGL kontextus�t, hogy haszn�lhassuk. */
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	//glfwSetCursorPosCallback(window, cursorPosCallback);
	//glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/* Incializ�ljuk a GLEW-t, hogy el�rhet�v� v�ljanak az OpenGL f�ggv�nyek. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	/* Az alkalmaz�shoz kapcsol�d� el�k�sz�t� l�p�sek, pl. hozd l�tre a shader objektumokat. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/* a k�d, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display(window, glfwGetTime());
		/* double buffered */
		glfwSwapBuffers(window);
		/* esem�nyek kezel�se az ablakunkkal kapcsolatban, pl. gombnyom�s */
		glfwPollEvents();
	}

	/* t�r�lj�k a GLFW ablakot. */
	glfwDestroyWindow(window);
	/* Le�ll�tjuk a GLFW-t */
	glfwTerminate();
	exit(EXIT_SUCCESS);
}