#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "object.h"
#include "shader.h"

GLFWwindow* startWindow(GLuint width, GLuint height);

void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height);

void renderLoop();

void processInput();

void processInput(GLFWwindow* window);

int main(int argc, char* argv[]) {

    // discard later !!!

    GLfloat triangle[] = {
        0.f, .5f, 0.f,      0.5f, 2.0f,
        -.5f, -.5f, 0.f,    0.f, 0.f,
        .5f, -.5f, 0.f,      1.f, 0.f
    };

    // Init

    GLuint screenW = 640, screenH = 480;

    if (argc >= 3) {
        screenW = atoi(argv[1]);
        screenH = atoi(argv[2]);
    }

    std::cout << "Program started with width: " << screenW << " and height: " << screenH << '.' << std::endl;

    GLFWwindow* window = startWindow(screenW, screenH);

    if (window == nullptr) {
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    glViewport(0, 0, screenW, screenH);

    // Setup

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // DISCARD LATER !!!

    shaderProgram shader("vertex.vert", "fragment.frag");
    object triangleObj(triangle, sizeof(triangle), 9, shader);
    triangleObj.loadTexture("assets/troll.png");
    
    // Main loop

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        triangleObj.draw();
        renderLoop();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void renderLoop() {

}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height) {
    glViewport(0, 0, width, height);
}

GLFWwindow* startWindow(GLuint width, GLuint height) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Program", nullptr, nullptr);

	glfwMakeContextCurrent(window);

    return window;
}