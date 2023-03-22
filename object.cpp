#include "object.h"

object::object(GLfloat* vertices, size_t nAttribs, GLuint nVerts, shaderProgram& shaderProg) : nVertices(nVerts), shader(shaderProg) {
    std::cout << "Vertices loaded: " << nVertices << std::endl;
    glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);

    type = TEXTURED_COLORED;
    
    fillBuffers(nAttribs, vertices);
    
    /*glBufferData(GL_ARRAY_BUFFER, nAttribs, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */
}


bool object::fillBuffers(size_t nAttribs, GLfloat* buffer) {
    if (type == UNDEFINED) {
        return false;
        std::cerr << "Error: undefined buffer type!" << std::endl;
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, nAttribs, buffer, GL_STATIC_DRAW);

    // vertices
    glVertexAttribPointer(VERT_LOC, 3, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(VERT_LOC);

    // normals
    glVertexAttribPointer(NORM_LOC, 3, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(NORM_LOC);

    // color
    glVertexAttribPointer(CLR_LOC, 3, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(CLR_LOC);

    // texture coords
    glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, type * sizeof(GLfloat), (void*)(9*sizeof(GLfloat)));
    glEnableVertexAttribArray(TEX_LOC);

    /*
    switch (type) {
        
        case (TEXTURED):
        
            glVertexAttribPointer(TEX_LOC, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(TEX_LOC);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
    }*/
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void object::draw() {

    shader.use();
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, nVertices);
    glBindVertexArray(0);
    glUseProgram(0);
}

void object::loadTexture(std::string name) {    
    /*
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */
    
    if (!(type == TEXTURED || type == TEXTURED_COLORED)) {
        std::cerr << "Error: object type does not support textures!" << std::endl;
        return;
    }

    texParams();
    stbi_set_flip_vertically_on_load(true);
    GLint width, height, nrChannels;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


	unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cerr << "Failed to load texture" << std::endl;
	}

    stbi_image_free(data);

    shader.use();
    shader.setUniform<int>("objTexture", int(0));
    glUseProgram(0);
}

void object::texParams() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}