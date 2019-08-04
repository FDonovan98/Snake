#include <iostream>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <shaders.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include <ctime>

using namespace std;

enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

//initialise globals for movement

//initialise segment positions
vector<glm::vec2> segmentPos;

direction snakeDirection = UP;

bool gameStart = false;
bool gameOver = false;

float deltaTime = 0.0f, lastFrame = 0.0f;

bool xMovement[] = {
    true,
    true,
    true,
    true
};



bool appleEaten = false;

glm::vec2 applePos;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width >= height) {
        glViewport((width - height) / 2, 0, height, height);
    } else {
        glViewport(0, (height - width) / 2, width, width);
    }
    
}

void createApple() {
    bool goodApple;
    appleEaten = false;

    do {
        applePos = glm::vec2(rand() % 18 - 9, rand() % 18 - 9);
        goodApple = true;
        for (unsigned int i = 0; i < segmentPos.size(); i++) {
            if (goodApple) {
                if (segmentPos[i] == applePos) {
                    goodApple = false;
                }
            }
        }
    } while (!goodApple);
}

void restart() {
    segmentPos.clear();
    segmentPos.push_back(glm::vec2(0.0f, 0.0f));
    segmentPos.push_back(glm::vec2(0.0f, -1.0f));
    segmentPos.push_back(glm::vec2(0.0f, -2.0f));

    createApple();

    gameOver = false;
    gameStart = false;
    snakeDirection = UP;
}

void processKeyboard(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        snakeDirection = UP;
        xMovement[0] = false;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        snakeDirection = DOWN;
        xMovement[0] = false;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        snakeDirection = LEFT;
        xMovement[0] = true;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        snakeDirection = RIGHT;
        xMovement[0] = true;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        gameStart = true;
    }
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (gameOver) {
            restart();
        }
    }
    // if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    //     appleEaten = true;
    // }
}

void moveSnake() {
    float xDiff, yDiff, snakeSpeed = 1.0f;
    glm::vec2 tail = segmentPos[segmentPos.size() - 1];

    for (unsigned int i = segmentPos.size() - 1; i > 0; i--) {
        xDiff = segmentPos[i].x - segmentPos[i-1].x;
        yDiff = segmentPos[i].y - segmentPos[i-1].y;

        if (segmentPos[i].x == segmentPos[i-1].x) {
            xMovement[i] = false;
        } else {
            xMovement[i] = true;
        }

        if (xMovement[i]) {
            if (xDiff > 0) {
                segmentPos[i].x -= snakeSpeed;
            } else {
                segmentPos[i].x += snakeSpeed;
            }
            
        } else {
            if (yDiff > 0) {
                segmentPos[i].y -= snakeSpeed;
            } else {
                segmentPos[i].y += snakeSpeed;
            }
            
        }
        
    }

    if (snakeDirection == UP) {
        segmentPos[0].y += snakeSpeed;
    } else if (snakeDirection == DOWN) {
        segmentPos[0].y -= snakeSpeed;
    } else if (snakeDirection == LEFT) {
        segmentPos[0].x -= snakeSpeed;
    } else {
        segmentPos[0].x += snakeSpeed;
    }

    if (appleEaten) {
        createApple();
        segmentPos.push_back(tail);
    }

}

void collisionCheck() {
    float border = 9.0f;

    if ((abs(segmentPos[0].x) > border) || (abs(segmentPos[0].y) > border)) {
        gameOver = true;
    }

    for (unsigned int i = 1; i < segmentPos.size(); i++) {
        if (segmentPos[0] == segmentPos[i]) {
            gameOver = true;
        }
    }

    if (segmentPos[0] == applePos) {
        appleEaten = true;
    }   
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    //Initialise glfw and glad
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Snake", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    srand(time(NULL));
    
    // set up segment dimensions
    float segment[] = {
        //position    
         0.5f,  0.5f, //top right
         0.5f, -0.5f, //bottom right
        -0.5f, -0.5f, //bottom left 
        -0.5f,  0.5f, //top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    //set starting segment positions
    restart();

    //set wall positions
    glm::vec2 wallPos[] = {
        glm::vec2(  0.0f,  10.0f),
        glm::vec2( 10.0f,   0.0f),
        glm::vec2(  0.0f, -10.0f),
        glm::vec2(-10.0f,   0.0f),
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(segment), segment, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    Shader snakeShader("Dependencies/Shaders/snakeShader.vs", "Dependencies/Shaders/snakeShader.fs");

    snakeShader.use();

    glm::vec3 color;
    float delay = 0.2f;
    


    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //reder loop

    while(!glfwWindowShouldClose(window)) {

        processKeyboard(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //render walls
        for (unsigned int i = 0; i < 4; i++) {

            glm::mat4 scale = glm::mat4(1.0f);
            scale = glm::scale(scale, glm::vec3(0.1f, 0.1f, 0.0f));            

            if (i % 2 == 0) {
                scale = glm::scale(scale, glm::vec3(20.0f, 1.0f, 0.0f));
            } else {
                scale = glm::scale(scale, glm::vec3(1.0f, 20.0f, 0.0f));
            }

            snakeShader.setMat4("scale", scale);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(wallPos[i], 0.0f));

            snakeShader.setMat4("model", model);

            color = glm::vec3(0.0f, 1.0f, 0.0f);
            snakeShader.setVec3("color", color);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;

        if (!gameOver) {

            glm::mat4 scale = glm::mat4(1.0f);
            scale = glm::scale(scale, glm::vec3(0.1f, 0.1f, 0.0f));
            snakeShader.setMat4("scale", scale);

            //render snake
            for (unsigned int i=0; i < (segmentPos.size()); i++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(segmentPos[i], 0.0f));
                snakeShader.setMat4("model", model);

                color = glm::vec3(1.0f, 1.0f, 1.0f);
                snakeShader.setVec3("color", color);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

            //reder apples
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(applePos, 0.0f));
            snakeShader.setMat4("model", model);

            color = glm::vec3(1.0f, 0.0f, 0.0f);
            snakeShader.setVec3("color", color);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            if ((gameStart) && (deltaTime > delay)) {
                lastFrame = currentTime;
                delay = 0.2f / (((float)segmentPos.size() + 2.0f) / 5.0f);
                moveSnake();
                collisionCheck();
            }

        } else {
            lastFrame = currentTime;
            int score = segmentPos.size() - 3;
            int rows = ceilf((float)score / 9);
            glm::vec2 scorePos[score];

            int scoreCounter = -1;

            for (unsigned int rowCount = 0; rowCount < rows; rowCount++) {             
                unsigned int columnEnd;
                columnEnd = score - 9 * rowCount;

                if (columnEnd > 9) {
                    columnEnd = 9;
                }

                for (unsigned int columnCount = 0; columnCount < columnEnd; columnCount++) {    
                    scoreCounter++;
                    scorePos[scoreCounter] = glm::vec2(2 * (float)columnCount - 8.0f, 8.0f - 2 * (float)rowCount );                    
                }
            } 

            glm::mat4 scale = glm::mat4(1.0f);
            scale = glm::scale(scale, glm::vec3(0.1f, 0.1f, 0.0f));
            snakeShader.setMat4("scale", scale);

            color = glm::vec3(1.0f, 0.0f, 0.0f);
            snakeShader.setVec3("color", color);

            for (unsigned int i = 0; i < score; i++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(scorePos[i], 0.0f));
                snakeShader.setMat4("model", model);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
                
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}