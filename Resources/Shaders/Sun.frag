#version 460 core
out vec4 FragColor;

uniform sampler2D Texture;

in vec2 TexCoord;
void main()
{   
    vec4 color = texture(Texture, TexCoord);
    FragColor = color;
}