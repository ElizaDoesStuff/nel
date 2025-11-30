#include "../Dependencies/glad/include/glad/glad.h"
#include "../Dependencies/glfw/include/glfw/glfw3.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <unordered_map>

using std::sin, std::cos;

#define DEBUG true
#define PI 3.1415926535897932384626433832795028841971693993

/////////////////////
// Print Functions //
/////////////////////

// A basic function to just print some debug prints with some nice colors and styling //
int color = 0;
std::string colors[3] = {"\x1b[31m", "\x1b[32m", "\x1b[34m"};
void print(std::string message) {
	std::cout << colors[color++ % 3] + ">> " + "\x1b[0;3m" + message + "\x1b[m" << std::endl;
}

// Similar function to print errors with a different styling //
void error(std::string message) {
	std::cerr << "\x1b[41;1m!! ERROR: " + message + "\x1b[m" << std::endl;
}

// Another print function for debug messages
void debug(std::string item, std::string message) {
	if (!DEBUG) return;
	std::cout << "\x1b[2;3m$$ DEBUG " + item + ": " + message + "\x1b[m" << std::endl;
}

//////////////
// Uniforms //
//////////////

float uFrame = 0;

float uCameraPosition[3] = {0, 0, 0};
float uCameraRotationMatrix[9]  = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
};

float uWidth, uHeight, uAspectRatio;

////////////
// Window //
////////////

GLFWwindow* Window;
int width, height;
bool ShouldExit = false, PauseStatus = false;
bool createWindow() {
	print("Creating window...");

	// Create a window //
	Window = glfwCreateWindow(1, 1, "Not Enough Light v1.0.0", nullptr, nullptr);
	if (!Window) { error("Could not create window object."); return false; }
	
	// Load OpenGL function pointers //
	// (This must be done after creating the window) //
	glfwMakeContextCurrent(Window);
	gladLoadGL(glfwGetProcAddress);	


	// Maximize the window and pass the dimensions over to OpenGL //
	glfwMaximizeWindow(Window);
	glfwGetWindowSize(Window, &width, &height);
	if (!width || !height) { error("Could not get window dimensions."); return false; }
	glViewport(0, 0, width, height);
	uWidth = (float)width;
	uHeight = (float)height;
	uAspectRatio = uWidth / uHeight;

	// Minimize again to show off my fancy print statements .w. //
	glfwIconifyWindow(Window);

	return true;
}

//////////////
// Geometry //
//////////////

unsigned int VertexBuffer;
bool createVertexBuffer() {
	print("Generating geometry...");
	
	// Create & bind vertex buffer //
	glGenBuffers(1, &VertexBuffer);
	if (VertexBuffer == 0) { error("Could not create vertex buffer."); return false; }
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	
	// Populate buffer with fullscreen quad //
	const float QuadMesh[] = {
		-1.0,  1.0,
		 1.0,  1.0,
		 1.0, -1.0,

		 1.0, -1.0,
		-1.0, -1.0,
		-1.0,  1.0,
	};
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), QuadMesh, GL_STATIC_DRAW);

	// Set up vertex attributes (literally just position lmao) //
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(0);

	// I was too lazy to reverse the winding order in my mesh TwT //
	glFrontFace(GL_CW);
	// Also for some reason clockwise winding is more intuitive to me idk //

	return true;
}

/////////////
// Shaders //
/////////////

unsigned int ShaderProgram, FragmentShader, VertexShader;
bool createShaders() {
	print("Creating shaders...");

	// Create all the necessary objects //
	ShaderProgram = glCreateProgram();
	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (FragmentShader == 0) { error("Failed to create fragment shader."); return false; }
	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (FragmentShader == 0) { error("Failed to create vertex shader."); return false; }

	return true;
}

