#include "../Dependencies/glad/include/glad/glad.h"
#include "../Dependencies/glfw/include/glfw/glfw3.h"

#include <iostream>

GLFWwindow* Window;
int width, height;

void print(std::string msg) {
	std::cout << msg << std::endl;
}

void createWindow() {
	print(
		"\x1b[32m"
		">> "
		"\x1b[0;3m"
		"Creating window..."
		"\x1b[m"
	);

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
	glfwMinimizeWindow(Window);
}

void tick() {
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(Window);
	glfwPollEvents();
}

int main() {
	// Print Header :3 //
	print(
		"\x1b[1;34m"
		"-----------------------\n"
		"Not Enough Light v1.0.0\n"
		"-----------------------"
		"\x1b[m"
	);
	
	// Initialize GLFW //
	glfwInit();

	// Create a window to display graphics on //
	createWindow();

	// Maximize window one last time before starting mainloop //
	glfwMaximizeWindow(Window);

	// Run mainloop until GLFW says we should stop //
	while (!glfwWindowShouldClose(Window)) tick();

	// Tell GLFW to clean up its mess :3c //
	glfwTerminate();
	return 0;
}