#version 460 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (std140, binding = 0) uniform Camera
{
  mat4 Proj;
  mat4 View;
};
uniform mat4 Model; 
out vec2 TexCoord;
void main()
{
   TexCoord = TexCoords;
   vec4 pos = Proj * mat4(mat3(View)) * Model * vec4(Position, 1.0);
   gl_Position = pos.xyww;
} 