#include "object.h"

object::object(GLuint nAttribs, GLuint nVerts, GLfloat* vertices, GLuint nIndcs, GLuint* indices, shaderProgram& shaderProg) : nIndices(nIndcs), nVertices(nVerts), shader(shaderProg) {
    std::cout << "Vertices loaded: " << nVertices << std::endl;

    type = TEXTURED;

    hasEBO = (indices==nullptr) ? false : true;

    if (hasEBO)
        std::cout << "EBO enabled!!!" << std::endl;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if (hasEBO)
        glGenBuffers(1, &EBO);

    fillBuffers(nAttribs, vertices, indices);
    
    /*glBufferData(GL_ARRAY_BUFFER, nAttribs, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */
}

object::object(GLuint nAttribs, GLfloat* vertices, GLuint nVerts, shaderProgram& shaderProg) : object(nAttribs, nVerts, vertices, 0, nullptr, shaderProg) {}

bool object::refillBuffers(GLuint nAttribs, GLfloat* buffer) {
    if (hasEBO) {
        std::cout << "Invalid operation!" << std::endl;
        return false;
    }
    std::cout << "refilling buffer" << std::endl;
    char a;
    return fillBuffers(nAttribs, buffer, nullptr);
}

bool object::fillBuffers(GLuint nAttribs, GLfloat* buffer, GLuint* indices) {
    if (type == UNDEFINED) {
        return false;
        std::cerr << "Error: undefined buffer type!" << std::endl;
    }

    glBindVertexArray(VAO);

    if (hasEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices*sizeof(GLfloat), indices, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, nAttribs*sizeof(GLfloat), buffer, GL_STATIC_DRAW);

    // vertices
    glVertexAttribPointer(VERT_LOC, 3, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(VERT_LOC);

    // normals
    glVertexAttribPointer(NORM_LOC, 3, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(NORM_LOC);

    // redundant but readable
    switch (type) {
        
        case (TEXTURED): {
            // texture coords
            glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
            glEnableVertexAttribArray(TEX_LOC);
        } break;

        case (COLORED): {
            // color
            glVertexAttribPointer(CLR_LOC, 3, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
            glEnableVertexAttribArray(CLR_LOC);
        } break;

        case (TEXTURED_COLORED): {
            // color
            glVertexAttribPointer(CLR_LOC, 3, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
            glEnableVertexAttribArray(CLR_LOC);

            // texture coords
            glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(9*sizeof(GLfloat)));
            glEnableVertexAttribArray(TEX_LOC);
        } break;
    }
    
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (hasEBO)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   

    return true;
}

void object::draw() {

    shader.use();
    shader.setUniform<glm::mat4>("model", model);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindVertexArray(VAO);
    
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
    
    if (!hasEBO)
        glDrawArrays(GL_TRIANGLES, 0, nVertices);
    else
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    shader.use(0);
}

void object::loadTexture(std::string name) {    
    
    if (!(type == TEXTURED || type == TEXTURED_COLORED)) {
        std::cerr << "Error: object type does not support textures!" << std::endl;
        return;
    }

    texParams();
    //stbi_set_flip_vertically_on_load(true);
    GLint width, height, nrChannels;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


	unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cerr << "Failed to load texture" << std::endl;
	}

    stbi_image_free(data);

    // trash????
    //shader.use();
    //shader.setUniform<int>("objTexture", int(0));
    //glUseProgram(0);
}

void object::texParams() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void object::updateModel() {
    glm::vec3 worldDir = glm::vec3(0.0f, 1.0f, 0.0f);
    model = glm::mat4(1.0f);
    glm::vec3 nDir = glm::normalize(direction);

    if (glm::dot(worldDir, nDir) != 1.0f) {

        glm::vec3 axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f),nDir);
        std::cout << "axis: " << axis.x << ' ' << axis.y << ' ' << axis.z << std::endl;
        float cosAngle = glm::dot(nDir, glm::vec3(0.0f, 1.0f, 0.0f));
        float angle = acos(cosAngle);
        std::cout << "angle: " << angle << std::endl;
        model = glm::rotate(model, angle, axis);

    }

    

    model = glm::translate(model, position);
}

void object::setDir(glm::vec3 dir) {
    direction = dir;
    updateModel();
}

void object::rotateQ(glm::quat q) {

}

void object::rotateErel(float xDeg, float yDeg, float zDeg) {
    model = glm::rotate(model, glm::radians(zDeg), glm::vec3(0.0f,0.0f,1.0f));
    model = glm::rotate(model, glm::radians(yDeg), glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, glm::radians(xDeg), glm::vec3(1.0f,0.0f,0.0f));
}

void object::rotateEabs(float xDeg, float yDeg, float zDeg) {
    model = glm::mat4(1.0f);
    rotateErel(xDeg, yDeg, zDeg);
}