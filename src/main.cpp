// template based on material from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include "shader.h"
#include "objParser.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void loadShaders(unsigned int& VBO, unsigned int& VAO, unsigned int& EBO, std::vector<float>& vertices, std::vector<unsigned int>& indices) {

    unsigned int numVertices = vertices.size()/6;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() , vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(0));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // glew: load all OpenGL function pointers
    glewInit();

    //shaders
    bool zbuffer = false;

    if(zbuffer) {
        Shader ourShader("../shaders/shader.vs", "../shaders/zbuffer_shader.fs");
    }
    else {
        Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs");
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    parseFile("castle", vertices, indices);


    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    //glBindVertexArray(0); 

    unsigned int VBO, VAO, EBO;
    loadShaders(VBO, VAO, EBO, vertices, indices);


    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //enable z buffer
    glEnable(GL_DEPTH_TEST);

    //initialize position rotation and scale for object
    glm::vec3 position(0.0f);
    glm::vec3 rotation(0.0f);
    glm::vec3 scale(1.0f);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window, position, rotation, scale);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //create transformations
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);

        //create view matrix 
        glm::mat4 view = glm::mat4(1.0f);
        //view transformations 
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
        //create projection matrix  
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        //projection transformations 


        //get matrix location and set matrix 

        // draw our triangles
        ourShader.use();
        ourShader.setFloat("someUniform", 1.0f);
        
        //gpu side transformations
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        unsigned int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // unbind our VA no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
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
void processInput(GLFWwindow *window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
    //Translations
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        position.x -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        position.x += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        position.y -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        position.y += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        position.z -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        position.z += 0.01f;
    }

    //rotations
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
        rotation.x -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
        rotation.x += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        rotation.y -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        rotation.y += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
        rotation.z -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
        rotation.z += 0.1f;
    }

    //Scale
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        scale *= glm::vec3(1.01f);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        scale *= glm::vec3(0.99f);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

