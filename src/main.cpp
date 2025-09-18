#include"config.h"
int main() {
    GLFWwindow* window;
    if(!glfwInit()) {
        std::cout<<"Failed to initialize GLFW"<<std::endl;
        return -1;
    }
    window=glfwCreateWindow(800,600,"HW1",NULL,NULL);
    glfwMakeContextCurrent(window);\
    gladLoadGL(glfwGetProcAddress);
     
    glClearColor(0.25f,0.5f,0.75f,1.0f); 
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);
            glVertex3f(-0.5f,-0.5f,0.0f);
            glColor3f(1.0f,0.0f,0.0f);
            glVertex3f(0.5f,-0.5f,0.0f);
            glColor3f(0.0f,1.0f,0.0f);
            glVertex3f(0.0f,0.5f,0.5f);
            glColor3f(0.0f,0.0f,1.0f);
        glEnd();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}