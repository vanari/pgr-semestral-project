#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "object.h"
#include "shader.h"
#include "buffer.h"
#include "camera.h"
#include "baseline.h"
#include "terrain.h"
#include "light.h"

GLFWwindow* startWindow(GLuint width, GLuint height);

void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height);

void renderLoop();

void processInput();

void processInput(GLFWwindow* window);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void toggleCamMode();

glm::vec3 moveInDir();

bool screenChanged = false;

enum CameraMode
{
    FREE_CAMERA,
    RTS_CAMERA,
    PLATFORM_CAMERA,
    PILOT
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

    struct KeyStates {
        bool W = 0;
        bool S = 0;
        bool D = 0;
        bool A = 0;
        bool Q = 0;
        bool E = 0;

    } keyStates;
//cu
    double speed = 0;

    struct Mouse {
        bool used = false;
        double x;
        double y;
        double deltaX;
        double deltaY;
    } mouse;

    bool pilotMode = false;

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
    float loadMtls[10];
    //buffer::load("assets/A6M_ZERO.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED);

    std::cout << "loaded buf size: " << nVertices << std::endl;
    std::cout << "loaded indices: " << nIndices << std::endl;

    shaderProgram shader("vertex.vert", "fragment.frag");
    shaderProgram red("vertex.vert", "red.frag");
    shaderProgram shaderOsc("osc.vert", "oscilating2.frag");
    shaderProgram skyBoxShader("skybox.vert", "fragment.frag");
    shaderProgram phongShader("phong.vert", "phong.frag");


