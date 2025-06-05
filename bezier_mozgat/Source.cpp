/*
https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Explicit_definition
https://en.wikipedia.org/wiki/Factorial
https://hu.wikipedia.org/wiki/Binomi%C3%A1lis_egy%C3%BCtthat%C3%B3
*/
#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

using namespace std;

int		window_width = 600;
int		window_height = 600;
char	window_title[] = "Bezier curve";

std::vector<glm::vec3> pointToDraw;

//std::vector<glm::vec3> myControlPoints = {
//	glm::vec3(-0.5f, -0.5f, 0.0f),
//	glm::vec3(-0.5f,  0.5f, 0.0f),
//	glm::vec3( 0.5f, -0.5f, 0.0f),
//	glm::vec3( 0.5f,  0.5f, 0.0f)
//};

std::vector<glm::vec3> myControlPoints = {
	glm::vec3(100,200, 0),
	glm::vec3(200,340, 0),
	glm::vec3(400,400, 0),
	glm::vec3(550,200, 0),
	glm::vec3(220,100, 0)
};

std::vector<glm::vec3> myNDCControlPoints;

std::vector<glm::vec3> tangentLines;

/* Vertex buffer objektum és vertex array objektum az adattároláshoz.*/
#define numVBOs 1
#define numVAOs 3
GLuint VBO[numVBOs];
GLuint VAO[numVAOs];

GLuint renderingProgram;

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

GLuint createShaderProgram()
{
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
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}

void transformPointsToNDC(float windowWidth, float windowHeight) {
	for (const auto& p : myControlPoints) {
		float x_ndc = (p.x / (windowWidth / 2)) - 1.0f;
		float y_ndc = 1.0f - (p.y / (windowHeight / 2));

		myNDCControlPoints.push_back(glm::vec3(x_ndc, y_ndc, 0));

		std::cout << x_ndc << " " << y_ndc << endl;
	}

}

void calculateTangentialLines() {
	glm::vec3 startTangent = myNDCControlPoints[1] - myNDCControlPoints[0];
	glm::vec3 endTangent = myNDCControlPoints[myNDCControlPoints.size() - 1] - myNDCControlPoints[myNDCControlPoints.size() - 2];
	tangentLines.push_back(myNDCControlPoints[0]);
	tangentLines.push_back(startTangent);
	tangentLines.push_back(myNDCControlPoints.back());
	tangentLines.push_back(endTangent);
}

/*
The definition of N choose R is to compute the two productsand divide one with the other,
(N * (N - 1) * (N - 2) * ... * (N - R + 1)) / (1 * 2 * 3 * ... * R)
However, the multiplications may become too large really quickand overflow existing data type.The implementation trick is to reorder the multiplicationand divisions as,
(N) / 1 * (N - 1) / 2 * (N - 2) / 3 * ... * (N - R + 1) / R
It's guaranteed that at each step the results is divisible (for n continuous numbers, one of them must be divisible by n, so is the product of these numbers).
For example, for N choose 3, at least one of the N, N - 1, N - 2 will be a multiple of 3, and for N choose 4, at least one of N, N - 1, N - 2, N - 3 will be a multiple of 4.
C++ code given below.
*/
int NCR(int n, int r) {
/*
binomial coefficient
*/
	if (r == 0) return 1;

	/*
	 Extra computation saving for large R,
	 using property:
	 N choose R = N choose (N - R)
	*/
	if (r > n / 2) return NCR(n, n - r);

	long res = 1;

	for (int k = 1; k <= r; ++k) {
		res *= n - k + 1;
		res /= k;
	}

	return res;
}

/*
It will be the Bernstein basis polynomial of degree n.
*/
GLfloat blending(GLint n, GLint i, GLfloat t) {
	return NCR(n, i) * pow(t, i) * pow(1.0f - t, n - i);
}

