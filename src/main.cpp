#include"config.h"
#include"triangle.h"
const char* vertex_shader=R"(
#version 400 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec3 aColor;
out vec3 outColor;
void main()
{
    gl_Position=vec4(aPos,0,1.0);
    outColor=aColor;
}
)";
const char* fragment_shader=R"(
#version 400 core

in vec3 outColor;
out vec4 screenColor;

void main()
{
    screenColor = vec4(outColor, 1.0);
}
)";
unsigned compileShader(const char* shaderSource,int module_type) {
    int shader=glCreateShader(module_type);
    glShaderSource(shader,1,&shaderSource,NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader,512,NULL,infoLog);
        std::cout<<"ERROR::SHADER::COMPILATION_FAILED\n"<<infoLog<<std::endl;
    }
    return shader;
}
unsigned make_program(int vertexShader,int fragmentShader) {
    int shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    int success;
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
        std::cout<<"ERROR::SHADER::PROGRAM::LINKING_FAILED\n"<<infoLog<<std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
int main() {
    GLFWwindow* window;
    if(!glfwInit()) {
        std::cout<<"Failed to initialize GLFW"<<std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
    window=glfwCreateWindow(800,600,"HW1",NULL,NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    int vertexShader=compileShader(vertex_shader,GL_VERTEX_SHADER);
    int fragmentShader=compileShader(fragment_shader,GL_FRAGMENT_SHADER);
    int shaderProgram=make_program(vertexShader,fragmentShader);
    glClearColor(0,0,0,0); 
    auto triangle=TriangleMesh();
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        triangle.draw();
        glfwSwapBuffers(window);
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}