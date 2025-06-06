//	http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <vector>

using namespace std;

int		window_width = 600;
int		window_height = 600;
char	window_title[] = "2D transformations";

float vertices[] = {
	// positions           // color
	 0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f, // top right
	 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f, // bottom left
	-0.5f,  0.5f, 0.0f,    0.0f, 1.0f, 0.0f, // top left 
};


std::vector<glm::vec3> polygon;
std::vector<glm::vec3> controlPolygon;
std::vector<GLfloat> alphaContainer;

/* Vertex buffer objektum �s vertex array objektum az adatt�rol�shoz.*/
#define numVBOs 1
#define numVAOs 1
GLuint VBO[numVBOs];
GLuint VAO[numVAOs];

GLuint			renderingProgram;
unsigned int	transformLoc;

// p�lda: eltol�s, forgat�s �s sk�l�z�s.
//transformation = glm::rotate(transformation, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//transformation = glm::translate(transformation, glm::vec3(-0.5, 0.5, 0.0));
//transformation = glm::rotate(transformation, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
//transformation = glm::scale(transformation, glm::vec3(0.5, 0.5, 0.5));

glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5, 0.5, 0.0));
glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
glm::mat4 transformation(1.0f);


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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void createPolygon() {
	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, polygon.size() * sizeof(glm::vec3), polygon.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
}

void generatePolygonPoints(GLint numberOfPoints, GLfloat r, std::vector<glm::vec3>& container, std::vector<GLfloat>& alphaContainer, bool alphaNeeded) {
	GLfloat x, y;
	GLfloat alpha = 0.0f;
	GLfloat full_circle = 2.0f * M_PI;

	for (int i = 0; i < numberOfPoints + 1; ++i) {
		float alpha = (full_circle * i) / numberOfPoints;
		x = 0.0 + r * cos(alpha);
		y = 0.0 + r * sin(alpha);

		container.push_back({ x, y, 0.0f });
		if (alphaNeeded) {
			alphaContainer.push_back(alpha);
		}
	}
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();
	generatePolygonPoints(4, 0.2, polygon, alphaContainer, false);
	generatePolygonPoints(12, 1.0, controlPolygon, alphaContainer, true);
	createPolygon();
	glClearColor(0.0, 0.0, 0.0, 1.0);

	transformLoc = glGetUniformLocation(renderingProgram, "transform");
	// aktiv�ljuk a shader-program objektumunkat.
	glUseProgram(renderingProgram);
}

/** A jelenet�nk ut�ni takar�t�s. */
void cleanUpScene()
{
	/** T�r�lj�k a vertex puffer �s vertex array objektumokat. */
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);

	/** T�r�lj�k a shader programot. */
	glDeleteProgram(renderingProgram);
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT); // fontos lehet minden egyes alkalommal t�r�lni!

	/*Csatoljuk a vertex array objektumunkat. */
	glBindVertexArray(VAO[0]);

	float newAngle = 0.0f;

	for (int i = 0; i < controlPolygon.size(); ++i) {
		glm::vec3 controlLine = controlPolygon.at(i);
		GLfloat alpha = alphaContainer.at(i);
		glm::mat4 model(1.0f);
		translateMatrix = glm::translate(model, controlLine);
		//rotateMatrix = glm::rotate(model, alpha, glm::vec3((0.0f, 0.0f, 1.0f)));
		//scaleMatrix = glm::scale(model, { 1.0f, 1.0f, 1.0f });
		transformation = translateMatrix * rotateMatrix * scaleMatrix;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translateMatrix));

		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, polygon.size());
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}

int main(void) {

	/* Pr�b�ljuk meg inicializ�lni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/* A k�v�nt OpenGL verzi� (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/* Pr�b�ljuk meg l�trehozni az ablakunkat. */
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);

	/* V�lasszuk ki az ablakunk OpenGL kontextus�t, hogy haszn�lhassuk. */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetKeyCallback(window, keyCallback);
	//glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

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

	cleanUpScene();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}