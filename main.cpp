#include "Libraries/GLM/ext/matrix_clip_space.hpp"
#include "Libraries/GLM/ext/matrix_transform.hpp"
#include "Libraries/GLM/ext/quaternion_geometric.hpp"
#include "Libraries/GLM/ext/vector_float3.hpp"
#include "Libraries/GLM/geometric.hpp"
#include "Libraries/PL/ClassShader.h"
#include "Libraries/PL/ClassCamera.h"
#include "Libraries/GLAD/glad/KHR/khrplatform.h"
#include "Libraries/GLAD/glad/glad.h"
#include "Libraries/GLFW/include/GLFW/glfw3.h"
#include "Libraries/GLM/glm.hpp"
#include "Libraries/GLM/gtc/matrix_transform.hpp"
#include "Libraries/GLM/gtc/type_ptr.hpp"

#include "Libraries/GLM/gtx/string_cast.hpp"

#include <gl/gl.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void processInput(GLFWwindow *window);


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

//NEEDS WORK
float *CalculateNormals(float vertices[], int arraySize){

    int arrayLength = arraySize;
    std::cout << arrayLength << std::endl;
    float *result = new float[arrayLength];
    for(int i = 0; i < arrayLength; i += 9){
        
        glm::vec3 vecA = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
        glm::vec3 vecB = glm::vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        glm::vec3 vecC = glm::vec3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);
        
        //std::cout << glm::to_string(vecA) << " " << glm::to_string(vecB) << " " << glm::to_string(vecC) << std::endl;

        glm::vec3 edgeAB = vecB - vecA;
        glm::vec3 edgeAC = vecC - vecA;

        glm::vec3 crossResult = glm::normalize(glm::cross(edgeAB, edgeAC));
        std::cout << glm::to_string(crossResult) << std::endl;
        for(int j = 0; j < ((arrayLength * 3)/ 9); j += 3){
            result[j] = crossResult.x;
            result[j + 1] = crossResult.y;
            result[j + 2] = crossResult.z;

            //std::cout << crossResult.x << std::endl;
            //std::cout<< crossResult.y << std::endl;
            //std::cout << crossResult.z <<std::endl;

            std::cout << result[j] << " " << result[j + 1] << " " << result[j + 2] << std::endl;
            
        };
    };
    
    return result;
    
};
int main()
{
    // glfw: initialize and configure
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // build and compile our shader program
    Shader ourShader("Resources/Shaders/Shader.shader");
    Shader planeShader("Resources/Shaders/floor.shader");
    Shader LightingShader("Resources/Shaders/Lighting.shader");

    ShaderProgramSource source = Parse("Resources/Shaders/Shader.shader");
    
    ShaderProgramSource planeSource = Parse("Resources/Shaders/floor.shader");
    
    ShaderProgramSource lightingSource = Parse("Resources/Shaders/Lighting.shader");

   float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f
}; 
float verticesP[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,  
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,  

        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f,  
        0.5f, -0.5f,  0.5f,  
        0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,  
        0.5f,  0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
}; 

    float planeVertices[]{
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,

        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };

    glm::vec3 postions[]{
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(-1.0, 0.0, 0.0)
    };

   /* unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    }; */
    
//CUBE
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    //Position Attrib
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //Color Attrib
    glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 

