#include "../Dependencies/glad/include/glad/glad.h"
#include "../Dependencies/glfw/include/glfw/glfw3.h"

#include <iostream>

GLFWwindow* Window;
int width, height;

void print(std::string msg) {
	std::cout << msg << std::endl;
}

void createWindow() {
	
	Window = glfwCreateWindow(1, 1, "Not Enough Light v1.0.0", nullptr, nullptr);
	glfwMaximizeWindow(Window);
	glfwMakeContextCurrent(Window);
	gladLoadGL(glfwGetProcAddress);

	glfwGetWindowSize(Window, &width, &height);
	glViewport(0, 0, width, height);
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
		"-----------------------\n"
		"\x1b[m"
	);
	
	// Initialize GLFW //
	glfwInit();

	// Create a Window to display graphics on //
	createWindow();
	while (!glfwWindowShouldClose(Window)) tick();

	return 0;
}