#version 330 core
in vec2 TexCoord;
in vec3 color;
out vec4 FragColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
    FragColor = texture(texture1, TexCoord)*vec4(color,0.0);
}