    buffer::load("assets/skybox.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object skyBox(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, skyBoxShader);
    skyBox.loadTexture("assets/skybox.png", object::RGBA, 0);
    skyBox.scale(57.f, 57.f, 57.f);
    skyBox.updateModel();
    

    shaderProgram* pShaders[] = {&skyBoxShader, &shader, &shaderOsc, &phongShader};
    int nShaders = 4;
    
    lightManager::init(pShaders, nShaders);
    //lightManager::add(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    lightManager::add(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0, 0), 15.0f);

    camera cam = camera(nShaders, pShaders);
    cam.setRatio((float(screenW)/float(screenH)));
    cam.setPos(glm::vec3(0.0f,0.0f,0.0f));
    cam.setDir(gameState.direction);
    cam.setUpDir(gameState.upDir);
    cam.calculate();

    cam.updateShaders();

    camera platformCam = camera(nShaders, pShaders);

    platformCam.setRatio((float(screenW)/float(screenH)));
    platformCam.setPos(glm::vec3(0.0f,0.0f,0.0f));
    platformCam.setDir(gameState.direction);
    platformCam.setUpDir(gameState.upDir);
    platformCam.calculate();


    camera RTSCam = camera(nShaders, pShaders);

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

    // V1 GETS LOADED

    buffer::load("assets/v1-main.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object v1(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shaderOsc);
    v1.loadTexture("assets/V1-BASE-FIRE.png", object::RGBA, 0);
    v1.loadTexture("assets/V1-BASE.png", object::RGBA, 1);
    
    buffer::load("assets/tail-top.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object v1_tail_top(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    v1_tail_top.loadTexture("assets/V1-COMP.png", object::RGBA, 0);

    buffer::load("assets/tail-L.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object v1_tail_left(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    v1_tail_left.loadTexture("assets/V1-COMP.png", object::RGBA, 0);

    buffer::load("assets/tail-R.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object v1_tail_right(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    v1_tail_right.loadTexture("assets/V1-COMP.png", object::RGBA, 0);
    
    buffer::load("assets/helix.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object v1_helix(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    v1_helix.loadTexture("assets/V1-COMP.png", object::RGBA, 0);
    
    v1.attachChild(&v1_tail_top);
    v1.attachChild(&v1_tail_left);
    v1.attachChild(&v1_tail_right);
    v1.attachChild(&v1_helix);


    v1_tail_top.setCustomAxis(0, 0, 1.0f);
    v1_tail_top.setCustomPos(0, 1.7f, 0);

    v1_tail_left.setCustomAxis(1.0f, 0, 0);
    v1_tail_left.setCustomPos(0, 1.7f, 0);
    
    v1_tail_right.setCustomAxis(1.0f, 0, 0);
    v1_tail_right.setCustomPos(0, 1.7f, 0);

    v1_helix.setCustomAxis(0, 1.0f, 0);
    v1_helix.setCustomPos(0,0,0);

    // --- mushy mushroom ---

    buffer::load("assets/shroom_cluster1.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object sc(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    sc.loadTexture("assets/shroom_color2.png", object::RGBA, 0);
    sc.setPos(glm::vec3(0.0f,0.0f,10.0f));
    sc.scale(3.0f, 3.0f, 3.0f);
    sc.updateModel();
 

    buffer::load("assets/shroom_cluster2.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object sc2(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    sc2.loadTexture("assets/shroom_color2.png", object::RGBA, 0);
    sc2.setPos(glm::vec3(0.0f,0.0f,0.0f));
    sc2.scale(10.0f, 10.0f, 10.0f);
    sc2.updateModel();
    sc2.setMtl(loadMtls);

    lightManager::updateShaders();

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

        auto viewXY = glm::normalize(glm::vec3(v1.getNavDir().x, v1.getNavDir().y, 0.0f));

        if (gameState.camMode == FREE_CAMERA)
            gameState.enabledCamera->translate(moveInDir());
        if (gameState.camMode == PILOT)
            gameState.enabledCamera->setPos(v1.getPos() + glm::vec3(0,0.0f,3.0f) - viewXY * 4.0f);
        
        if (gameState.camMode == FREE_CAMERA)
            gameState.enabledCamera->setDir(gameState.direction);
        if (gameState.camMode == PILOT)
            gameState.enabledCamera->setDir(glm::normalize(glm::vec3(0,0.0f,-2.0f) + viewXY * 4.0f));

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

        skyBox.setPos(gameState.enabledCamera->getPos());
        skyBox.updateModel();

        if (gameState.camMode == FREE_CAMERA) {
            lightManager::setDir(0, gameState.direction);
            lightManager::setPos(0, gameState.enabledCamera->getPos());
            lightManager::updateShaders();
        }

        if (gameState.camMode == PILOT) {

            //v1.rotateErel(0,0,(float)gameState.mouse.deltaX*(0.1f));
            //v1_tail.setDir(gameState.direction);

            auto pitch = 10 * gameState.motion.front;
            auto yaw = 10 * gameState.motion.roll;
            auto roll = -10 * gameState.motion.side;
            v1.pitch(pitch);
            v1.yaw(yaw);
            v1.roll(roll);

            if (gameState.keyStates.Q == true) {
                v1_tail_top.customRotate(45.0f);
            } if (gameState.keyStates.E == true) {
                v1_tail_top.customRotate(-45.0f);
            }
            
            if (gameState.keyStates.W == true) {
                v1_tail_right.customRotate(22.0f);
                v1_tail_left.customRotate(22.0f);
            } if (gameState.keyStates.S == true) {
                v1_tail_right.customRotate(-22.0f);
                v1_tail_left.customRotate(-22.0f);
            }
            
            if (gameState.keyStates.A == true) {
                v1_tail_right.customRotate(-22.0f);
                v1_tail_left.customRotate(22.0f);
            } if (gameState.keyStates.D == true) {
                v1_tail_right.customRotate(22.0f);
                v1_tail_left.customRotate(-22.0f);
            }

            float rotation = 150*glfwGetTime();

            v1_helix.customRotate(rotation);

            //v1.updateModel();

            v1.ttranslate(0.3f);
        }
        
        gameState.motion.front = 0;
        gameState.motion.side = 0;
        gameState.motion.up = 0;
        gameState.motion.roll = 0;

        terrain.draw();
        skyBox.draw();
        v1.draw();
        sc.draw();
        sc2.draw();
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
    static bool I_is_pressed = false;

    gameState.keyStates.Q = false;
    gameState.keyStates.E = false;
    gameState.keyStates.W = false;
    gameState.keyStates.S = false;
    gameState.keyStates.A = false;
    gameState.keyStates.D = false;

    static double prevTime = 0;
    double deltaTime = glfwGetTime() - prevTime;
    prevTime = glfwGetTime();

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_PRESS) {
        if (I_is_pressed) {
        } else {
            I_is_pressed = true;
            toggleCamMode();
            };
    } else if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_RELEASE) {
        I_is_pressed = false;
    } 

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        gameState.keyStates.W = true;
        gameState.motion.hustle.z -= deltaTime * gameState.speed;
        gameState.motion.front += deltaTime * gameState.speed;
    }
    
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        gameState.keyStates.S = true;
        gameState.motion.hustle.z += deltaTime * gameState.speed;
        gameState.motion.front -= deltaTime * gameState.speed;
    }
    
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        gameState.keyStates.A = true;
        gameState.motion.hustle.x -= deltaTime * gameState.speed;
        gameState.motion.side -= deltaTime * gameState.speed;   
    }
    
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        gameState.keyStates.D = true;
        gameState.motion.hustle.x += deltaTime * gameState.speed;
        gameState.motion.side += deltaTime * gameState.speed;   
    }

    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        gameState.keyStates.Q = true;
        gameState.motion.roll -= deltaTime * gameState.speed;   
    }
    
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        gameState.keyStates.E = true;
        gameState.motion.roll += deltaTime * gameState.speed;   
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

void toggleCamMode() {
    if (gameState.camMode == PILOT)
        gameState.camMode = FREE_CAMERA;
    else if (gameState.camMode == FREE_CAMERA) {
        gameState.camMode = PILOT;
    }
}