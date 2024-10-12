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
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 

    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f   
};

unsigned int indices[] = {
    0, 1, 2,  2, 3, 0, 
    4, 5, 6,  6, 7, 4,  
    0, 1, 5,  5, 4, 0,  
    3, 2, 6,  6, 7, 3,  
    0, 3, 7,  7, 4, 0,  
    1, 2, 6,  6, 5, 1   
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


void createRotationMatrix(float angleX, float angleY, float* matrix) {
    float cosX = cos(angleX);
    float sinX = sin(angleX);
    float cosY = cos(angleY);
    float sinY = sin(angleY);

    matrix[0] = cosY;
    matrix[1] = 0.0f;
    matrix[2] = -sinY;
    matrix[3] = 0.0f;
    
    matrix[4] = sinX * sinY;
    matrix[5] = cosX;
    matrix[6] = sinX * cosY;
    matrix[7] = 0.0f;
    
    matrix[8] = cosX * sinY;
    matrix[9] = -sinX;
    matrix[10] = cosX * cosY;
    matrix[11] = 0.0f;
    
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL2: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    SDL_Window* window = SDL_CreateWindow("Spinning 3D Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL2: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        return -1;
    }

    GLenum glewError = glewInit();
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "OpenGL: %s\n", glewGetErrorString(glewError));
        return -1;
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Color
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
    float angleX = 0.0f, angleY = 0.0f;
    float frameCount = 0;
    float elapsedTime = 0.0f;
    Uint32 lastTime = SDL_GetTicks();
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angleX += 0.02f;
        angleY += 0.01005f;

        float rotationMatrix[16];
        createRotationMatrix(angleX, angleY, rotationMatrix);

        GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, rotationMatrix);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);

	frameCount++;
	Uint32 currentTime = SDL_GetTicks();
	elapsedTime += currentTime - lastTime;
	lastTime = currentTime;
	if (elapsedTime >= 3000.0f) [
		printf("%.2fFPS\n", frameCount / (elapsedTime / 1000.0f));
	    	frameCout = 0;
	    	elapsedTime = 0.0f;
    	}
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



