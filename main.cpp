/*
   File: main.cpp
   Description: Semestral project for PGR using OpenGL.
   Author: Richard Váňa
   Date: May 23, 2023
*/

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

void togglePCamMode();

glm::vec3 moveInDir();

bool screenChanged = false;

enum CameraMode
{
    FREE_CAMERA,
    RTS_CAMERA,
    PLATFORM_CAMERA,
    PILOT,
    PASSENGER
};

// global struct to set the game state

struct GameState {
    int passengerState = 0;

    bool usedShrooms[256];

    float fogDensity = 0.0f;

    float transition = 1.0;

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
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glBlendFunc(GL_ONE, GL_ONE);  

    for (int i = 0; i < 256; ++i)
        gameState.usedShrooms[i] = false;


    gameState.speed = 5.0;
    gameState.direction = glm::vec3(0.0f, -0.70710678118f, -0.70710678118f);
    gameState.camMode = FREE_CAMERA;


    // buffer pointers
    float* buf;
    unsigned int* indices;
    unsigned int nVertices;
    unsigned int nIndices;
    float loadMtls[10];
    //buffer::load("assets/A6M_ZERO.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED);

    std::cout << "loaded buf size: " << nVertices << std::endl;
    std::cout << "loaded indices: " << nIndices << std::endl;

    // shaders
    shaderProgram shader("phong.vert", "fragment.frag");
    shaderProgram red("vertex.vert", "red.frag");
    shaderProgram shaderOsc("osc.vert", "oscilating2.frag");
    shaderProgram skyBoxShader("skybox.vert", "fragment.frag");
    shaderProgram phongShader("phong.vert", "phong.frag");
    shaderProgram slideShader("slide.vert", "phong.frag");
    shaderProgram cursorShader("cursor.vert", "fragment.frag");
    shaderProgram expS("expl.vert", "fragment.frag");

    // skybox

