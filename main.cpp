#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "object.h"
#include "shader.h"
#include "buffer.h"
#include "camera.h"
#include "baseline.h"
#include "terrain.h"

GLFWwindow* startWindow(GLuint width, GLuint height);

void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height);

void renderLoop();

void processInput();

void processInput(GLFWwindow* window);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

glm::vec3 moveInDir();

bool screenChanged = false;

enum CameraMode
{
    FREE_CAMERA,
    RTS_CAMERA,
    PLATFORM_CAMERA
};


struct GameState {
    GLuint screenH = 480, screenW = 640;

    glm::vec3 direction;
    glm::vec3 upDir = glm::vec3(0.0f, 0.0f, 1.0f);

    struct Motion {
        glm::vec3 hustle;
        double front;
        double up;
        double side;
        double roll;
    } motion;

    double speed = 0;

    struct Mouse {
        bool used = false;
        double x;
        double y;
        double deltaX;
        double deltaY;
    } mouse;

    CameraMode camMode;
    camera* enabledCamera;

} gameState;

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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

    // DISCARD LATER !!!

    gameState.speed = 5.0;
    gameState.direction = glm::vec3(0.0f, -0.70710678118f, -0.70710678118f);
    gameState.camMode = FREE_CAMERA;


    float* buf;
    unsigned int* indices;
    unsigned int nVertices;
    unsigned int nIndices;
    //buffer::load("assets/A6M_ZERO.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED);

    std::cout << "loaded buf size: " << nVertices << std::endl;
    std::cout << "loaded indices: " << nIndices << std::endl;

    shaderProgram shader("vertex.vert", "fragment.frag");
    shaderProgram shader2("hustleVertex.vert", "husleFrag.frag");
    
    
    auto pShader = &shader;
    
    camera cam = camera(1, &pShader);
    cam.setRatio((float(screenW)/float(screenH)));
    cam.setPos(glm::vec3(0.0f,0.0f,0.0f));
    cam.setDir(gameState.direction);
    cam.setUpDir(gameState.upDir);
    cam.calculate();

    cam.updateShaders();

    camera platformCam = camera(1, &pShader);

    platformCam.setRatio((float(screenW)/float(screenH)));
    platformCam.setPos(glm::vec3(0.0f,0.0f,0.0f));
    platformCam.setDir(gameState.direction);
    platformCam.setUpDir(gameState.upDir);
    platformCam.calculate();


    camera RTSCam = camera(1, &pShader);

    RTSCam.setRatio((float(screenW)/float(screenH)));
    RTSCam.setPos(glm::vec3(0.0f, 0.0f, -1.0f));
    RTSCam.setDir(gameState.direction);
    RTSCam.setUpDir(gameState.upDir);
    RTSCam.calculate();

    gameState.enabledCamera = &RTSCam;


    //Baseline floor(shader);
    Terrain terrain(shader, "assets/4tiles.jpg");

    //object meshObj(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    //meshObj.loadTexture("assets/A6M_ZERO_D.tga");


    // Main loop
    std::cout << "Main loop" << std::endl;

    while (!glfwWindowShouldClose(window)) {

        if (screenChanged) {
            cam.setRatio(float(screenW)/float(screenH));
            cam.calculate();
            screenChanged = false;
        }

        processInput(window);

        glClearColor(0.f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto a = glfwGetTime();

        float x = 5*cos(a);
        float z = 5*sin(a);
        //meshObj.rotateEabs(85.0f,0.0f,0);

        //meshObj.setDir(glm::vec3(x, 0.0f, z));

        //cam.setPos(glm::vec3(x, 0.0f, z));
        //cam.setDir(glm::vec3(-x, 0.0f, -z));
        gameState.enabledCamera->translate(moveInDir());
        gameState.enabledCamera->setDir(gameState.direction);
        gameState.enabledCamera->calculate();
        gameState.enabledCamera->updateShaders();
        //gameState.enabledCamera->log();

        terrain.setCenter(gameState.enabledCamera->getPos().x, gameState.enabledCamera->getPos().y);
        //floor.setCenter(gameState.enabledCamera->getPos().x, gameState.enabledCamera->getPos().y);
        //floor.fillBuffer();

        //triangleObj.draw();
        //triangle2Obj.draw();
        //meshObj.draw();
        //floor.draw();

        terrain.draw();
        renderLoop();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    delete buf;
    glfwTerminate();
    return 0;
}

glm::vec3 moveInDir() {
    glm::vec3 side(0);
    side = glm::cross(gameState.upDir, gameState.direction);
    side = glm::normalize(side);
    glm::vec3 translation = gameState.direction * float(gameState.motion.front) + side * float(gameState.motion.side) + gameState.upDir * float(gameState.motion.up);
    gameState.motion.front = 0;
    gameState.motion.side = 0;
    gameState.motion.up = 0;
    return translation;
}

void renderLoop() {

}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //std::cout << "x: " << xpos << " y: " << ypos << std::endl;
    static double prevX = xpos;
    static double prevY = ypos;
    gameState.mouse.x = xpos;
    gameState.mouse.y = ypos;

    gameState.mouse.deltaX = xpos - prevX;
    gameState.mouse.deltaY = ypos - prevY;

    //std::cout << "dX: " << gameState.mouse.deltaX << std::endl;

    prevX = xpos;
    prevY = ypos;
    
    glm::vec3 side = glm::cross(gameState.direction, gameState.upDir);

    glm::mat4 rot(1.0f);
    rot = glm::rotate(rot, glm::radians((float)gameState.mouse.deltaX*(0.1f)), gameState.upDir);
    if (gameState.camMode == FREE_CAMERA)
        rot = glm::rotate(rot, glm::radians((float)gameState.mouse.deltaY*(-0.1f)), side);
    auto bruh = rot * glm::vec4(gameState.direction, 1.0f);
    gameState.direction = glm::vec3(bruh.x, bruh.y, bruh.z);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void processInput(GLFWwindow* window) {
    static double prevTime = 0;
    double deltaTime = glfwGetTime() - prevTime;
    prevTime = glfwGetTime();

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        gameState.motion.hustle.z -= deltaTime * gameState.speed;
        gameState.motion.front += deltaTime * gameState.speed;
    }
    
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        gameState.motion.hustle.z += deltaTime * gameState.speed;
        gameState.motion.front -= deltaTime * gameState.speed;
    }
    
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        gameState.motion.hustle.x -= deltaTime * gameState.speed;
        gameState.motion.side -= deltaTime * gameState.speed;   
    }
    
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        gameState.motion.hustle.x += deltaTime * gameState.speed;
        gameState.motion.side += deltaTime * gameState.speed;   
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        gameState.motion.up += deltaTime * gameState.speed;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        gameState.motion.up -= deltaTime * gameState.speed;
    }
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

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL program", nullptr, nullptr);

	glfwMakeContextCurrent(window);

    return window;
}