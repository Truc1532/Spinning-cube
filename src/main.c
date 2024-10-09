#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

const GLchar* vertexShaderSource = R"(
    #version 460 core
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;
    out vec3 ourColor;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

const GLchar* fragmentShaderSource = R"(
    #version 460 core
    in vec3 ourColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(ourColor, 1.0);
    }
)";

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
};

GLuint compileShader(GLenum type, const GLchar* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Shader Compilation Failed\n%s\n", infoLog);
    }

    return shader;
}

void createRotationMatrix(float angleX, float angleY, float angleZ, float* matrix) {
    float cosX = cos(angleX);
    float sinX = sin(angleX);
    float cosY = cos(angleY);
    float sinY = sin(angleY);
    float cosZ = cos(angleZ);
    float sinZ = sin(angleZ);

    matrix[0] = cosY * cosZ;
    matrix[1] = sinX * sinY * cosZ - cosX * sinZ;
    matrix[2] = cosX * sinY * cosZ + sinX * sinZ;
    matrix[3] = 0.0f;

    matrix[4] = cosY * sinZ;
    matrix[5] = sinX * sinY * sinZ + cosX * cosZ;
    matrix[6] = cosX * sinY * sinZ - sinX * cosZ;
    matrix[7] = 0.0f;

    matrix[8] = -sinY;
    matrix[9] = sinX * cosY;
    matrix[10] = cosX * cosY;
    matrix[11] = 0.0f;

    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL\n");
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Spinning Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Failed to create SDL window\n");
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        return -1;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);

    glEnable(GL_DEPTH_TEST);

    int running = 1;
    float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angleX += 0.01f;
        angleY += 0.01f;
        angleZ += 0.01f;

        float rotationMatrix[16];
        createRotationMatrix(angleX, angleY, angleZ, rotationMatrix);

        GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, rotationMatrix);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        SDL_GL_SwapWindow(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