    buffer::load("assets/skybox.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, nullptr);
    object skyBox(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    skyBox.loadTexture("assets/skybox.png", object::RGBA, 0);
    skyBox.scale(57.f, 57.f, 57.f);
    skyBox.updateModel();
    
    // shaders used

    shaderProgram* pShaders[] = {&skyBoxShader, &shader, &cursorShader, &phongShader, &slideShader, &expS};
    int nShaders = 6;

    // light setup

    lightManager::init(pShaders, nShaders);
    //lightManager::add(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    lightManager::add(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0, 0), 15.0f);
    lightManager::addDir(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    lightManager::add(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(1.0f,0,0));

    //camera setup

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


    //generated terrain 
    
    Terrain terrain(phongShader, "assets/grassTiles2.jpg");
    terrain.pShroomShader = &phongShader;

    //object meshObj(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, shader);
    //meshObj.loadTexture("assets/A6M_ZERO_D.tga");

    // CURSOR

    buffer::load("assets/cursorHolder2.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, 0);
    object cursor(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, cursorShader);
    cursor.loadTexture("assets/cursor_2.png", object::RGBA, 0);
    cursor.scale(0.02f, 0.02f, 0);
    cursor.screenRatio = float(screenW)/float(screenH);
    cursor.updateModel();

    // explosion

    buffer::load("assets/expl.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, 0);
    object exp(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, expS);
    exp.loadTexture("assets/exp.png", object::RGBA, 0);
    exp.screenRatio = float(screenW)/float(screenH);
    exp.updateModel();


    // V1 GETS LOADED

    buffer::load("assets/v1-main--m.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object v1(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    v1.setMtl(loadMtls);
    v1.loadTexture("assets/V1-BASE-FIRE.png", object::RGBA, 0);
    v1.loadTexture("assets/V1-BASE.png", object::RGBA, 1);
    
    buffer::load("assets/tail-top.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object v1_tail_top(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    v1_tail_top.setMtl(loadMtls);
    v1_tail_top.loadTexture("assets/V1-COMP.png", object::RGBA, 0);

    buffer::load("assets/tail-L.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object v1_tail_left(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    v1_tail_left.setMtl(loadMtls);
    v1_tail_left.loadTexture("assets/V1-COMP.png", object::RGBA, 0);

    buffer::load("assets/tail-R.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object v1_tail_right(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    v1_tail_right.setMtl(loadMtls);
    v1_tail_right.loadTexture("assets/V1-COMP.png", object::RGBA, 0);
    
    buffer::load("assets/helix.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object v1_helix(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    v1_helix.setMtl(loadMtls);
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

    // --- mushrooms ---

    buffer::load("assets/shroom_cluster1.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object sc(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    sc.loadTexture("assets/shroom_color2.png", object::RGBA, 0);
    sc.setMtl(loadMtls);
    sc.setPos(glm::vec3(0.0f,0.0f,10.0f));
    sc.scale(3.0f, 3.0f, 3.0f);
    sc.updateModel();
    sc.setMtl(loadMtls);
 

    buffer::load("assets/shroom_cluster1.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object sc2(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, phongShader);
    sc2.loadTexture("assets/shroom_color.png", object::RGBA, 0);
    sc2.setPos(glm::vec3(25.0f, 30.0f,terrain.getZlvl(25.0f, 30.0f)));
    sc2.scale(10.0f, 10.0f, 10.0f);
    sc2.updateModel();
    sc2.setMtl(loadMtls);

    buffer::load("assets/valec.obj", &nVertices, buf, &nIndices, indices, object::TEXTURED, loadMtls);
    object valec(nVertices * object::TEXTURED, nVertices, buf, nIndices, indices, slideShader);
    valec.loadTexture("assets/valec.png", object::RGBA, 0);
    valec.setPos(glm::vec3(5.0f,-25.0f,1.0f+terrain.getZlvl(5.0f, -25.f)));
    //valec.scale(10.0f, 10.0f, 10.0f);
    valec.updateModel();
    valec.setMtl(loadMtls);


    lightManager::updateShaders();

    // Main loop
    std::cout << "Main loop" << std::endl;
    glm::vec3 initPos;
    glm::vec3 initDir;
    float explodingTime = 0.0f;
    float currentTime = 0.0f;

    while (!glfwWindowShouldClose(window)) {

        if (screenChanged) {
            cam.setRatio(float(screenW)/float(screenH));
            cam.calculate();
            screenChanged = false;
        }

        processInput(window);

        glClearColor(0.f, .3f, .3f, 1.f);
        glClearStencil(0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        auto lastTime = currentTime;
        currentTime = glfwGetTime();

        float deltaTime = currentTime - lastTime;

        // EXPLOSION

        if ((explodingTime > 0.0001f) || (v1.getPos().z < terrain.getZlvl(v1.getPos().x, v1.getPos().y))) {
            static float explTime = glfwGetTime();
            explodingTime = glfwGetTime() - explTime;
            v1.setExplode(explodingTime);
            v1_helix.setExplode(glfwGetTime() - explTime);
            v1_tail_left.setExplode(glfwGetTime() - explTime);
            v1_tail_right.setExplode(glfwGetTime() - explTime);
            v1_tail_top.setExplode(glfwGetTime() - explTime);
        }



        // TRANSITION 

        
        auto viewXY = glm::normalize(glm::vec3(v1.getNavDir().x, v1.getNavDir().y, 0.0f));

        if (gameState.camMode == FREE_CAMERA) {
            gameState.enabledCamera->translate(moveInDir());

        }
        if (gameState.camMode == PILOT) {
            glm::vec3 viewPos = v1.getPos() + glm::vec3(0,0.0f,3.0f) - viewXY * 4.0f;
            if (gameState.transition <= 0.0025f)
                initPos = gameState.enabledCamera->getPos();
            if (gameState.transition < 1.0f)
                gameState.enabledCamera->setPos(glm::mix(initPos, viewPos, gameState.transition));
            else {
                gameState.enabledCamera->setPos(viewPos);    
            }
        }
        if (gameState.camMode == FREE_CAMERA) {
            gameState.enabledCamera->setDir(gameState.direction);
            initDir = gameState.direction;
        }
        if (gameState.camMode == PILOT) {
            glm::vec3 playerDir = glm::normalize(glm::vec3(0,0.0f,-2.0f) + viewXY * 4.0f);
            gameState.enabledCamera->setDir(glm::mix(initDir, playerDir, gameState.transition));
            gameState.direction = playerDir;
        }
        
        if (gameState.transition < 1.0f) {
            gameState.transition += 0.0125f;
        }


        terrain.setCenter(gameState.enabledCamera->getPos().x, gameState.enabledCamera->getPos().y);

        skyBox.setPos(gameState.enabledCamera->getPos());
        skyBox.updateModel();

        if (gameState.camMode == FREE_CAMERA) {
            lightManager::setDir(0, gameState.direction);
            lightManager::setPos(0, gameState.enabledCamera->getPos());
            lightManager::updateShaders();
        }

        if (gameState.transition >= 1.0f)
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

        // passenger mode

        if (gameState.camMode == PASSENGER) {
            float xPos, yPos;
            float radius = 15.0f;
            xPos = radius * cos(glfwGetTime());
            yPos = radius * sin(glfwGetTime());
            float xDir = -sin(glfwGetTime());
            float yDir = cos(glfwGetTime());
            v1.setPos(glm::vec3(xPos, yPos, 15.0f));
            //v1.setCustomAxis(0,0, 1.0f);
            v1.setCustomPos(0, 0, 15.0f);
            //v1.customRotate(90 + glm::degrees(deltaTime));
            float cosAngle = glm::dot(glm::vec2(xDir, yDir), glm::vec2(0, 1.0f));
            //v1.rotateEabs(0,0, glm::degrees(360 - 2*glm::degrees(cosAngle)));
            v1.updateModel();
            v1.customRotate(glm::degrees(currentTime));
            v1_tail_top.customRotate(-25);

            gameState.direction = glm::vec3(xDir, yDir, -1.0f);

            if (gameState.passengerState == 0) {
                gameState.enabledCamera->setPos(glm::vec3(xPos - 5*xDir, yPos - 5*yDir, 20.0f));
                gameState.enabledCamera->setDir(glm::vec3(xDir, yDir, -1));
            }

            if (gameState.passengerState == 1) {
                gameState.enabledCamera->setPos(glm::vec3(10.0f,0,20.0f));
                gameState.enabledCamera->setDir(glm::vec3(-1.0f,0.0f,0.0f));
            }

            if (gameState.passengerState == 2) {}
        }


        gameState.enabledCamera->calculate();
        gameState.enabledCamera->updateShaders();
        
        gameState.motion.front = 0;
        gameState.motion.side = 0;
        gameState.motion.up = 0;
        gameState.motion.roll = 0;

        phongShader.use();
        phongShader.setUniform("baseFogDensity", gameState.fogDensity);
        phongShader.use(0);

        shader.use();
        shader.setUniform("baseFogDensity", gameState.fogDensity);
        shader.use(0);

        expS.use();
        expS.setUniform("explTime", explodingTime);
        expS.use(0);

        glDisable(GL_STENCIL_TEST);
        //glDisable(GL_DEPTH_TEST);
     
        cursor.draw();
        
        //glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);

        // SHROOMS

        int sIdx = 20;

        for (int x = -2; x <= 2; ++x) {
            for (int y = -2; y <= 2; ++y) {
                glStencilFunc(GL_ALWAYS, sIdx++, -1);
                float xPos = 5.0f + 10.0f * float(x);
                float yPos = 5.0f + 10.0f * float(y);
                sc.setPos(glm::vec3(xPos, yPos, terrain.getZlvl(xPos, yPos)));
                sc.updateModel();
                if (gameState.usedShrooms[sIdx-1] == false) 
                    sc.draw();
            }
        }


        glStencilFunc(GL_ALWAYS, 1, -1);
        skyBox.draw();
        glStencilFunc(GL_ALWAYS, 2, -1);
        terrain.draw();
        valec.draw();
        glStencilFunc(GL_ALWAYS, 3, -1);
        if (explodingTime < 1.0f)
            v1.draw();
        
        glEnable(GL_BLEND);
        if (explodingTime < 5.0f && explodingTime > 0.0f)
            exp.draw();
        glDisable(GL_BLEND);

        sc2.draw();

        renderLoop();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    delete buf;
    glfwTerminate();
    return 0;
}

// calculates translation vector
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

// keyboard input processign
void processInput(GLFWwindow* window) {
    static bool I_is_pressed = false;
    static bool M_is_pressed = false;
    static bool O_is_pressed = false;

    gameState.keyStates.Q = false;
    gameState.keyStates.E = false;
    gameState.keyStates.W = false;
    gameState.keyStates.S = false;
    gameState.keyStates.A = false;
    gameState.keyStates.D = false;

    static double prevTime = 0;
    double deltaTime = glfwGetTime() - prevTime;
    prevTime = glfwGetTime();

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)  {
        GLuint index;    
        if (M_is_pressed) {
        } else {
            M_is_pressed = true;
            glReadPixels(screenW/2, screenH/2, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);
            std::cout << "Stencil index: " << index << std::endl;

            gameState.usedShrooms[index] = true;

            if (index >= 20) {          
                gameState.fogDensity += 0.005f;
            }

        };
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        M_is_pressed = false;
    }
        
    

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        //gameState.enabledCamera->setPos(glm::vec3(0,0,0));
        //gameState.enabledCamera->setDir(glm::vec3(0,0,1.0));
        gameState.passengerState = 1;
    }


    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (O_is_pressed) {
        } else {
            O_is_pressed = true;
            togglePCamMode();
            //gameState.camMode = PASSENGER;    
        };
    } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
        O_is_pressed = false;
    }
    
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


// if window gets resized
void framebufferSizeCallback(GLFWwindow* window, GLint width, GLint height) {
    glViewport(0, 0, width, height);
    screenChanged = true;
    screenW = static_cast<GLuint>(width);
    screenH = static_cast<GLuint>(height);

}

// window initialization
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
    gameState.transition = 0.0f;
    if (gameState.camMode == PILOT)
        gameState.camMode = FREE_CAMERA;
    else if (gameState.camMode == FREE_CAMERA) {
        gameState.camMode = PILOT;
    }
}

void togglePCamMode() {
    gameState.transition = 0.0f;
    if (gameState.camMode == PASSENGER)
        gameState.camMode = FREE_CAMERA;
    else if (gameState.camMode == FREE_CAMERA) {
        gameState.camMode = PASSENGER;
    }
}