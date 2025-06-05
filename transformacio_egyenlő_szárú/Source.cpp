//	http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
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


std::vector<glm::vec3> triangle = {
	{0.0f,  0.5f, 0.0f},
	{-0.5f, -0.5f, 0.0f},
	{0.5f, -0.5f, 0.0f}
};

/* Vertex buffer objektum és vertex array objektum az adattároláshoz.*/
#define numVBOs 1
#define numVAOs 1
GLuint VBO[numVBOs];
GLuint VAO[numVAOs];

GLuint			renderingProgram;
unsigned int	transformLoc;

// példa: eltolás, forgatás és skálázás.
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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void crateTriangle() {
	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(glm::vec3), triangle.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();
	crateTriangle();
	glClearColor(0.0, 0.0, 0.0, 1.0);

	transformLoc = glGetUniformLocation(renderingProgram, "transform");
	// aktiváljuk a shader-program objektumunkat.
	glUseProgram(renderingProgram);
}

/** A jelenetünk utáni takarítás. */
void cleanUpScene()
{
	/** Töröljük a vertex puffer és vertex array objektumokat. */
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);

	/** Töröljük a shader programot. */
	glDeleteProgram(renderingProgram);
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT); // fontos lehet minden egyes alkalommal törölni!

	/*Csatoljuk a vertex array objektumunkat. */
	glBindVertexArray(VAO[0]);

	for (int i = 0; i < 26; ++i) {
		float scale = 0.05f + i * 0.02f;
		float angle = glm::degrees(i * 15.0f);

		glm::mat4 model = glm::mat4(1.0f);
		translateMatrix = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		scaleMatrix = glm::scale(model, glm::vec3(scale, scale, 1.0f));
		rotateMatrix = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		transformation = translateMatrix * rotateMatrix * scaleMatrix;
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation));
		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	}
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}

int main(void) {

	/* Próbáljuk meg inicializálni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/* A kívánt OpenGL verzió (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/* Próbáljuk meg létrehozni az ablakunkat. */
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);

	/* Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwSetKeyCallback(window, keyCallback);
	//glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

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

	cleanUpScene();

	glfwTerminate();
	exit(EXIT_SUCCESS);
}