bool compileShader(unsigned int Shader, std::string path) {
	print("Compiling shader '" + path + "'...");

	// Read file from path into a stringstream //
	std::ifstream fileStream(path);
	if (!fileStream.is_open()) { error("Could not open file '" + path + "'."); return false; }
	std::stringstream readStream;
	readStream << fileStream.rdbuf();

	// Read the contents of the file from the string stream //
	const std::string FileContents = readStream.str();

	// Print the contents for debugging //
	debug("FileContents", "\n" + FileContents);
	
	// Attach the shader source string to the shader object //
	const char* fileContentsString = FileContents.c_str();
	glShaderSource(Shader, 1, &fileContentsString, nullptr);

	// Compile the shader //
	glCompileShader(Shader);

	// Check for compilation errors //
	int compileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE) {
		int logLength;
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &logLength);
		
		char* InfoLog = (char*)malloc(logLength);
		glGetShaderInfoLog(Shader, logLength, NULL, InfoLog);
		print("BEGIN GLSL ERROR LOG");
		std::cout << InfoLog << std::endl;
		print("END GLSL ERROR LOG");
		free( (void*)InfoLog );

		glDeleteShader(Shader);
		error("Your shader code has an error in it, ya doofus -w-");
		return false;
	}

	return true;
}

bool linkProgram() {
	print("Linking program...");
	
	// Attach compiled shaders and link them into an executable //
	glAttachShader(ShaderProgram, FragmentShader);
	glAttachShader(ShaderProgram, VertexShader);
	glLinkProgram(ShaderProgram);
	glUseProgram(ShaderProgram);
	
	// Check for any linking errors (very rare) //
	int linkStatus;
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE) {
		error("Program linking failed.");
		return false;
	}
	
	// Print the number of attached shaders (for debugging) //
	int shaderCount;
	glGetProgramiv(ShaderProgram, GL_ATTACHED_SHADERS, &shaderCount);
	debug("shaderCount", std::to_string(shaderCount));

	return true;
}

//////////////
// Uniforms //
//////////////

bool setUniform(const char* name, float value) {
	
	// Get the location of the requested uniform //
	int location = glGetUniformLocation(ShaderProgram, name);
	if (location == -1) { error("Could not get location of uniform '" + std::string(name) + "'."); return false; }

	// Set its value //
	glUniform1f(location, value);	

	return true;
}

enum Uniform {
	FLOAT,
	INT,
	UINT,
	VEC3,
	MAT3
};

bool setUniform(const char* name, float* data, unsigned int type) {
	int location = glGetUniformLocation(ShaderProgram, name);
	if (location == -1) { error("Could not set uniform '" + std::string(name) + "' - Location could not be found."); return false; }
	switch (type) {
		case Uniform::FLOAT:
			glUniform1f(location, *data);
			break;
		case Uniform::INT:
			glUniform1i(location, (int)*data);
			break;
		case Uniform::UINT:
			glUniform1ui(location, (unsigned int)*data);
			break;
		case Uniform::VEC3:
			glUniform3f(location, data[0], data[1], data[2]);
			break;
		case Uniform::MAT3:
			glUniformMatrix3fv(location, 1, GL_TRUE, data);
			break;
		default:
			error("Could not set uniform '" + std::string(name) + "' - Type is not supported.");
			return false;
	}
	
	return true;
}

bool setInitialUniforms() {
	print("Passing parameters to the GPU...");
	
	bool successState = true;

	// Set uniforms to store the screen dimensions & aspect ratio //
	successState &= setUniform("uWidth", &uWidth, Uniform::FLOAT);
	successState &= setUniform("uHeight", &uHeight, Uniform::FLOAT);
	successState &= setUniform("uAspectRatio", &uAspectRatio, Uniform::FLOAT);

	return successState;
}

bool setPerFrameUniforms() {
	bool successState = true;
	
	successState &= setUniform("uCameraRotationMatrix", uCameraRotationMatrix, Uniform::MAT3);
	successState &= setUniform("uFrame", &uFrame, Uniform::UINT);

	return successState;
}

////////////
// Camera //
////////////

float CameraRotation[3] = {0, 0, 0};
bool calculateCamera() {
	// For SOME reason GLSL uniform mat3s are stored in column-major order //
	// Because of course, everyone just loves screwing with mathematicians //
	float newCameraRotationMatrix[9] = {
		cos( CameraRotation[1] ), -sin( CameraRotation[0] ) * sin( CameraRotation[1] ), -cos( CameraRotation[0] ) * sin( CameraRotation[1] ),
		0.0                     ,  cos( CameraRotation[0] )                           , -sin( CameraRotation[0] )                           ,
		sin( CameraRotation[1] ),  sin( CameraRotation[0] ) * cos( CameraRotation[1] ),  cos( CameraRotation[0] ) * cos( CameraRotation[1] ),
	};
	
	for (int i = 0; i < 9; i++) uCameraRotationMatrix[i] = newCameraRotationMatrix[i];

	return true;
}

