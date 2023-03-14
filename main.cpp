#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "stb_image.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow *window);

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
	"out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
	"	ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 fragColor;\n"
	"in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   fragColor = vec4(ourColor, 1.0);\n"
    "}\0";

const char *fragmentShader2Source = "#version 330 core\n"
    "out vec4 fragColor;\n"
	"in vec4 vertexColor;\n"
    "void main()\n"
    "{\n"
    "   fragColor = vec4(0.0, 0.0, 0.5, 1.0);\n"
    "}\0";


int main() {	

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "asdga", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	GLfloat vertices[] = {
     0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	1.0f, 1.0f,	// top right
     0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,	// bottom right
    -0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	0.0f, 0.0f,	// bottom left
    -0.5f,  0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	0.0f, 1.0f	// top left 
	//  0.5f,  0.5f, 0.0f,  // top right
	// -0.5f, -0.5f, 0.0f  // bottom left
	};

	GLfloat triangle2[] = {
	-0.5f,  0.5f, 1.0f,  // top left 
	 0.5f,  0.5f, 0.0f,  // top right
	-0.5f, -0.5f, 0.0f  // bottom left
	};

	GLfloat triangle[] = {
	 0.0f,  0.5f, 0.0f,  	1.0f, 0.0f, 0.0f,	// top left 
	 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	// bottom right
	-0.5f, -0.5f, 0.0f, 	0.0f, 0.0f, 1.0f	// bottom left
	};

	GLfloat texCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f
	};

	GLuint indices[] = {  // note that we start from 0!
    0, 1, 2,   // triangle
	0, 2, 3
	};  

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// // vertex shader
	// GLuint vertexShader;
	// vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// glCompileShader(vertexShader);

	// int  success;
	// char infoLog[512];
	// glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	// if(!success)
	// {
    // 	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    // 	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	// }

	// // fragment shader
	// GLuint fragmentShader;
	// fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// glCompileShader(fragmentShader);

	// glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	// if(!success)
	// {
    // 	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    // 	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	// }


	// // shader program
	// GLuint shaderProgram;
	// shaderProgram = glCreateProgram();

	// glAttachShader(shaderProgram, vertexShader);
	// glAttachShader(shaderProgram, fragmentShader);
	// glLinkProgram(shaderProgram);

	// glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	// if(!success) {
    // glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    // std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	// }

	// glDeleteShader(vertexShader);
	// glDeleteShader(fragmentShader);

	Shader ourShader("vertex.vert", "fragment.frag");

	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

	//GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

	// TEXTURES
	
	
	stbi_set_flip_vertically_on_load(true);

	GLint width, height, nrChannels;
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

	GLuint texture1;
	glGenTextures(1, &texture1);

	glBindTexture(GL_TEXTURE_2D, texture1);


	// parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	// TEXTURE 2
	
	GLuint texture2;
	glGenTextures(1, &texture2);

	glBindTexture(GL_TEXTURE_2D, texture2);

	// parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}


	stbi_image_free(data);

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	// render loop
	while(!glfwWindowShouldClose(window)) {
		// input
		processInput(window);

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//glUseProgram(shaderProgram);
		ourShader.use();

		// update color
		GLfloat timeValue = glfwGetTime();
		float mixValue = 0.5 + sin(2*timeValue) / 2.0f;
		ourShader.setFloat("mixVal", mixValue*0.5);
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		//ourShader.setFloat


		glBindVertexArray(VAO);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		
		// checks, calls and swapping
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;	
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
	
