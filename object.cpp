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

    direction = glm::vec3(0, 1.0f, 0.0f);
    trackFwdDir = direction;
    
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
    shader.setUniform<GLfloat>("time", glfwGetTime());

    int mtlIndex = 0;

    shader.setUniform<glm::vec3>("ambientM", glm::vec3(material[mtlIndex++], material[mtlIndex++], material[mtlIndex++]));
    shader.setUniform<glm::vec3>("diffuseM", glm::vec3(material[mtlIndex++], material[mtlIndex++], material[mtlIndex++]));
    shader.setUniform<glm::vec3>("specularM", glm::vec3(material[mtlIndex++], material[mtlIndex++], material[mtlIndex++]));
    shader.setUniform<GLfloat>("shininessM", material[mtlIndex++]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindVertexArray(VAO);
    
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
    
    glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
    
    if (!hasEBO)
        glDrawArrays(GL_TRIANGLES, 0, nVertices);
    else
        glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    shader.use(0);
    
    for (object* kid : childObjects) {
        kid->draw();
    }
}

void object::loadTexture(std::string name, TEXTURE_TYPE texType, int texNo) {    
    
    if (!(type == TEXTURED || type == TEXTURED_COLORED)) {
        std::cerr << "Error: object type does not support textures!" << std::endl;
        return;
    }

    texParams();
    //stbi_set_flip_vertically_on_load(true);
    GLint width, height, nrChannels;

    glGenTextures(1, &(textures[texNo]));
    glBindTexture(GL_TEXTURE_2D, textures[texNo]);


	unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
        if (texType == RGB)
		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        if (texType == RGBA)
		    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cerr << "Failed to load texture" << std::endl;
	}

    stbi_image_free(data);

    shader.use();
    if (texNo == 0)
        shader.setUniform<int>("objTexture", 0);
    if (texNo == 1)
        shader.setUniform<int>("objTexture1", 1);
    shader.use(0);

    // trash????
    //shader.use();
    //shader.setUniform<int>("objTexture", int(0));
    //glUseProgram(0);
}

void object::texParams() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void object::scale(float x, float y, float z) {
    scaleVector = glm::vec3(x, y, z);
    
    for (object* kid : childObjects) {
        kid->scale(x, y, z);
    }
}

void object::relUpdateModel() {

    for (object* kid : childObjects) {
        kid->relUpdateModel();
    }
}

void object::updateModel() {
    glm::vec3 worldDir = glm::vec3(0.0f, 1.0f, 0.0f);
    model = glm::mat4(1.0f);
    glm::vec3 nDir = glm::normalize(trackFwdDir);

    if (glm::dot(worldDir, nDir) != 1.0f) {

        glm::vec3 axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f),nDir);
        std::cout << "axis: " << axis.x << ' ' << axis.y << ' ' << axis.z << std::endl;
        float cosAngle = glm::dot(nDir, glm::vec3(0.0f, 0.0f, 1.0f));
        float angle = acos(cosAngle);
        std::cout << "angle: " << angle << std::endl;
        model = glm::rotate(model, angle, axis);

    }

    model = glm::translate(model, position);
    model = glm::scale(model, scaleVector);

    for (object* kid : childObjects) {
        kid->updateModel();
    }
}

void object::setDir(glm::vec3 dir) {
    direction = dir;
    updateModel();

    for (object* kid : childObjects) {
        kid->setDir(dir);
        kid->updateModel();
    }
}

void object::rotateQ(glm::quat q) {

}

void object::rotateErel(float xDeg, float yDeg, float zDeg) {
    model = glm::rotate(model, glm::radians(zDeg), glm::vec3(0.0f,0.0f,1.0f));
    model = glm::rotate(model, glm::radians(yDeg), glm::vec3(0.0f,1.0f,0.0f));
    model = glm::rotate(model, glm::radians(xDeg), glm::vec3(1.0f,0.0f,0.0f));
    
    for (object* kid : childObjects) {
        kid->rotateErel(xDeg, yDeg, zDeg);
    }
}

void object::rotateEabs(float xDeg, float yDeg, float zDeg) {
    model = glm::mat4(1.0f);
    rotateErel(xDeg, yDeg, zDeg);
    
    for (object* kid : childObjects) {
        kid->rotateEabs(xDeg, yDeg, zDeg);
    }
}
    
void object::attachChild(object* kid) {
    childObjects.push_back(kid);
}
    
void object::setCustomAxis(float x, float y, float z) {
    customAxis = glm::vec3(x,y,z);
}

void object::setCustomPos(float x, float y, float z) {
    customPos = glm::vec3(x,y,z);
}

void object::customRotate(float deg) {
    //model = glm::mat4(1.0f);
    model = glm::translate(model, -customPos);
    model = glm::rotate(model, glm::radians(deg), customAxis);
    model = glm::translate(model, +customPos);
}

void object::pitch(float deg) {
    auto pitchAxis = glm::cross(navUpDir, navFwdDir);
    model = glm::rotate(model, glm::radians(deg), pitchAxis);

    glm::mat4 rotMatrix(1.0f);
    auto trackPitchAxis = glm::cross(trackUpDir, trackFwdDir);
    rotMatrix = glm::rotate(rotMatrix, glm::radians(deg), trackPitchAxis);

    trackUpDir = glm::vec3(rotMatrix * glm::vec4(trackUpDir, 1.0f));
    trackFwdDir = glm::vec3(rotMatrix * glm::vec4(trackFwdDir, 1.0f));
    

    for (object* kid : childObjects) {
        kid->model = model;//glm::rotate(model, deg, pitchAxis);
    }
    
}

void object::yaw(float deg) {
    auto yawAxis = navUpDir;
    model = glm::rotate(model, glm::radians(deg), yawAxis);
    
    glm::mat4 rotMatrix(1.0f);
    rotMatrix = glm::rotate(rotMatrix, glm::radians(deg), trackUpDir);

    trackFwdDir = glm::vec3(rotMatrix * glm::vec4(trackFwdDir, 1.0f));

    for (object* kid : childObjects) {
        kid->model = model;//glm::rotate(model, deg, pitchAxis);
    }
}

void object::roll(float deg) {
    auto rollAxis = navFwdDir;
    model = glm::rotate(model, glm::radians(deg), rollAxis);
    
    glm::mat4 rotMatrix(1.0f);
    rotMatrix = glm::rotate(rotMatrix, glm::radians(deg), trackFwdDir);

    trackUpDir = glm::vec3(rotMatrix * glm::vec4(trackUpDir, 1.0f));

    for (object* kid : childObjects) {
        kid->model = model;//glm::rotate(model, deg, pitchAxis);
    }
}

glm::vec3 object::getNavDir() {
    return trackFwdDir;
}

void object::setMtl(float* mtls) {
    for (int i = 0; i < 10; ++i) {
        material[i] = mtls[i];
    }
}