////////////////////
// Event Handlers //
////////////////////

bool recompileShaders() {
	if (!compileShader(FragmentShader, "../Shaders/frag.glsl")) return false;
	if (!compileShader(VertexShader, "../Shaders/vert.glsl")) return false;

	// Finally, link and use the program //
	if (!linkProgram()) return false;

	// Set the necessary uniforms //
	if (!setInitialUniforms()) return false;

	print("Successfully recompiled shaders!!");
	return true;
}

float Delta;
float prevFrameTime = 0;
std::unordered_map<int, bool> KeyStates;
void handleKeypress(GLFWwindow* window, int key, int _, int action, int mods) {
	KeyStates[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) ShouldExit = true;
	if (action == GLFW_PRESS && key == GLFW_KEY_R) ShouldExit = !recompileShaders();
	if (action == GLFW_PRESS && key == GLFW_KEY_P) PauseStatus = !PauseStatus;
}

bool handleMovement() {
	if (KeyStates[GLFW_KEY_LEFT]) {
		CameraRotation[1] += PI / 2 * Delta;
	}
	if (KeyStates[GLFW_KEY_RIGHT]) {
		CameraRotation[1] -= PI / 2 * Delta;
	}
	if (KeyStates[GLFW_KEY_UP]) {
		CameraRotation[0] -= PI / 2 * Delta;
	}
	if (KeyStates[GLFW_KEY_DOWN]) {
		CameraRotation[0] += PI / 2 * Delta;
	}
	return true;
}

/////////////////////
// Main & Mainloop //
/////////////////////

bool mainloop();
int main() {
	// Print Header :3 //
	std::cout <<
		"\x1b[1m"
		"-----------------------\n"
		"Not Enough Light v1.0.0\n"
		"-----------------------"
		"\x1b[m"
	<< std::endl;
	
	///////////
	// Setup //
	///////////

	// Initialize GLFW //
	glfwInit();

	// Create a window to display graphics on //
	if (!createWindow()) { return -1; }

	// Tell GLFW to call our event handler when a key is pressed/released //
	glfwSetKeyCallback(Window, handleKeypress);

	// Initialize the KeyStates map with all false //
	for (int i = 0; i < 348; i++) {
		KeyStates[i] = false;
	}

	// Create a vertex buffer and populate it with a rectangle that covers the whole screen //
	if (!createVertexBuffer()) { error("Could not create and populate vertex buffer."); return -1; }

	// Create a shader program and shader objects //
	if (!createShaders()) { error("Could not create shaders."); return -1; }
	
	// Compile the shaders! //
	if (!compileShader(FragmentShader, "../Shaders/frag.glsl")) return -1;
	if (!compileShader(VertexShader, "../Shaders/vert.glsl")) return -1;

	// Finally, link and use the program //
	if (!linkProgram()) return -1;

	// Set the necessary uniforms //
	if (!setInitialUniforms()) return -1;

	///////////////
	// Main Loop //
	///////////////

	// Maximize window one last time before starting mainloop //
	glfwMaximizeWindow(Window);

	// Run mainloop until GLFW says we should stop //
	print("Running simulation!");
	while (!ShouldExit) if (!mainloop()) return -1;

	// Tell GLFW to clean up its mess :3c //
	glfwTerminate();

	std::cout << std::endl;
	return 0;
}

bool mainloop() {
	if (PauseStatus) return true;

	// Increment the frame counter //
	uFrame++;

	// Calculate Delta for framerate-independent movement //
	Delta = glfwGetTime() - prevFrameTime;
	prevFrameTime = glfwGetTime();

	// Handle player movement //
	handleMovement();
	
	// Bind the shaders //
	glUseProgram(ShaderProgram);
	
	// Calculate the camera rotation matrix and pass it to the GPU //
	if (!calculateCamera()) { error("Could not calculate rotation matrix for camera."); return false; }

	// Pass all updated parameters to the GPU //
	setPerFrameUniforms();

	// Clear the screen //
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw our beautifully decorated rectangle //
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Tell GLFW to actually show all our hard work //
	glfwSwapBuffers(Window);
	
	// Make sure that key presses are handled //
	// Also, without this line it crashes -w- //
	glfwPollEvents();
	if (glfwWindowShouldClose(Window)) ShouldExit = true;

	return true;
}