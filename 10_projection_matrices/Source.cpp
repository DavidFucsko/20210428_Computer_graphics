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

extern	glm::mat4 calculateProjection();


int		window_width	= 600;
int		window_height	= 600;
char	window_title[]	= "3D transformation";

/* Vertex buffer objektum �s vertex array objektum az adatt�rol�shoz.*/
#define numVBOs	1
#define numVAOs	1
GLuint			VBO[numVBOs];
GLuint			VAO[numVAOs];

GLuint			renderingProgram;
// 4x4 transformation matrices
// this will make the object 2x bigger among all axes
glm::mat4		scaleM		= glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
// it shifts the object to the left by 3.0 unit
glm::mat4		translateM	= glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.0f, 0.0f));
// it will hold the rotation, now the identity matrix or unit matrix keeps the object in place
glm::mat4		rotateM		= glm::mat4(1.0f);
// it will hold the multiplication of the tranformations
glm::mat4		model		= glm::mat4(1.0f);
// it shifts the object into the viewing cone, for perspective -10.0 shall be sufficient
glm::mat4		view		= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
// it can be perspective or orthographic projection onto the x-y plane
glm::mat4		projection	= calculateProjection();

// they will hold the location of the uniform variables of the vertex shader
unsigned int	modelLoc;
unsigned int	viewLoc;
unsigned int	projectionLoc;

// this array defines a triangular pyramid with colors for the vertices
float vertices[] = {
	// positions           // color
	0.0f, 0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.5f,  -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,

	0.5f,  -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
	0.0f,  -0.5f, -0.5f,	0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
	0.0f,  -0.5f, -0.5f,	0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.5f,  -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
	0.0f,  -0.5f, -0.5f	,	0.0f, 1.0f, 0.0f
};

// you can change the projection here
glm::mat4 calculateProjection() {
// orthographic screen coordinates
	GLfloat		left, right, bottom, top;
// clipping plane distances for any projection
	GLfloat		zNear = 0.1f, zFar = 100.0f;

// orthographic shorter border will be [-5 ... 5], the other side is proportionally larger than the sorter
	if (window_width > window_height) {
		bottom	= -5.0f;
		top		=  5.0f;
		left	= -5.0f * (GLfloat)(window_width) / (GLfloat)(window_height);
		right	=  5.0f * (GLfloat)(window_width) / (GLfloat)(window_height);
	} else {
		left	= -5.0f;
		right	=  5.0f;
		bottom	= -5.0f * (GLfloat)(window_height) / (GLfloat)(window_width);
		top		=  5.0f * (GLfloat)(window_height) / (GLfloat)(window_width);
	}

//	return glm::perspective(glm::radians(45.0f), (GLfloat)window_width / (GLfloat)window_height, zNear, zFar);
	return glm::ortho(left, right, bottom, top, zNear, zFar);
}

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

void init(GLFWwindow* window) {
	{
		renderingProgram = createShaderProgram();

		/* L�trehozzuk a sz�ks�ges Vertex buffer �s vertex array objektumot. */
		glGenBuffers(numVBOs, VBO);
		glGenVertexArrays(numVAOs, VAO);

		/* T�pus meghat�roz�sa: a GL_ARRAY_BUFFER neves�tett csatol�ponthoz kapcsoljuk a buffert (ide ker�lnek a vertex adatok). */
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

		/* M�soljuk az adatokat a pufferbe! Megadjuk az aktu�lisan csatolt puffert,  azt hogy h�ny b�jt adatot m�solunk,
		a m�soland� adatot, majd a feldolgoz�s m�dj�t is meghat�rozzuk: most az adat nem v�ltozik a felt�lt�s ut�n */
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		/* Csatoljuk a vertex array objektumunkat a konfigur�l�shoz. */
		glBindVertexArray(VAO[0]);

		/* Ezen adatok szolg�lj�k a 0 index� vertex attrib�tumot (itt: poz�ci�).
		Els�k�nt megadjuk ezt az azonos�t�sz�mot.
		Ut�na az attrib�tum m�ret�t (vec3, l�ttuk a shaderben).
		Harmadik az adat t�pusa.
		Negyedik az adat normaliz�l�sa, ez maradhat FALSE jelen p�ld�ban.
		Az attrib�tum �rt�kek hogyan k�vetkeznek egym�s ut�n? Milyen l�p�sk�z ut�n tal�lom a k�vetkez� vertex adatait?
		V�g�l megadom azt, hogy honnan kezd�dnek az �rt�kek a pufferben. Most r�gt�n, a legelej�t�l veszem �ket.*/
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

		/* Enged�lyezz�k az im�nt defini�lt 0 index� attrib�tumot. */
		glEnableVertexAttribArray(0);

		/* Sz�n attrib�tum */
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		/* Lev�lasztjuk a vertex array objektumot �s a puufert is.*/
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// aktiv�ljuk a shader-program objektumunkat.
		glUseProgram(renderingProgram);
	}

// get the location of the uniform variables and store them
	modelLoc = glGetUniformLocation(renderingProgram, "model");
	viewLoc = glGetUniformLocation(renderingProgram, "view");
	projectionLoc = glGetUniformLocation(renderingProgram, "projection");

// set the matrices that do not change later
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

// set clearing color for black
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow* window, double currentTime) {
// fontos lehet minden egyes alkalommal t�r�lni!
// important to clear the screen every time 
	glClear(GL_COLOR_BUFFER_BIT); 

// rotate change for every frame
	rotateM	= glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// model matrix is the multiplication of the transformations, first one is on the right and the followings to the left one by one
	model	= rotateM * translateM * scaleM;
//	model	= translateM * rotateM * scaleM;

// pass the newly created model matrix to the vertex shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	/* Csatoljuk a vertex array objektumunkat. */
	glBindVertexArray(VAO[0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glPolygonMode(GL_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

	/* Lev�lasztjuk, nehogy b�rmilyen �rt�k fel�l�r�djon.*/
	glBindVertexArray(0);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);

// P�ld�k vet�t�si m�trixok megalkot�s�ra
// after resize, projection shall be recalculated
	projection = calculateProjection();
// pass the new matrix to the vertex shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
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

/** A jelenet�nk ut�ni takar�t�s. */
void cleanUpScene()
{
	/** T�r�lj�k a vertex puffer �s vertex array objektumokat. */
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);

	/** T�r�lj�k a shader programot. */
	glDeleteProgram(renderingProgram);
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
	glfwSetCursorPosCallback(window, cursorPosCallback);
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