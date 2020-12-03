
/* OpenGL dependencies */
#include "../headers/glad.h"
#include <GLFW/glfw3.h>
#include "../headers/glm/glm.hpp"
#include "../headers/glm/gtc/matrix_transform.hpp"
#include "../headers/glm/gtc/type_ptr.hpp"

/* std dependencies */
#include <iostream>
#include <math.h>

/* Other project files */
#include "../headers/shader.h"
#include "../headers/plane.h"
#include "../headers/wavefunction.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    Shader mainShader("./shaders/vShader1.vert", "./shaders/fShader1.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices1[] = {
         0.0f,  0.0f, 0.0f,  1.0, 1.0f, 1.0f, // origin
         0.5f,  0.0f, 0.0f,  1.0, 0.0f, 0.0f, // x
         0.0f,  0.5f, 0.0f,  0.0, 1.0f, 0.0f, // y
         0.0f,  0.0f, 0.5f,  0.0, 0.0f, 1.0f, // z
    };
    unsigned int indices1[] = {  // note that we start from 0!
        0, 1, 2,  // x y
        0, 1, 3,  // x z
        0, 2, 3,  // y z
        1, 2, 3,
    };

    unsigned int tVBO, tVAO, tEBO;
    glGenVertexArrays(1, &tVAO);
    glGenBuffers(1, &tVBO);
    glGenBuffers(1, &tEBO);

    glBindVertexArray(tVAO);

    glBindBuffer(GL_ARRAY_BUFFER, tVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    Plane plane1(0.7, 0.7, 150, 150);
    float* vertices = plane1.getVertices();
    unsigned int* indices = plane1.getIndices();

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    std::cout << "Size of vertices: "<< plane1.verticesSize() << "\nSize of indices: " << plane1.indicesSize() << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, plane1.verticesSize(), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane1.indicesSize(), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    // uncomment this call to draw in wireframe polygons.

    float timeValue, greenValue;
    int vertexColorLocation;

    glm::mat4 trans(1.0f);
    glm::mat4 axesTrans(1.0f);
    axesTrans = glm::scale(axesTrans, glm::vec3(0.5, 0.5, 0.5));
    glm::mat4 tmpTrans;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // render loop
    // -----------

    // ms counting
    double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    int frameDiff = 0;

    glfwSwapInterval(0);

    double theta = 0;
    double phi = 0;
    bool nWasPressed = false;
    bool lWasPressed = false;
    bool mWasPressed = false;
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            phi += 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            phi -= 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            theta += 0.01;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            theta -= 0.01;
        }
        // Increment n/l/m when n/l/m is pressed
        
        if (!nWasPressed && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
                plane1.decrement_n();
            else
                plane1.increment_n();
            nWasPressed = true;
        }
        if (nWasPressed && glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) {
            nWasPressed = false;
        }

        if (!lWasPressed && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
                plane1.decrement_l();
            else
                plane1.increment_l();
            lWasPressed = true;
        }
        if (lWasPressed && glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
            lWasPressed = false;
        }
        if (!mWasPressed && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
                plane1.decrement_m();
            else
                plane1.increment_m();
            mWasPressed = true;
        }
        if (mWasPressed && glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
            mWasPressed = false;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            plane1.zoomIn();
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            plane1.zoomOut();
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            plane1.incSensitivity();
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            plane1.decSensitivity();
        }


        // update state
        plane1.updateColors(theta,phi);
        vertices = plane1.getVertices();
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, plane1.verticesSize(), vertices, GL_STATIC_DRAW);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        mainShader.use();

        timeValue = glfwGetTime();
        greenValue = (sin(timeValue));
        //mainShader.setFloat("alpha", 1.0);
        mainShader.setMat4("transform", trans);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, plane1.indicesSize() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        glBindVertexArray(tVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        tmpTrans = glm::translate(axesTrans, glm::vec3(1.2, 0.0, 0.0));
        tmpTrans = glm::rotate(tmpTrans, (float) phi, glm::vec3(1.0, 0.0, 0.0));
        tmpTrans = glm::rotate(tmpTrans, (float) -theta, glm::vec3(0.0, 0.0, 1.0));
        mainShader.setMat4("transform", tmpTrans);
        mainShader.setFloat("alpha", 1);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        currentTime = glfwGetTime();
        frameDiff++;
        if ( currentTime - lastTime >= 1.0 ) {
            printf("%f ms\n", 1000.0/((double)(frameDiff)));
            frameDiff = 0;
            lastTime += 1.0;
        }
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(-500, -500, 2000, 2000);
}
