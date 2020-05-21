#ifndef shader_h
#define shader_h
#include "glad.h"
#include "bits/stdc++.h"
using namespace std;
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

#endif