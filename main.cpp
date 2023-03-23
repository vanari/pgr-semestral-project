#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "object.h"
#include "shader.h"
#include "buffer.h"
#include "camera.h"

GLFWwindow* startWindow(GLuint width, GLuint height);

void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height);

void renderLoop();

void processInput();

void processInput(GLFWwindow* window);

bool screenChanged = false;
GLuint screenW = 640, screenH = 480;

int main(int argc, char* argv[]) {

    // discard later !!!

    GLfloat triangle[] = {
        0.f, .5f, 0.f,   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.5f, 2.0f,
        -.5f, -.5f, 0.f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,   0.f, 0.f,
        .5f, -.5f, 0.f,    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,   1.f, 0.f
    };
    
    GLfloat triangle2[] = {
        0.f, -1.f, 0.f,      0.5f, 2.0f,
        -.5f, -.5f, 0.f,    0.f, 0.f,
        .5f, -.5f, 0.f,      1.f, 0.f
    };

    // Init

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
    glEnable(GL_DEPTH_TEST);

    // DISCARD LATER !!!

    float* buf;
    unsigned int nVertices = buffer::load("assets/lober.obj", buf, nullptr, object::TEXTURED);

    //std::cout << "loaded buf size: " << sizeof(buf) << std::endl;

    shaderProgram shader("vertex.vert", "fragment.frag");
    
    auto pShader = &shader;
    
    camera cam = camera(1, &pShader);
    cam.setRatio(static_cast<float>(screenW/screenH));
    cam.setPos(glm::vec3(0.0f,0.0f,5.0f));

    cam.updateShaders();

    object meshObj(buf, nullptr, sizeof(float) * nVertices * object::TEXTURED, nVertices, shader);
    meshObj.loadTexture("assets/lobster.png");

    /*
    object triangleObj(triangle, sizeof(triangle), 3, shader);
    triangleObj.loadTexture("assets/troll.png");
    
    object triangle2Obj(triangle2, sizeof(triangle2), 3, shader);
    triangle2Obj.loadTexture("assets/awesomeface.png");
    */

    // Main loop

    while (!glfwWindowShouldClose(window)) {

        if (screenChanged) {
            cam.setRatio(float(screenW)/float(screenH));
            screenChanged = false;
        }

        processInput(window);

        glClearColor(0.f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto a = glfwGetTime();

        float x = 5*cos(a);
        float z = 5*sin(a);
        cam.setPos(glm::vec3(x, 0.0f, z));
        cam.setDir(-glm::vec3(x, 0.0f, z));
        cam.updateShaders();

        //triangleObj.draw();
        //triangle2Obj.draw();
        meshObj.draw();
        renderLoop();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    delete buf;
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
    screenChanged = true;
    screenW = static_cast<GLuint>(width);
    screenH = static_cast<GLuint>(height);

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