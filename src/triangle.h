#pragma once
#include"config.h"
class TriangleMesh {
public:
TriangleMesh();
void draw();
~TriangleMesh();

private:
unsigned int  VAO, vertex_count,EBO;
vector<unsigned> VBO;
int eli[7]={0,1,2,2,1,3};
};
TriangleMesh::TriangleMesh() {
    std::vector<float> data = {
        -1.0f, -1.0f, 0.0f, //bottom left
        1.0f, -1.0f, 0.0f, //bottom right
        -1.0f,  1.0f, 0.0f ,
        1.f,1.f,.0f,
    };
    int colori[]={0,1,2,3};
    vertex_count = 6;
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

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eli), eli, GL_STATIC_DRAW);


}

void TriangleMesh::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, 0);
}

TriangleMesh::~TriangleMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, VBO.data());
}