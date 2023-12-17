#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "camera.h"
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
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
float deltaTime = 0.0f;	
float lastFrame = 0.0f;


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void model_init()
{
    printf("read model init.\n");
    model.read_model();
}
unsigned int init_shader()
{
    printf("init shader.\n");
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNor;\n"
    "layout (location = 2) in vec3 aText;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec3 ourNor;\n"
    "out vec3 FragPos;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   FragPos = vec3(model * vec4(aPos, 1.0));\n"
    "   ourNor = mat3(transpose(inverse(model))) * aNor;\n"
    "   TexCoord = vec2(aText.x, aText.y);\n"
    "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourNor;\n"
    "in vec3 FragPos;\n"
    "in vec2 TexCoord;\n"
    "uniform vec3 viewPos;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "   vec3 lightColor = vec3(1.0f,1.0f,1.0f);\n"
    "   vec3 objectColor = vec3(texture(ourTexture, TexCoord));\n"
    "   float ambientStrength = 0.4;\n"
    "   vec3 ambient =  ambientStrength* lightColor;\n"
    "   vec3 lightPos = vec3(100.0f,100.0f,100.0f);\n"
    "   vec3 norm = normalize(ourNor)\n;"
    "   vec3 lightDir = normalize(lightPos - FragPos);\n"
    "   float diff = max(dot(norm, lightDir), 0.0);\n"
    "   vec3 diffuse = diff * lightColor;\n"
    "   vec3 viewDir = normalize(viewPos - FragPos);\n"
    "   float specularStrength = 1;\n"
    "   vec3 reflectDir = reflect(-lightDir, norm);\n"
    "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
    "   vec3 specular = specularStrength * spec * lightColor;\n"
    "   vec3 result = (ambient + diffuse + specular) * objectColor;\n"

    "   //FragColor = vec4(result, 1.0);\n"
    "   FragColor = texture(ourTexture, TexCoord);\n"
    "}\n\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

void projection_perform(glm::vec3 cameramove,unsigned int shaderProgram)
{
    glUseProgram(shaderProgram);
    glm::mat4 model         = glm::mat4(1.0f); 
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);
    model = glm::rotate(model,glm::radians(modelrotate),glm::vec3(0.0f,1.0f,0.0f));

    view = camera.GetViewMatrix();
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc  = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    unsigned int viewposLoc = glGetUniformLocation(shaderProgram, "viewPos");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewposLoc, 1, GL_FALSE, glm::value_ptr(cameramove));
}
void draw(unsigned int shaderProgram)
{
    for(int k = 0; k < model.m_obj.size(); k++)
    {
        if(k == 1)
        {
            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glm::mat4 model_move         = glm::mat4(1.0f); 
            model.m_obj[k]->sky_move(OBJ_Vector4d(0,-0.1,0,0));
            model_move = glm::translate(model_move, glm::vec3(0, model.m_obj[k]->m_pos.y, 0.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_move));
        }
        if(k == 4)
        {
            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glm::mat4 model_move = glm::mat4(1.0f); 
            model.m_obj[k]->move(OBJ_Vector4d(camera.Position.x,camera.Position.y,camera.Position.z,0));
            model_move = glm::translate(model_move, glm::vec3(model.m_obj[k]->m_pos.x, model.m_obj[k]->m_pos.y, model.m_obj[k]->m_pos.z));
            model_move = glm::rotate(model_move, glm::radians(-camera.Yaw), glm::vec3(0.0, 1.0, 0.0));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_move));
        }
        if(k == 5)
        {
            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glm::mat4 model_move = glm::mat4(1.0f); 
            ((Ball*)(model.m_obj[k]))->sprit();
            //model.m_obj[k]->move(OBJ_Vector4d(camera.Position.x,camera.Position.y,camera.Position.z,0));
            model_move = glm::translate(model_move, glm::vec3(model.m_obj[k]->m_pos.x, model.m_obj[k]->m_pos.y, model.m_obj[k]->m_pos.z));
        
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_move));
        }
        for(int i = 0; i <  model.m_obj[k]->m_f_muti.size(); i++)
        {
            string m_name = model.m_obj[k]->m_texture_name_string[i];
            //printf("%s\n",m_name.c_str());
            map<string, unsigned int>::iterator iter = model.m_obj[k]->Material_map_texture.find(m_name);  
            if(iter != model.m_obj[k]->Material_map_texture.end())  
            {
                glBindTexture(GL_TEXTURE_2D, iter->second);
            }
            glBindVertexArray(model.m_obj[k]->m_VAO[i]);
            glDrawArrays(GL_TRIANGLES, 0, model.m_obj[k]->m_v_vertices_size[i]);
        }
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glm::mat4 model_move         = glm::mat4(1.0f); 
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_move));
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
    unsigned int shaderProgram = init_shader();
    init_buffer(shaderProgram);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    model.show();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 cameramove  = camera.Position;
        projection_perform(cameramove, shaderProgram);
        draw(shaderProgram);
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
    
    glDeleteProgram(shaderProgram);
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