void drawBezierCurve(std::vector<glm::vec3> controlPoints) {
/*
https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Explicit_definition
*/
	glm::vec3	nextPoint;
	GLfloat		t = 0.0f, B;
	GLfloat		increment = 1.0f / 100.0f; /* hány darab szakaszból rakjuk össze a görbénket? */

	while (t <= 1.0f) {
		nextPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < controlPoints.size(); i++) {
			B = blending(controlPoints.size() - 1, i, t);
			nextPoint.x += B * controlPoints.at(i).x;
			nextPoint.y += B * controlPoints.at(i).y;
			nextPoint.z += B * controlPoints.at(i).z;
		}

		pointToDraw.push_back(nextPoint);
		t += increment;
	}
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();

	transformPointsToNDC(window_width, window_height);

	drawBezierCurve(myNDCControlPoints);

	calculateTangentialLines();

	/* Létrehozzuk a szükséges Vertex buffer és vertex array objektumot. */
	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);

	/* Típus meghatározása: a GL_ARRAY_BUFFER nevesített csatolóponthoz kapcsoljuk a buffert (ide kerülnek a vertex adatok). */
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	/* Másoljuk az adatokat a pufferbe! Megadjuk az aktuálisan csatolt puffert,  azt hogy hány bájt adatot másolunk,
	a másolandó adatot, majd a feldolgozás módját is meghatározzuk: most az adat nem változik a feltöltés után */
	std::vector<glm::vec3> combinedPoints;
	combinedPoints.insert(combinedPoints.end(), pointToDraw.begin(), pointToDraw.end());
	combinedPoints.insert(combinedPoints.end(), myNDCControlPoints.begin(), myNDCControlPoints.end());
	combinedPoints.insert(combinedPoints.end(), tangentLines.begin(), tangentLines.end());
	size_t bezierPointsOffset = 0;
	size_t controlPointsOffset = pointToDraw.size();
	size_t tangentLinesOffset = controlPointsOffset + myNDCControlPoints.size();
	glBufferData(GL_ARRAY_BUFFER, combinedPoints.size() * sizeof(glm::vec3), combinedPoints.data(), GL_STATIC_DRAW);

	/* Csatoljuk a vertex array objektumunkat a konfiguráláshoz. */
	glBindVertexArray(VAO[0]);

	/* Ezen adatok szolgálják a 0 indexû vertex attribútumot (itt: pozíció).
	Elsõként megadjuk ezt az azonosítószámot.
	Utána az attribútum méretét (vec3, láttuk a shaderben).
	Harmadik az adat típusa.
	Negyedik az adat normalizálása, ez maradhat FALSE jelen példában.
	Az attribútum értékek hogyan következnek egymás után? Milyen lépésköz után találom a következõ vertex adatait?
	Végül megadom azt, hogy honnan kezdõdnek az értékek a pufferben. Most rögtön, a legelejétõl veszem õket.*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(bezierPointsOffset*sizeof(glm::vec3)));

	/* Engedélyezzük az imént definiált 0 indexû attribútumot. */
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(controlPointsOffset * sizeof(glm::vec3)));

	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO[2]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(tangentLinesOffset * sizeof(glm::vec3)));

	glEnableVertexAttribArray(0);

	/* Leválasztjuk a vertex array objektumot és a puffert is.*/
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// aktiváljuk a shader-program objektumunkat.
	glUseProgram(renderingProgram);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

/** A jelenetünk utáni takarítás. */
void cleanUpScene() {
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

	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_STRIP, 0, pointToDraw.size());

	glBindVertexArray(VAO[1]);
	glPointSize(6.0f);
	glDrawArrays(GL_POINTS, 0, myNDCControlPoints.size());

	glBindVertexArray(VAO[2]);
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, 4);

	/* Leválasztjuk, nehogy bármilyen érték felülíródjon.*/
	glBindVertexArray(0);
}

int main(void) {
	/* Próbáljuk meg inicializálni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/* A kívánt OpenGL verzió (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/* Próbáljuk meg létrehozni az ablakunkat. */
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
	glViewport(0, 0, window_width, window_height);

	/* Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

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