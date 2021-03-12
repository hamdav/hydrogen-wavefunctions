
/* OpenGL dependencies */
#include "../headers/glad.h"
#include <GLFW/glfw3.h>
#include "../headers/glm/glm.hpp"
#include "../headers/glm/gtc/matrix_transform.hpp"
#include "../headers/glm/gtc/type_ptr.hpp"

/* To render text */
#include <ft2build.h>
#include FT_FREETYPE_H

/* std dependencies */
#include <iostream>
#include <math.h>
#include <map>

/* Other project files */
#include "../headers/shader.h"
#include "../headers/plane.h"
#include "../headers/wavefunction.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Declare function at end of file (put in another file at a later date)
void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color, GLuint VAO, GLuint VBO);

// Who doesn't like global variables
// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

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

    // freetype2: Initialize and add font
    // ---------------------------------------
    FT_Library ft;

    if(FT_Init_FreeType(&ft)) {
      fprintf(stderr, "Could not init freetype library\n");
      return 1;
    }
    FT_Face face;
    if (FT_New_Face(ft, "fonts/cmunsi.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    // Set the font width and height, 
    // width=0 means font calculates it
    FT_Set_Pixel_Sizes(face, 0, 20);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    // Populate the characters map
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    Shader mainShader("./shaders/vShader1.vert", "./shaders/fShader1.frag");
    Shader textShader("./shaders/vShaderText.vert", "./shaders/fShaderText.frag");

    textShader.use();
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // Create the axis coordinates
    float axis_vertices[] = {
         0.0f,  0.0f, 0.0f,  1.0, 1.0f, 1.0f, // origin
         0.2f,  0.0f, 0.0f,  1.0, 0.0f, 0.0f, // x
         0.0f,  0.2f, 0.0f,  0.0, 1.0f, 0.0f, // y
         0.0f,  0.0f, 0.2f,  0.0, 0.0f, 1.0f, // z
    };
    unsigned int axis_indices[] = {  // note that we start from 0!
        0, 1, 0,  // x
        0, 2, 0,  // y
        0, 3, 0,  // z
    };

    unsigned int aVBO, aVAO, aEBO;
    glGenVertexArrays(1, &aVAO);
    glGenBuffers(1, &aVBO);
    glGenBuffers(1, &aEBO);

    glBindVertexArray(aVAO);

    glBindBuffer(GL_ARRAY_BUFFER, aVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(axis_indices), axis_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create indicator plane coordinates
    //               G          
    //     F ------------------ H
    //       |       |        |
    //       |       y        |
    //     E |       Lx_______| A
    //       |                |
    //       |                |
    //     D ------------------ B
    //               C
    float indicator_plane_vertices[] = {
         0.0f,  0.0f, 0.0f,  1.0, 1.0f, 1.0f, // origin
         0.2f,  0.0f, 0.0f,  1.0, 0.0f, 0.0f, // A
         0.2f, -0.2f, 0.0f,  1.0, 1.0f, 1.0f, // B
         0.0f, -0.2f, 0.0f,  1.0, 1.0f, 1.0f, // C
        -0.2f, -0.2f, 0.0f,  1.0, 1.0f, 1.0f, // D
        -0.2f,  0.0f, 0.0f,  1.0, 1.0f, 1.0f, // E
        -0.2f,  0.2f, 0.0f,  1.0, 1.0f, 1.0f, // F
         0.0f,  0.2f, 0.0f,  0.0, 1.0f, 0.0f, // G
         0.2f,  0.2f, 0.0f,  1.0, 1.0f, 1.0f, // H
    };
    unsigned int indicator_plane_indices[] = {  // note that we start from 0!
        0, 1, 2, //OAB
        0, 2, 3, //OBC
        0, 3, 4, //OCD
        0, 4, 5, //ODE
        0, 5, 6, //OEF
        0, 6, 7, //OFG
        0, 7, 8, //OGH
        0, 8, 1, //OHA
    };

    unsigned int ipVBO, ipVAO, ipEBO;
    glGenVertexArrays(1, &ipVAO);
    glGenBuffers(1, &ipVBO);
    glGenBuffers(1, &ipEBO);

    glBindVertexArray(ipVAO);

    glBindBuffer(GL_ARRAY_BUFFER, ipVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indicator_plane_vertices), indicator_plane_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ipEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicator_plane_indices), indicator_plane_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);


    // Create the plane with the wavefunction image
    Plane plane1(0.7, 0.7, 150, 150);
    float* plane_vertices = plane1.getVertices();
    unsigned int* plane_indices = plane1.getIndices();

    unsigned int pVBO, pVAO, pEBO;
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1, &pVBO);
    glGenBuffers(1, &pEBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(pVAO);

    std::cout << "Size of vertices: "<< plane1.verticesSize() << "\nSize of indices: " << plane1.indicesSize() << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, pVBO);
    glBufferData(GL_ARRAY_BUFFER, plane1.verticesSize(), plane_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane1.indicesSize(), plane_indices, GL_STATIC_DRAW);

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


    // VAOSTUFF for textrendering
    unsigned int textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    float timeValue, greenValue;
    int vertexColorLocation;

    glm::mat4 trans(1.0f);
    glm::mat4 axesTrans(1.0f); // Trans(late) or Trans(form)??? Both it seems???
    axesTrans = glm::scale(axesTrans, glm::vec3(0.5, 0.5, 0.5));
    axesTrans = glm::translate(axesTrans, glm::vec3(1.2, 0.0, 0.0));
    axesTrans = glm::rotate(axesTrans, 0.3f, glm::vec3(1, 0, 0));
    axesTrans = glm::rotate(axesTrans, -0.3f, glm::vec3(0, 1, 0));
    axesTrans = glm::rotate(axesTrans, -2.094f, glm::vec3(1, 1, 1));
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

    std::string nmltext{"n=1, l=0, m=0"};
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
        nmltext = "n=" + std::to_string(plane1.getn()) + ", l=" + std::to_string(plane1.getl()) + ", m=" + std::to_string(plane1.getm());
        plane1.updateColors(theta,phi);
        plane_vertices = plane1.getVertices();
        glBindBuffer(GL_ARRAY_BUFFER, pVBO);
        glBufferData(GL_ARRAY_BUFFER, plane1.verticesSize(), plane_vertices, GL_STATIC_DRAW);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        // draw our first triangle
        mainShader.use();

        timeValue = glfwGetTime();
        greenValue = (sin(timeValue));
        mainShader.setFloat("alpha", 1.0);
        mainShader.setMat4("transform", trans);


        glBindVertexArray(pVAO); 
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, plane1.indicesSize() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        glBindVertexArray(ipVAO); 
        tmpTrans = glm::rotate(axesTrans, (float) theta, glm::vec3(0.0, 0.0, 1.0));
        tmpTrans = glm::rotate(tmpTrans, (float) -phi, glm::vec3(0.0, 1.0, 0.0));
        mainShader.setMat4("transform", tmpTrans);
        mainShader.setFloat("alpha", 1);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

        glBindVertexArray(aVAO); 
        mainShader.setMat4("transform", axesTrans);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        
        // Should depth testing be disabled? I donno but it seems to work
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        RenderText(textShader, nmltext, static_cast<float>(SCR_WIDTH)/2, 360.0f, 0.5f, glm::vec3(1.0, 1.0f, 1.0f), textVAO, textVBO);
        glEnable(GL_DEPTH_TEST);


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
    glDeleteVertexArrays(1, &pVAO);
    glDeleteBuffers(1, &pVBO);
    glDeleteBuffers(1, &pEBO);

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


void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color, GLuint VAO, GLuint VBO)
{
    // activate corresponding render state
    s.use();
    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
