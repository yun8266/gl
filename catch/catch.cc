#define STB_IMAGE_IMPLEMENTATION
#include "glad.h"
#include "bits/stdc++.h"
#include "shader.h"
#include "loader.h"
#include "text.h"
using namespace std;
unsigned delta;
float lastX = 400, lastY = 300;
float yaw=0,pitch=0;
float fov=45.0f;
FT_Library ft;
FT_Face face;

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

    float cameraSpeed = 0.03 * delta; 
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
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
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

GLFWwindow* window;
int max_balls = 200;
int curr_balls = 0;
int score=0;
glm::vec3 bound_max = glm::vec3(100.0f,100.0f,100.0f);
glm::vec3 bound_min = glm::vec3(100.0f,100.0f,-100.0f);
class Ball {
    public:
    glm::vec3 start;
    glm::vec3 dest;
    glm::vec3 curr;
    glm::vec3 dir;
    glm::vec3 color;
    glm::vec3 axis;
    float angle =0.0f;
    bool completed=false;
    Ball(){
        int t = rand()%4;
        if (t==0){
            start = glm::vec3(rand()%200-100,rand()%200-100,100);
            dest = glm::vec3(rand()%200-100,rand()%200-100,-100);
            dir = glm::normalize(dest-start);

        }
        else if (t==1) {
            start = glm::vec3(rand()%200-100,rand()%200-100,-100);
            dest = glm::vec3(rand()%200-100,rand()%200-100,100);
            dir = glm::normalize(dest-start);
        }
        else if (t==2){
            start = glm::vec3(rand()%200-100,-100 ,rand()%200-100);
            dest = glm::vec3(rand()%200-100,100,rand()%200-100);
            dir = glm::normalize(dest-start);
        }
        else{
            start = glm::vec3(rand()%200-100,100 ,rand()%200-100);
            dest = glm::vec3(rand()%200-100,-100,rand()%200-100);
            dir = glm::normalize(dest-start);
        }
        curr =start;
        curr_balls+=1;
        color = glm::vec3(
            (float)(rand()%100)/100,
            (float)(rand()%100)/100,
            (float)(rand()%100)/100
        );
        axis = glm::vec3(
            (float)(rand()%100+10)/100,
            (float)(rand()%100+10)/100,
            (float)(rand()%100+10)/100
        );
    }
    void update(){
        float speed =0.01f*delta;
        curr+=dir*speed;
    }

    glm::mat4 model(){
        float off = 0.3f*delta;
        angle+=off;
        if (angle>=360){
            angle =0;
        }
        glm::mat4 model = glm::translate(glm::mat4(1.0f),curr);
        model =  glm::rotate(model,glm::radians(angle),axis);
        model = glm::scale(model,glm::vec3(1.5f,1.5f,1.5f));
        // glm::mat4 rot = glm::rotate(glm::mat4(1.0f),glm::radians(angle),axis);
        // return glm::translate(rot,curr);
        // return glm::translate(glm::mat4(1.0f),curr);
        // return glm::rotate(glm::mat4(1.0f),angle,axis);
        return model;
    }

    int check_completed(){
        float val  = glm::dot(start-curr,dest-curr);
        if (val>0){
            return 1;
        }
        else if (glm::distance(c1.pos,curr)<8){
            return 2;
        }
        else {
            return 0;
        }
    }
};
void RenderText(ShaderProgram &shader, std::string text, float x, float y, float scale, glm::vec3 color);

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
    
    window = glfwCreateWindow(800, 600, "Catch!", nullptr,nullptr);
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    ShaderProgram program1;
    program1.compile(
        read_file("shaders/vertex.vs"),
        read_file("shaders/fragment.fs"));
    program1.use();
    
    Model ourModel("star.obj");
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//GL_FILL
    glm::mat4 trans;
    auto t = chrono::system_clock::now();
    glm::mat4 projection = glm::perspective(glm::radians(50.0f), (float)800 / (float)600, 0.1f, 300.0f);
    program1.set_mat4("projection", projection);
    vector<Ball> balls;
    int color_loc = program1.get_uniform_location("color");
    ShaderProgram program2;
    program2.compile(
        read_file("shaders/text/vertex.vs"),
        read_file("shaders/text/fragment.fs")
    );
    TextRender tr(program2);
    unsigned time_frame = 100;
    auto last_save = chrono::system_clock::now();
    int cnt=0;
    while(!glfwWindowShouldClose(window))
    {
        if (curr_balls<max_balls){
            balls.push_back(Ball());
        }
        glfwPollEvents();
        processInput(window);
        c1.update();
        auto t2 = chrono::system_clock::now();
        delta = chrono::duration_cast<chrono::milliseconds>(t2-t).count();
        t=t2;

        // if (chrono::duration_cast<chrono::milliseconds>(t2-last_save).count()>time_frame){
        //     string filename = "saves/im"+to_string(cnt)+".tga";
        //     saveScreenshotToFile(filename,800,600);
        //     cnt+=1;
        //     last_save = t2;
        // }
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        program1.use();
        glm::mat4 view = c1.lookAt;
        
        program1.set_mat4("view", view);
        for (auto it = balls.begin();it!=balls.end();){
            int completed = it->check_completed();
            if (completed==2){
                score+=1;
                it = balls.erase(it);
                curr_balls-=1;
            }
            else if (completed==1){
                it = balls.erase(it);
                curr_balls-=1;
            }
            else {
                it->update();
                glm::mat4 model = it->model();
                // model = glm::scale(model,glm::vec3(2.0,2.0,2.0));
                program1.set_mat4("model", model);
                // cerr<<color_loc<<it->color.x<<" "<<it->color.y<<" "<<it->color.z<<endl;
                glUniform3f(color_loc,it->color.x,it->color.y,it->color.z);
                ourModel.Draw(program1);
                ++it;
            }
        }
        string show = "Score: "+to_string(score);
        tr.RenderText(show,300,530,1,glm::vec3(1.0f,1.0f,0.0f));
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}