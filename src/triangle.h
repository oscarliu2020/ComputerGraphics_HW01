#pragma once
#include"config.h"
class TriangleMesh {
public:
TriangleMesh();
void draw();
~TriangleMesh();

private:
unsigned int  VAO, vertex_count;
vector<unsigned> VBO;
};
TriangleMesh::TriangleMesh() {
    std::vector<float> data = {
        -0.5f, -0.5f,0,
         0.5f, -0.5f,0,
         0.0f,  0.5f,0,    };
    int colori[]={0,1,2};
    vertex_count = 3;
    VBO.resize(2);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(2, VBO.data());
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colori), colori, GL_STATIC_DRAW);
    glVertexAttribIPointer(1,1,GL_INT,sizeof(int),(void*)0);
    glEnableVertexAttribArray(1);
}

void TriangleMesh::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

TriangleMesh::~TriangleMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, VBO.data());
}