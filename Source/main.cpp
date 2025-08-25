#include "../Dependencies/glad/include/glad/glad.h"
#include "../Dependencies/glfw/include/glfw/glfw3.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define DEBUG false

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

GLFWwindow* Window;
int width, height;
float uWidth, uHeight;
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

	// Minimize again to show off my fancy print statements .w. //
	glfwIconifyWindow(Window);

	return true;
}

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

bool setUniform(const char* name, float value) {
	
	// Get the location of the requested uniform //
	int location = glGetUniformLocation(ShaderProgram, name);
	if (location == -1) { error("Could not get location of uniform '" + std::string(name) + "'."); return false; }

	// Set its value //
	glUniform1f(location, value);	

	return true;
}

bool setUniform(const char* name, float v1, float v2) {
	
	// Get the location of the requested uniform //
	int location = glGetUniformLocation(ShaderProgram, name);
	if (location == -1) { error("Could not get location of uniform '" + std::string(name) + "'."); return false; }

	// Set its value //
	glUniform2f(location, v1, v2);	

	return true;
}

bool setUniform(const char* name, float v1, float v2, float v3) {
	
	// Get the location of the requested uniform //
	int location = glGetUniformLocation(ShaderProgram, name);
	if (location == -1) { error("Could not get location of uniform '" + std::string(name) + "'."); return false; }

	// Set its value //
	glUniform3f(location, v1, v2, v3);	

	return true;
}

float uCameraPosition[3] = {0, 0, 0};
float CameraRotation[2] = {0, 0};
float uCameraDirection[3] = {0, 0, 0};
bool setUniforms() {
	print("Passing parameters to the GPU...");
	
	bool successState = true;

	// Set a uniform to store the screen dimensions & aspect ratio //
	successState &= setUniform("uWidth", uWidth);
	successState &= setUniform("uHeight", uHeight);
	successState &= setUniform("uAspectRatio", uWidth / uHeight);

	// Pass the camera settings to the GPU //
	successState &= setUniform("uCameraPosition", uCameraPosition[0], uCameraPosition[1], uCameraPosition[2]);
	successState &= setUniform("uCameraDirection", uCameraDirection[0], uCameraDirection[1], uCameraDirection[2]);

	return successState;
}

void mainloop();
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
	if (!createWindow()) { error("Could not create window."); return -1; }

	// Create a vertex buffer and populate it with a rectangle that covers the whole screen //
	if (!createVertexBuffer()) { error("Could not create and populate vertex buffer."); return -1; }

	// Create a shader program and shader objects //
	if (!createShaders()) { error("Could not create shaders."); return -1; }
	
	// Compile the shaders! //
	if (!compileShader(FragmentShader, "../Shaders/frag.glsl")) { error("Could not compile fragment shader."); return -1; }
	if (!compileShader(VertexShader, "../Shaders/vert.glsl")) { error("Could not compile vertex shader."); return -1; }

	// Finally, link and use the program //
	if (!linkProgram()) { error("Could not link shader program."); return -1; }

	// Set the necessary uniforms //
	if (!setUniforms()) { error("Could not set uniforms properly."); return -1; }

	///////////////
	// Main Loop //
	///////////////

	// Maximize window one last time before starting mainloop //
	glfwMaximizeWindow(Window);

	// Run mainloop until GLFW says we should stop //
	print("Running simulation!");
	while (!glfwWindowShouldClose(Window)) mainloop();

	// Tell GLFW to clean up its mess :3c //
	glfwTerminate();

	std::cout << std::endl;
	return 0;
}

void mainloop() {
	// Clear the screen //
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw our beautifully decorated rectangle //
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Tell GLFW to actually show all our hard work //
	glfwSwapBuffers(Window);

	// Make sure that key presses are handled //
	// Also, without this line it crashes -w- //
	glfwPollEvents();
}