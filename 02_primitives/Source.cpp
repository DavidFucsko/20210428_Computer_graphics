#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#define USE_SHADERS

using namespace std;


int			window_width	= 600;
int			window_height	= 600;
char		window_title[]	= "Drawing primitives";
GLFWwindow* window			= nullptr;
// normal keys are fom [0..255], arrow and special keys are from [256..511]
GLboolean	keyboard[512]	= {GL_FALSE};

GLuint		renderingProgram;


/** Vizsg�ljuk meg, hogy van-e aktu�lisan OpenGL hiba, �s amennyiben igen, �rassuk ki azokat a konzolra egyenk�nt. */
/** Check for OpenGL errors, and send them to the console ony by one. */
bool checkOpenGLError() {
	bool	foundError	= false;
	int		glErr		= glGetError();

	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError	= true;
		glErr		= glGetError();
	}

	return foundError;
}

/** Vizsg�ljuk meg, hogy van-e valami a Shader Info Logban, �s amennyiben igen, �rassuk ki azt a konzolra. */
/** Check for Shader Info Log, and send it to the console if it is created for the last compile. */
void printShaderLog(GLuint shader) {
	int		len			= 0;
	int		chWrittn	= 0;
	char	*log;
	
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

/** F�ggv�ny, a shader f�jlok sorainak beolvas�s�hoz. */
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

	/** Beolvassuk a shader f�jlok tartalm�t. */
	string vertShaderStr = readShaderSource("vertexShader.glsl");
	string fragShaderStr = readShaderSource("fragmentShader.glsl");

	/** L�trehozzuk a shader objektumainkat. */
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	/** A shader f�jlok tartalm�t elt�rol� string objektum sz�vegg� konvert�l�s�s is elv�gezz�k. */
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	/** Ekkor a bet�lt�tt k�dot hozz�rendelhetj�k a shader objektumainkhoz. */
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	/** Ford�tsuk le ezen shader objektumhoz rendelt k�dot. */
	glCompileShader(vShader);
	/** Hibakeres�si l�p�sek. P�ld�ul sikeres volt-e a ford�t�s? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}

	/** A m�sodik shader objektumhoz rendelt k�dunkat is leford�tjuk. */
	glCompileShader(fShader);
	/** Ism�t hibakeres�si l�p�sek. P�ld�ul sikeres volt-e a ford�t�s? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	/** Shader program objektum l�trehoz�sa: � fogja �ssze a shadereket. Elt�roljuk az ID �rt�ket. */
	GLuint vfProgram = glCreateProgram();
	/** Csatoljuk a shadereket az el�z� l�p�sben l�trehozott objektumhoz. */
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	/** V�g�l a GLSL ford�t�nk ellen�rzi, hogy a csatolt shaderek kompatibilisek-e egym�ssal. */
	/** GLSL linker checks the shaders for compatibility. */
	glLinkProgram(vfProgram);
	/** Ha hiba l�pett fel, n�zz�k meg mi volt ennek az oka. */
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	/** Ha minden rendben ment a linkel�s sor�n, az objektumok lev�laszthat�ak a programr�l. */
	/** If everything is OK, the shaders can be destroyed. */
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	/** Az elk�sz�lt program azonos�t�ja a visszat�r�si �rt�k�nk. */
	return vfProgram;
}

/** Felesleges objektumok t�rl�se. */
/** Clenup the unnecessary objects. */
void cleanUpScene() {
	/** T�r�lj�k a shader programot. */
	glDeleteProgram(renderingProgram);
	/** T�r�lj�k a GLFW ablakot. */
	/** Destroy the GLFW window. */
	glfwDestroyWindow(window);
	/** Le�ll�tjuk a GLFW-t. */
	/** Stop the GLFW system. */
	glfwTerminate();
	/** Kil�p�s EXIT_SUCCESS k�ddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	exit(EXIT_SUCCESS);
}
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
// terminate
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		cleanUpScene();

	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
}

void init(GLFWwindow* window) {
	/** A rajzol�shoz haszn�lt shader programok bet�lt�se. */
	renderingProgram = createShaderProgram();

	/** Aktiv�ljuk a shader-program objektumunkat. */
	/** Activate our shader-program object instead of the fix pipeline. */
	glUseProgram(renderingProgram);

	/** Ha kirajzoltuk a pontunkat, k�s�rletezz�nk a pont m�ret�nek n�vel�s�vel. */
	glEnable(GL_POINT_SMOOTH);
	glPointSize(30.0);
	/** Ha vonalat rajzolunk ki, ezzel m�dos�thatjuk a vastags�g�t. */
	glLineWidth(10.0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow* window, double currentTime) {
/* Primitive types in core and compatibility profiles.
https://www.cs.uregina.ca/Links/class-info/315/WWW/Lab2/
*/
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
//	glBegin(GL_LINE_LOOP);
//	glBegin(GL_TRIANGLES);
	//	glColor3f(1.0f, .0f, .0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	//glVertex4f(0.25, -0.25, 0.0, 1.0);
	//glVertex4f(-0.25, -0.25, 0.0, 1.0);
	//glVertex4f(0.0, 0.25, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glEnd();
}

int main(void) {
	/** Pr�b�ljuk meg inicializ�lni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/** A haszn�lni k�v�nt OpenGL verzi� (4.3). */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	/** Pr�b�ljuk meg l�trehozni az ablakunkat. */
	window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);

	/** V�lasszuk ki az ablakunk OpenGL kontextus�t, hogy haszn�lhassuk. */
	glfwMakeContextCurrent(window);

	/** A kurzor hely�nek vizsg�lata. */
	glfwSetCursorPosCallback(window, cursorPosCallback);
	/** A k�perny� �tm�retez�s kezel�se.*/
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	/** Billenty�zethez k�thet� esem�nyek kezel�se. */
	glfwSetKeyCallback(window, keyCallback);
	/** Az eg�r gombjaihoz k�thet� esem�nyek kezel�se. */
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/** Incializ�ljuk a GLEW-t, hogy el�rhet�v� v�ljanak az OpenGL f�ggv�nyek. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	glfwSetWindowAspectRatio(window, 1, 1);

	/** Az alkalmaz�shoz kapcsol�d� el�k�sz�t� l�p�sek, pl. a shader objektumok l�trehoz�sa. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/** A k�d, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display(window, glfwGetTime());
		/** Double buffered m�k�d�s. */
		glfwSwapBuffers(window);
		/** Esem�nyek kezel�se az ablakunkkal kapcsolatban, pl. gombnyom�s. */
		glfwPollEvents();
	}

	/** Felesleges objektumok t�rl�se. */
	cleanUpScene();
	/** T�r�lj�k a GLFW ablakot. */
	glfwDestroyWindow(window);
	/** Le�ll�tjuk a GLFW-t. */
	glfwTerminate();

	exit(EXIT_SUCCESS);
}