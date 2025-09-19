#include"config.h"

// Function to load OBJ file
bool loadOBJ(const std::string& filename, Mesh& mesh) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    std::vector<Vec3> temp_vertices;
    std::vector<Vec3> temp_normals;
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            // Vertex position
            Vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vn") {
            // Vertex normal
            Vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            // Face (triangle)
            std::string vertex1, vertex2, vertex3;
            iss >> vertex1 >> vertex2 >> vertex3;
            
            // Parse face indices (format: v/vt/vn or v//vn or just v)
            auto parseVertex = [&](const std::string& vertexStr) {
                std::istringstream viss(vertexStr);
                std::string index;
                
                // Get vertex index
                std::getline(viss, index, '/');
                int vertexIndex = std::stoi(index) - 1; // Convert to 0-based
                
                // Skip texture coordinate if present
                if (std::getline(viss, index, '/')) {
                    // Get normal index if present
                    if (std::getline(viss, index)) {
                        int normalIndex = std::stoi(index) - 1;
                        mesh.vertices.push_back(temp_vertices[vertexIndex]);
                        if (normalIndex < temp_normals.size()) {
                            mesh.normals.push_back(temp_normals[normalIndex]);
                        } else {
                            mesh.normals.push_back(Vec3(0, 1, 0)); // Default normal
                        }
                    } else {
                        mesh.vertices.push_back(temp_vertices[vertexIndex]);
                        mesh.normals.push_back(Vec3(0, 1, 0)); // Default normal
                    }
                } else {
                    mesh.vertices.push_back(temp_vertices[vertexIndex]);
                    mesh.normals.push_back(Vec3(0, 1, 0)); // Default normal
                }
                
                mesh.indices.push_back(mesh.vertices.size() - 1);
            };
            
            parseVertex(vertex1);
            parseVertex(vertex2);
            parseVertex(vertex3);
        }
    }
    
    // If no normals were provided, calculate them
    if (temp_normals.empty() && !mesh.vertices.empty()) {
        mesh.normals.clear();
        mesh.normals.resize(mesh.vertices.size(), Vec3(0, 0, 0));
        
        // Calculate face normals and accumulate to vertices
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            if (i + 2 < mesh.indices.size()) {
                Vec3 v1 = mesh.vertices[mesh.indices[i]];
                Vec3 v2 = mesh.vertices[mesh.indices[i + 1]];
                Vec3 v3 = mesh.vertices[mesh.indices[i + 2]];
                
                Vec3 normal = Vec3::cross(v2 - v1, v3 - v1).normalize();
                
                mesh.normals[mesh.indices[i]] = mesh.normals[mesh.indices[i]] + normal;
                mesh.normals[mesh.indices[i + 1]] = mesh.normals[mesh.indices[i + 1]] + normal;
                mesh.normals[mesh.indices[i + 2]] = mesh.normals[mesh.indices[i + 2]] + normal;
            }
        }
        
        // Normalize accumulated normals
        for (auto& normal : mesh.normals) {
            normal = normal.normalize();
        }
    }
    
    std::cout << "Loaded " << mesh.vertices.size() << " vertices and " 
              << mesh.indices.size() / 3 << " triangles" << std::endl;
    
    return true;
}

// Shader source code
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main() {
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

// Global variables for camera control
float cameraRadius = 3.0f;
float cameraAngle = 0.0f;
float cameraHeight = 0.0f;
bool firstMouse = true;
float lastX = 400.0f, lastY = 300.0f;

// Mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        cameraAngle += xoffset * 0.01f;
        cameraHeight += yoffset * 0.01f;
        cameraHeight = std::max(-1.5f, std::min(1.5f, cameraHeight));
    }
}

// Scroll callback for zoom
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraRadius -= yoffset * 0.1f;
    cameraRadius = std::max(1.0f, std::min(10.0f, cameraRadius));
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Mesh Viewer - HW1", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Load OpenGL functions
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Load mesh
    Mesh mesh;
    if (!loadOBJ("Dino.obj", mesh)) {
        std::cout << "Failed to load mesh!" << std::endl;
        return -1;
    }
    
    // Normalize mesh
    mesh.normalize();
    std::cout << "Mesh normalized. Bounds: (" << mesh.minBounds.x << ", " << mesh.minBounds.y << ", " << mesh.minBounds.z << ") to ("
              << mesh.maxBounds.x << ", " << mesh.maxBounds.y << ", " << mesh.maxBounds.z << ")" << std::endl;
    
    // Create shader
    Shader shader(vertexShaderSource, fragmentShaderSource);
    
    // Set up vertex data and buffers
    unsigned int VAO, VBO, NBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vec3), mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Vertex normals
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(Vec3), mesh.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
    glEnableVertexAttribArray(1);
    
    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    // Set background color
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader
        shader.use();
        
        // Set up matrices
        Mat4 model = Mat4::rotateY(glfwGetTime() * 0.5f); // Slow rotation
        
        Vec3 cameraPos(cameraRadius * cos(cameraAngle), cameraHeight, cameraRadius * sin(cameraAngle));
        Mat4 view = Mat4::lookAt(cameraPos, Vec3(0, 0, 0), Vec3(0, 1, 0));
        
        Mat4 projection = Mat4::perspective(45.0f * 3.14159f / 180.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        
        // Set uniforms
        shader.setMat4("model", model.data);
        shader.setMat4("view", view.data);
        shader.setMat4("projection", projection.data);
        
        // Lighting uniforms
        shader.setVec3("lightPos", 2.0f, 2.0f, 2.0f);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("objectColor", 0.8f, 0.6f, 0.4f);
        shader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
        
        // Draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
    }
    
    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
}