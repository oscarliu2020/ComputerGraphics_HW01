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


struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    
    float length() const { return std::sqrt(x*x + y*y + z*z); }
    Vec3 normalize() const { float len = length(); return len > 0 ? *this / len : Vec3(); }
    
    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
};

// Mesh structure
struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<unsigned int> indices;
    
    // Bounding box
    Vec3 minBounds, maxBounds;
    
    void calculateBounds() {
        if (vertices.empty()) return;
        
        minBounds = maxBounds = vertices[0];
        for (const auto& vertex : vertices) {
            minBounds.x = std::min(minBounds.x, vertex.x);
            minBounds.y = std::min(minBounds.y, vertex.y);
            minBounds.z = std::min(minBounds.z, vertex.z);
            maxBounds.x = std::max(maxBounds.x, vertex.x);
            maxBounds.y = std::max(maxBounds.y, vertex.y);
            maxBounds.z = std::max(maxBounds.z, vertex.z);
        }
    }
    
    void normalize() {
        calculateBounds();
        
        // Find center and size
        Vec3 center = (minBounds + maxBounds) * 0.5f;
        Vec3 size = maxBounds - minBounds;
        float maxDimension = std::max({size.x, size.y, size.z});
        
        if (maxDimension > 0) {
            float scale = 1.0f / maxDimension;
            
            // Normalize all vertices
            for (auto& vertex : vertices) {
                vertex = (vertex - center) * scale;
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

// Matrix utilities (simplified 4x4 matrix)
struct Mat4 {
    float data[16];
    
    Mat4() {
        // Identity matrix
        for (int i = 0; i < 16; i++) data[i] = 0.0f;
        data[0] = data[5] = data[10] = data[15] = 1.0f;
    }
    
    static Mat4 perspective(float fovy, float aspect, float near, float far) {
        Mat4 result;
        for (int i = 0; i < 16; i++) result.data[i] = 0.0f;
        float tanHalfFovy = tan(fovy / 2.0f);
        
        result.data[0] = 1.0f / (aspect * tanHalfFovy);
        result.data[5] = 1.0f / tanHalfFovy;
        result.data[10] = -(far + near) / (far - near);
        result.data[11] = -1.0f;
        result.data[14] = -(2.0f * far * near) / (far - near);
        result.data[15] = 0.0f;
        
        return result;
    }
    
    static Mat4 lookAt(Vec3 eye, Vec3 center, Vec3 up) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = Vec3::cross(f, up).normalize();
        Vec3 u = Vec3::cross(s, f);
        
        Mat4 result;
        for (int i = 0; i < 16; i++) result.data[i] = 0.0f;
        result.data[0] = s.x; result.data[4] = s.y; result.data[8] = s.z;
        result.data[1] = u.x; result.data[5] = u.y; result.data[9] = u.z;
        result.data[2] = -f.x; result.data[6] = -f.y; result.data[10] = -f.z;
        result.data[12] = -(s.x * eye.x + s.y * eye.y + s.z * eye.z);
        result.data[13] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
        result.data[14] = -(-f.x * eye.x - f.y * eye.y - f.z * eye.z);
        result.data[15] = 1.0f;
        
        return result;
    }
    
    static Mat4 rotateY(float angle) {
        Mat4 result;
        float c = cos(angle);
        float s = sin(angle);
        
        result.data[0] = c;
        result.data[2] = s;
        result.data[8] = -s;
        result.data[10] = c;
        
        return result;
    }
    
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 16; i++) result.data[i] = 0.0f;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++) {
                    result.data[i * 4 + j] += data[i * 4 + k] * other.data[k * 4 + j];
                }
            }
        }
        return result;
    }
};