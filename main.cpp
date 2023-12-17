#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/camera.h"


#include "stb_image.h"

#include <iostream>
#include <fstream>
#include "obj.h"
#include "model.h"
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);


const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float modelrotate   = 0;
Model model;
Camera camera(glm::vec3(0.0f, 4.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// meshes
unsigned int planeVAO;
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void model_init()
{
    printf("read model init.\n");
    model.read_model();
}

unsigned int text(unsigned int shaderProgram,const char* name)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // 加载并生成纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load(name, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}
void init_buffer(unsigned int shaderProgram)
{
    printf("init buffer.\n");
    for(int k = 0; k < model.m_obj.size(); k++)
    {
        map<string, string>::iterator iter; 
        for(iter = model.m_obj[k]->Material_map_Kd.begin(); iter != model.m_obj[k]->Material_map_Kd.end(); iter++)  
        {
            unsigned int texture = text(shaderProgram,iter->second.c_str());
            model.m_obj[k]->Material_map_texture.insert(pair<string, unsigned int>(iter->first,texture));  
        }
        for(int i = 0; i <  model.m_obj[k]->m_f_muti.size(); i++)
        {
            unsigned int VBO = 0;
            unsigned int VAO = 0;
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);  
            glBufferData(GL_ARRAY_BUFFER, model.m_obj[k]->m_v_vertices_byte[i],model.m_obj[k]->m_v_vertices[i], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, 0); 
            glBindVertexArray(0); 
            model.m_obj[k]->m_VAO.push_back(VAO);
            model.m_obj[k]->m_VBO.push_back(VBO);
        }
    }
}
void draw(const Shader &shader)
{
    glm::mat4 model_move         = glm::mat4(1.0f); 
    for(int k = 0; k < model.m_obj.size(); k++)
    {
        if(k == 1)
        {
            continue;
        }
        if(k == 1 && 0)
        {
            model_move = glm::mat4(1.0f); 
            model.m_obj[k]->sky_move(OBJ_Vector4d(0,-0.1,0,0));
            model_move = glm::translate(model_move, glm::vec3(0, model.m_obj[k]->m_pos.y, 0.0f));
            shader.setMat4("model", model_move);
        }
        if(k == 4)
        {
            model_move = glm::mat4(1.0f); 
            model.m_obj[k]->move(OBJ_Vector4d(camera.Position.x,camera.Position.y,camera.Position.z,0));
            model_move = glm::translate(model_move, glm::vec3(model.m_obj[k]->m_pos.x, model.m_obj[k]->m_pos.y, model.m_obj[k]->m_pos.z));
            model_move = glm::rotate(model_move, glm::radians(-camera.Yaw), glm::vec3(0.0, 1.0, 0.0));
            shader.setMat4("model", model_move);
        }
        if(k == 5)
        {
            model_move = glm::mat4(1.0f); 
            ((Ball*)(model.m_obj[k]))->sprit();
            //model.m_obj[k]->move(OBJ_Vector4d(camera.Position.x,camera.Position.y,camera.Position.z,0));
            model_move = glm::translate(model_move, glm::vec3(model.m_obj[k]->m_pos.x, model.m_obj[k]->m_pos.y, model.m_obj[k]->m_pos.z));
            shader.setMat4("model", model_move);
        }
        
        

        for(int i = 0; i <  model.m_obj[k]->m_f_muti.size(); i++)
        {
            string m_name = model.m_obj[k]->m_texture_name_string[i];
            //printf("%s\n",m_name.c_str());
            map<string, unsigned int>::iterator iter = model.m_obj[k]->Material_map_texture.find(m_name);  
            if(iter != model.m_obj[k]->Material_map_texture.end())  
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, iter->second);
            }
            glBindVertexArray(model.m_obj[k]->m_VAO[i]);
            glDrawArrays(GL_TRIANGLES, 0, model.m_obj[k]->m_v_vertices_size[i]);
        }
        model_move = glm::mat4(1.0f);
        shader.setMat4("model", model_move);
    }
}
int main()
{
    
    int errorCode = 0;

    model_init();
    errorCode = glfwInit();
    if(!errorCode)
    {
        printf("glfw init fail\n");
        glfwTerminate();
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    glfwSetErrorCallback( error_callback );

     bool isFullScreen = true;
    int monitorCount;
    GLFWmonitor** pMonitor = isFullScreen ? glfwGetMonitors(&monitorCount) : NULL;
    //sssstd::cout << "Screen number is " << monitorCount << std::endl;
    int holographic_screen = -1;
    for(int i=0; i<monitorCount; i++){
        int screen_x, screen_y;
        const GLFWvidmode * mode = glfwGetVideoMode(pMonitor[i]);
        screen_x = mode->width;
        screen_y = mode->height;
        //std::cout << "Screen size is X = " << screen_x << ", Y = " << screen_y << std::endl;
        if(screen_x==1920 && screen_y==1080){
            holographic_screen = i;
        }
    }
    //std::cout << holographic_screen << std::endl;
    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Holographic projection", pMonitor[holographic_screen], NULL);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ModelShow", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    else{
        printf("success create GLFW window.\n");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);
    Shader shader("3.1.2.shadow_mapping.vs", "3.1.2.shadow_mapping.fs");
    Shader simpleDepthShader("3.1.2.shadow_mapping_depth.vs", "3.1.2.shadow_mapping_depth.fs");
    Shader debugDepthQuad("3.1.2.debug_quad.vs", "3.1.2.debug_quad_depth.fs");
    init_buffer(shader.ID);
    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };

    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // configure depth map FBO
    // -----------------------
    const unsigned int SHADOW_WIDTH = 10240, SHADOW_HEIGHT = 10240;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);

    glm::vec3 lightPos(0, 50.0, 50);

    //model.show();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //渲染阴影贴图
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = -1000, far_plane = 1000;
        lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glCullFace(GL_FRONT);
            draw(simpleDepthShader);
            glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //渲染真实场景
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        // set light uniforms
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        draw(shader);
        // render Depth map to quad for visual debugging
        // ---------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    for(int k = 0; k < model.m_obj.size(); k++)
    {
        for(int i = 0; i <  model.m_obj[k]->m_f_muti.size(); i++)
        {
            glDeleteVertexArrays(1, &model.m_obj[k]->m_VAO[i]);
        }
    }
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}
void processInput(GLFWwindow *window)
{
    deltaTime *= 10;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        modelrotate += 1;
        modelrotate = int(modelrotate) % 360;
    }
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        ((Ball*)model.m_obj[5])->init_sprit(camera.Position,camera.Yaw,camera.Pitch);
    }
}
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
    yoffset = 0;
    camera.ProcessMouseMovement(xoffset, yoffset);


    //camerazero += glm::vec3(xoffset, yoffset, -1.0f);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