//Uncolored cube
    
    unsigned int cVBO, cVAO;

    glGenVertexArrays(1, &cVAO);
    glGenBuffers(1, &cVBO);

    glBindVertexArray(cVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesP), verticesP, GL_DYNAMIC_DRAW);
    //std::cout << sizeof(verticesP) << std::endl;
    
    //normals
    int arrayLength = *(&verticesP + 1) - verticesP;

    unsigned int normCVBO;
    glGenBuffers(1, &normCVBO);

    glBindBuffer(GL_ARRAY_BUFFER, normCVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CalculateNormals(verticesP, arrayLength)), CalculateNormals(verticesP, arrayLength), GL_DYNAMIC_DRAW);
    //std::cout << sizeof(CalculateNormals(verticesP, arrayLength)) << std::endl; 
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, cVBO);

    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, normCVBO);    

    glVertexAttribPointer(1, 3,GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    

    glBindVertexArray(0);
 
//PLANE
    unsigned int pVAO, pVBO;

    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1, &pVBO);

    glBindVertexArray(pVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    

    /* for(int i = 0; i < arrayLength; i++){   
        std::cout << CalculateNormals(planeVertices,arrayLength)[i] << std::endl;
    }; */
    

    


    glEnable(GL_DEPTH_TEST);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  


        float xMove;
        float yMove;
        float zMove;
        
        xMove += .01;
        yMove += .01;
        zMove += .01;
        if(xMove  > 10000 || yMove > 10000 || zMove > 10000){
            xMove = 0;
            yMove = 0;
            zMove = 0;
        }

        // input
        processInput(window);

        // render
        glClearColor(0.59f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calls the shader program we set up outside of render loop
        
        ourShader.use();
    
        
        //3D
        //CUBE
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(sinf(xMove), cosf(yMove), 2 * sin(zMove)));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f) , glm::vec3(1.0f, 0.5f, 0.0f));

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();

        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        int projLoc = glGetUniformLocation(ourShader.ID, "proj");
        int viewLoc = glGetUniformLocation(ourShader.ID, "view");
 
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)); 
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        


        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        int lightCol = glGetUniformLocation(ourShader.ID, "lightColor");
        glUniform3fv(lightCol, 1, glm::value_ptr(lightColor));
        

        glBindVertexArray(VAO); 
        //
        for(int i = 0; i < 3; i++){
            glm::mat4 model = glm::mat4(1.0f);
            //model = glm::translate(model, postions[i] * glm::vec3(sinf(xMove), cosf(yMove), 2 * sin(zMove)));
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f) , postions[i]);
            model = glm::scale(model, glm::vec3(.75f, .75f, .75f));
            model = glm::translate(model, postions[i]);

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); 
     
            glDrawArrays(GL_TRIANGLES, 0 , 36);
        }; 
        //PLANE
        planeShader.use();

        glm::mat4 planeModel = glm::mat4(1.0f);
        planeModel = glm::rotate(planeModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        planeModel = glm::scale(planeModel, glm::vec3(10, 10, 10));

        glm::mat4 planeProj = glm::mat4(1.0f);
        planeProj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 planeView = glm::mat4(1.0f);
        planeView = camera.GetViewMatrix();


        int planeMLoc = glGetUniformLocation(planeShader.ID, "model");
        int planePLoc = glGetUniformLocation(planeShader.ID, "proj");
        int planeVLoc = glGetUniformLocation(planeShader.ID, "view");

        glUniformMatrix4fv(planeMLoc, 1 , GL_FALSE, glm::value_ptr(planeModel));
        glUniformMatrix4fv(planePLoc, 1 , GL_FALSE, glm::value_ptr(planeProj));
        glUniformMatrix4fv(planeVLoc, 1 , GL_FALSE, glm::value_ptr(planeView));
        

        glBindVertexArray(pVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //cube w no color data
        LightingShader.use();
        glm::mat4 cModel = glm::mat4(1.0f);
        cModel = glm::translate(cModel, glm::vec3(2.0f, 0.0f, 0.0f));

        glm::mat4 cProjection = glm::mat4(1.0f);
        cProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 cView = glm::mat4(1.0f);
        cView = camera.GetViewMatrix();

        int cmodelLoc = glGetUniformLocation(LightingShader.ID, "model");
        int cprojLoc = glGetUniformLocation(LightingShader.ID, "proj");
        int cviewLoc = glGetUniformLocation(LightingShader.ID, "view");
 
        glUniformMatrix4fv(cmodelLoc, 1, GL_FALSE, glm::value_ptr(cModel)); 
        glUniformMatrix4fv(cprojLoc, 1, GL_FALSE, glm::value_ptr(cProjection)); 
        glUniformMatrix4fv(cviewLoc, 1, GL_FALSE, glm::value_ptr(cView));






        glBindVertexArray(cVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
   

    // glfw: terminate, clearing all previously allocated GLFW resources.
    //ALWAYS CALL AT END
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// Checks if escape is pressed and sets window close flag to true if it is
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
