#pragma once

#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<algorithm>
#include<cmath>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include<GLFW/glfw3.h>
#include<linmath.h>

// Wrapper for vec3 to work with std::vector
struct Vec3Wrapper {
    vec3 data;
    
    Vec3Wrapper() { data[0] = data[1] = data[2] = 0.0f; }
    Vec3Wrapper(float x, float y, float z) { data[0] = x; data[1] = y; data[2] = z; }
    Vec3Wrapper(const vec3& v) { vec3_dup(data, v); }
    
    operator vec3&() { return data; }
    operator const vec3&() const { return data; }
    
    float& operator[](int i) { return data[i]; }
    const float& operator[](int i) const { return data[i]; }
};



// Mesh structure
struct Mesh {
    std::vector<Vec3Wrapper> vertices;
    std::vector<Vec3Wrapper> normals;
    std::vector<unsigned int> indices;
    
    // Bounding box
    Vec3Wrapper minBounds, maxBounds;
    
    void calculateBounds() {
        if (vertices.empty()) return;
        
        minBounds = maxBounds = vertices[0];
        
        for (const auto& vertex : vertices) {
            minBounds[0] = std::min(minBounds[0], vertex[0]);
            minBounds[1] = std::min(minBounds[1], vertex[1]);
            minBounds[2] = std::min(minBounds[2], vertex[2]);
            maxBounds[0] = std::max(maxBounds[0], vertex[0]);
            maxBounds[1] = std::max(maxBounds[1], vertex[1]);
            maxBounds[2] = std::max(maxBounds[2], vertex[2]);
        }
    }
    
    void normalize() {
        calculateBounds();
        
        // Find center and size
        vec3 center, size;
        vec3_add(center, minBounds.data, maxBounds.data);
        vec3_scale(center, center, 0.5f);
        vec3_sub(size, maxBounds.data, minBounds.data);
        
        float maxDimension = std::max({size[0], size[1], size[2]});
        
        if (maxDimension > 0) {
            float scale = 1.0f / maxDimension;
            
            // Normalize all vertices
            for (auto& vertex : vertices) {
                vec3 temp;
                vec3_sub(temp, vertex.data, center);
                vec3_scale(vertex.data, temp, scale);
            }
            
            // Recalculate bounds
            calculateBounds();
        }
    }
};

// Shader utilities
class Shader {
public:
    unsigned int ID;
    
    Shader(const char* vertexSource, const char* fragmentSource) {
        // Vertex shader
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSource, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        // Fragment shader
        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSource, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        // Shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        
        // Clean up
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void use() { glUseProgram(ID); }
    
    void setMat4(const std::string& name, const float* value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
    }
    
    void setVec3(const std::string& name, float x, float y, float z) {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    
private:
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
};

