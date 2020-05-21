#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color_in;
layout (location = 2) in vec2 aTexCoord;
out vec2 TexCoord;
out vec3 color;
uniform mat4 model;
uniform mat4 project;
uniform mat4 view;
void main()
{
    gl_Position = project*view*model*vec4(position.x, position.y, position.z, 1.0);
    TexCoord = aTexCoord;
    color =color_in;
}