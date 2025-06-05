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


/** Vizsgáljuk meg, hogy van-e aktuálisan OpenGL hiba, és amennyiben igen, írassuk ki azokat a konzolra egyenként. */
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

/** Vizsgáljuk meg, hogy van-e valami a Shader Info Logban, és amennyiben igen, írassuk ki azt a konzolra. */
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

/** Függvény, a shader fájlok sorainak beolvasásához. */
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

	/** Beolvassuk a shader fájlok tartalmát. */
	string vertShaderStr = readShaderSource("vertexShader.glsl");
	string fragShaderStr = readShaderSource("fragmentShader.glsl");

	/** Létrehozzuk a shader objektumainkat. */
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	/** A shader fájlok tartalmát eltároló string objektum szöveggé konvertálásás is elvégezzük. */
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	/** Ekkor a betöltött kódot hozzárendelhetjük a shader objektumainkhoz. */
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	/** Fordítsuk le ezen shader objektumhoz rendelt kódot. */
	glCompileShader(vShader);
	/** Hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}

	/** A második shader objektumhoz rendelt kódunkat is lefordítjuk. */
	glCompileShader(fShader);
	/** Ismét hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	/** Shader program objektum létrehozása: õ fogja össze a shadereket. Eltároljuk az ID értéket. */
	GLuint vfProgram = glCreateProgram();
	/** Csatoljuk a shadereket az elõzõ lépésben létrehozott objektumhoz. */
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	/** Végül a GLSL fordítónk ellenõrzi, hogy a csatolt shaderek kompatibilisek-e egymással. */
	/** GLSL linker checks the shaders for compatibility. */
	glLinkProgram(vfProgram);
	/** Ha hiba lépett fel, nézzük meg mi volt ennek az oka. */
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	/** Ha minden rendben ment a linkelés során, az objektumok leválaszthatóak a programról. */
	/** If everything is OK, the shaders can be destroyed. */
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	/** Az elkészült program azonosítója a visszatérési értékünk. */
	return vfProgram;
}

/** Felesleges objektumok törlése. */
/** Clenup the unnecessary objects. */
void cleanUpScene() {
	/** Töröljük a shader programot. */
	glDeleteProgram(renderingProgram);
	/** Töröljük a GLFW ablakot. */
	/** Destroy the GLFW window. */
	glfwDestroyWindow(window);
	/** Leállítjuk a GLFW-t. */
	/** Stop the GLFW system. */
	glfwTerminate();
	/** Kilépés EXIT_SUCCESS kóddal. */
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
	/** A rajzoláshoz használt shader programok betöltése. */
	renderingProgram = createShaderProgram();

	/** Aktiváljuk a shader-program objektumunkat. */
	/** Activate our shader-program object instead of the fix pipeline. */
	glUseProgram(renderingProgram);

	/** Ha kirajzoltuk a pontunkat, kísérletezzünk a pont méretének növelésével. */
	glEnable(GL_POINT_SMOOTH);
	glPointSize(30.0);
	/** Ha vonalat rajzolunk ki, ezzel módosíthatjuk a vastagságát. */
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
	/** Próbáljuk meg inicializálni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/** A használni kívánt OpenGL verzió (4.3). */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	/** Próbáljuk meg létrehozni az ablakunkat. */
	window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);

	/** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(window);

	/** A kurzor helyének vizsgálata. */
	glfwSetCursorPosCallback(window, cursorPosCallback);
	/** A képernyõ átméretezés kezelése.*/
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	/** Billentyûzethez köthetõ események kezelése. */
	glfwSetKeyCallback(window, keyCallback);
	/** Az egér gombjaihoz köthetõ események kezelése. */
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	glfwSetWindowAspectRatio(window, 1, 1);

	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések, pl. a shader objektumok létrehozása. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/** A kód, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display(window, glfwGetTime());
		/** Double buffered mûködés. */
		glfwSwapBuffers(window);
		/** Események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás. */
		glfwPollEvents();
	}

	/** Felesleges objektumok törlése. */
	cleanUpScene();
	/** Töröljük a GLFW ablakot. */
	glfwDestroyWindow(window);
	/** Leállítjuk a GLFW-t. */
	glfwTerminate();

	exit(EXIT_SUCCESS);
}