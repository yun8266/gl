#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bits/stdc++.h"
#include "stb_image.h"
#include "shader.h"

using namespace std;
unsigned delta;
float lastX = 400, lastY = 300;
float yaw=-90.0f,pitch=0.0f;
float fov=45.0f;
bool type=false;
class Camera {
    public:
    glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 front = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 up=glm::vec3(0.0f,1.0f,0.0f);
    glm::mat4 lookAt;
    Camera()=default;
    void update(glm::vec3 pos,glm::vec3 front){
        this->pos =pos;
        this->front=front;
        update();
    }
    void update(){
        lookAt =glm::lookAt(
            pos,
            pos+front,
            up
        ); 
    }
};
Camera c1;
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.01 * delta; 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        c1.pos += cameraSpeed * c1.front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        c1.pos -= cameraSpeed * c1.front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        c1.pos -= glm::normalize(glm::cross(c1.front, c1.up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        c1.pos += glm::normalize(glm::cross(c1.front, c1.up)) * cameraSpeed;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= yoffset;
    fov =max(1.0f,min(fov,45.0f));

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose
    // property to true,
    // closing the application
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key==GLFW_KEY_Q && action == GLFW_PRESS){
        type=!type;
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    static bool firstMouse=true;
    if(firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.01f*delta;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw   += xoffset;
    pitch += yoffset;  
    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    c1.front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    c1.front.y = sin(glm::radians(pitch));
    c1.front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    c1.front = glm::normalize(c1.front);
}

class TextureImage{
    public:
    GLuint id;
    string path;
    int unit;
    TextureImage(string path,int unit=0):path(path),unit(unit){
        glGenTextures(1,&id);
        use();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        int width, height, nrChannels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (!data){
            cerr<<"unable to load image"<<endl;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    void use(){
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(GL_TEXTURE_2D,id);
    }
};

//cube
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

// GLuint indices[] = { 
// 0,1,7,
// 0,6,7,
// 0,6,4,
// 0,4,2,
// 6,7,5,
// 6,5,4,
// 4,5,3,
// 4,3,2,
// 0,3,2,
// 0,3,1,
// 1,7,5,
// 1,5,3,
// };
GLFWwindow* window;
glm::vec3 cubePositions[] = {
    glm::vec3(2.0f,2.0f,2.0f),
    glm::vec3(2.0f,2.0f,-2.0f),
    glm::vec3(-2.0f,2.0f,2.0f),
    glm::vec3(-2.0f,2.0f,-2.0f),
    glm::vec3(-2.0f,-2.0f,2.0f),
    glm::vec3(-2.0f,-2.0f,-2.0f),
    glm::vec3(2.0f,-2.0f,2.0f),
    glm::vec3(2.0f,-2.0f,-2.0f),
};

glm::vec3 cubeAngles[] ={
    glm::vec3(1.0f,1.0f,0.0f),
    glm::vec3(1.0f,1.0f,1.0f),
    glm::vec3(0.0f,1.0f,1.0f),
    glm::vec3(1.0f,0.0f,1.0f),
    glm::vec3(0.0f,1.0f,0.0f),
    glm::vec3(1.0f,0.0f,1.0f),
    glm::vec3(0.0f,1.0f,0.0f),
    glm::vec3(1.0f,0.0f,1.0f),
};

void saveScreenshotToFile(std::string filename, int windowWidth, int windowHeight) {    
    const int numberOfPixels = windowWidth * windowHeight * 3;
    unsigned char pixels[numberOfPixels];

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, windowWidth, windowHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);

    FILE *outputFile = fopen(filename.c_str(), "w");
    short header[] = {0, 2, 0, 0, 0, 0, (short) windowWidth, (short) windowHeight, 24};

    fwrite(&header, sizeof(header), 1, outputFile);
    fwrite(pixels, numberOfPixels, 1, outputFile);
    fclose(outputFile);
}


int main()
{
    c1.update(
    glm::vec3(0.0f,0.0f,3.0f),
    glm::vec3(0.0f,0.0f,-1.0f));

    stbi_set_flip_vertically_on_load(true); 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    window = glfwCreateWindow(800, 600, "Camera", nullptr,nullptr);
    if (window == nullptr)
    {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);  
    glfwSetKeyCallback(window,key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback); 
    glfwSetScrollCallback(window,scroll_callback); 
    ShaderProgram program1;
    program1.compile(
        read_file("shaders/vertex.vs"),
        read_file("shaders/fragment.fs"));
    program1.use();
    GLuint VBO;
    glGenBuffers(1, &VBO);
    GLuint EBO;
    glGenBuffers(1, &EBO);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(float)));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    TextureImage t1("container.jpg",0);
    program1.set_int("texture2",1);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//GL_FILL
    GLint vertexColorLocation = program1.get_uniform_location("ourColor");
    glm::mat4 trans;
    auto t = chrono::system_clock::now();
    glm::vec3 rand_move = glm::vec3(0.0f,0.0f,0.f);
    unsigned time_frame = 100;
    auto last_save = chrono::system_clock::now();
    int cnt=0;
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        auto t2 = chrono::system_clock::now();
        delta = chrono::duration_cast<chrono::milliseconds>(t2-t).count();
        t=t2;
        if (chrono::duration_cast<chrono::milliseconds>(t2-last_save).count()>time_frame){
            string filename = "saves/im"+to_string(cnt)+".tga";
            saveScreenshotToFile(filename,800,600);
            cnt+=1;
            last_save = t2;
        }
        // cerr<<1000/(el+0.0001f)<<endl;
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        c1.update();
        cerr<<c1.pos.x<<" "<<c1.pos.y<<" "<<c1.pos.z<<endl;
        program1.set_mat4("view",c1.lookAt);
        for (int i=0;i<8;i++){
            glm::mat4 model,view,project;
            model = glm::translate(glm::mat4(1.0f),cubePositions[i]);
            model  = glm::rotate(model, (float)glfwGetTime()*5, cubeAngles[i]);
            const float radius = 10.0f;
            float camX = sin(glfwGetTime()) * radius;
            float camZ = cos(glfwGetTime()) * radius;
            if (type==0)
                project = glm::perspective(glm::radians(fov),800/600.0f,0.1f,100.0f);
            else
                project  =glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -50.0f, 50.0f);// glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, -100.0f, 100.0f);
            program1.set_mat4("model",model);
            program1.set_mat4("project",project);
            glDrawArrays(GL_TRIANGLES,0,64);
        }
        // glDrawElements(GL_TRIANGLES, 65, GL_UNSIGNED_INT, 0);
        

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}