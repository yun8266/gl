#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
unsigned delta=0.001;
float angle=0.0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

string read_file(string path){
    FILE *f =fopen(path.c_str(),"r");
    size_t s;
    fseek(f,0,SEEK_END);
    s = ftell(f);
    fseek(f,0,SEEK_SET);
    char buf[s+1];
    fread(buf,1,s,f);
    fclose(f);
    buf[s]='\0';
    return string(buf);
}
int check_compile_status(int id){
    GLint success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout  <<infoLog << std::endl;
    }
    return success;
}

int check_link_status(int id){
    GLint success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout  <<infoLog << std::endl;
    }
    return success;
}

class ShaderProgram {
    public:
    int id;
    ShaderProgram(){
        id = glCreateProgram();
    }
    void compile(string vertex_shader,string fragment_shader){
        const char *v=vertex_shader.c_str(),*f=fragment_shader.c_str();
        int v_id = glCreateShader(GL_VERTEX_SHADER);
        int f_id =glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(v_id, 1, &v, NULL);
        glCompileShader(v_id);
        check_compile_status(v_id);
        glShaderSource(f_id, 1, &f, NULL);
        glCompileShader(f_id);
        check_compile_status(f_id);
        glAttachShader(id, v_id);
        glAttachShader(id, f_id);
        glLinkProgram(id);
        check_link_status(id);
        glDeleteShader(v_id);
        glDeleteShader(f_id);
    }
    void use(){
        glUseProgram(id);
    }
    void set_int(string name,int val){
        int loc = get_uniform_location(name);
        glUniform1i(loc, val);
    }
    int get_uniform_location(string name){
        GLint loc = glGetUniformLocation(id, name.c_str());
        return loc;
    }
    void set_mat4(string name,const glm::mat4 &mat){
        int loc = get_uniform_location(name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
};

class Texture{
    public:
    GLuint id;
    string path;
    int unit;
    Texture(string path,int unit=0):path(path),unit(unit){
        glGenTextures(1,&id);
        use();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

GLuint indices[] = { // Note that we start from 0!
0, 1, 3, // First Triangle
1, 2, 3  // Second Triangle
};
GLFWwindow* window;
void scroll_callback(GLFWwindow *window,double xoffset,double yoffset){
    float sens = 0.05f*delta;
    int off = sens*yoffset;
    angle+=off;
    angle = max(-90.0f,min(angle,90.0f));
}

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
    stbi_set_flip_vertically_on_load(true); 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(800, 600, "Rotating Rectangle", nullptr,nullptr);
    if (window == nullptr)
    {
        cout << "error glfw" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cout << "error glew" <<endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetKeyCallback(window,key_callback);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    Texture t1("container.jpg",0);
    Texture t2("awesomeface.jpg",1);
    program1.set_int("texture1",0);
    program1.set_int("texture2",1);

    GLint vertexColorLocation = program1.get_uniform_location("ourColor");
    glm::mat4 trans;
    auto t = chrono::system_clock::now();
    unsigned time_frame = 100;
    auto last_save = chrono::system_clock::now();
    int cnt=0;
    while(!glfwWindowShouldClose(window))
    {
        auto t2 = chrono::system_clock::now();
        delta= 1000/(chrono::duration_cast<chrono::milliseconds>(t2-t).count()+0.0001);
        // if (chrono::duration_cast<chrono::milliseconds>(t2-last_save).count()>time_frame){
        //     string filename = "saves/im"+to_string(cnt)+".tga";
        //     saveScreenshotToFile(filename,800,600);
        //     cnt+=1;
        //     last_save = t2;
        // }

        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);

        glm::mat4 model,view,project;
        model = trans = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(glm::mat4(1.0f),glm::vec3(0,0,-3.0));
        project = glm::perspective(glm::radians(45.0f),800/600.0f,0.1f,100.0f);
        program1.set_mat4("model",model);
        program1.set_mat4("view",view);
        program1.set_mat4("project",project);
        glUniformMatrix4fv(program1.get_uniform_location("t"), 1, GL_FALSE, glm::value_ptr(trans));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}