#include "../Dependencies/glad/include/glad/glad.h"
#include "../Dependencies/glfw/include/glfw/glfw3.h"

#include <iostream>
#include <string>

int color = 0;
std::string colors[3] = {"\x1b[31m", "\x1b[32m", "\x1b[34m"};
void print(const char* message) {
	std::cout << colors[color++ % 3] + ">> " + "\x1b[0;3m" + message + "\x1b[m" << std::endl;
}

GLFWwindow* Window;
int width, height;
void createWindow() {
	print("Creating window...");

	// Create a window //
	Window = glfwCreateWindow(1, 1, "Not Enough Light v1.0.0", nullptr, nullptr);
	
	// Load OpenGL function pointers //
	// (This must be done after creating the window) //
	glfwMakeContextCurrent(Window);
	gladLoadGL(glfwGetProcAddress);	


	// Maximize the window and pass the dimensions over to OpenGL //
	glfwMaximizeWindow(Window);
	glfwGetWindowSize(Window, &width, &height);
	glViewport(0, 0, width, height);

	// Minimize again to show off my fancy print statements .w. //
	glfwIconifyWindow(Window);
}

unsigned int VertexBuffer;
void createVertexBuffer() {
	print("Generating geometry...");
	
	// Create & bind vertex buffer //
	glGenBuffers(1, &VertexBuffer);
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

	// I was too lazy to reverse the winding order in my mesh //
	glFrontFace(GL_CW);
}

void tick() {
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glfwSwapBuffers(Window);
	glfwPollEvents();
}

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
	createWindow();

	// Create a vertex buffer and populate it with a rectangle that covers the whole screen //
	createVertexBuffer();

	///////////////
	// Main Loop //
	///////////////

	// Maximize window one last time before starting mainloop //
	glfwMaximizeWindow(Window);

	// Run mainloop until GLFW says we should stop //
	while (!glfwWindowShouldClose(Window)) tick();

	// Tell GLFW to clean up its mess :3c //
	glfwTerminate();
	return 0;
}