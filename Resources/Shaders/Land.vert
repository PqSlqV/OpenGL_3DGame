#version 460 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

layout (std140, binding = 0) uniform Camera
{
  mat4 Proj;
  mat4 View;
};
layout (std140, binding = 1) uniform Sun
{
  vec2 Sun_Posit;
};

uniform mat4 Model;

out VS {
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentFragPos;
} vs_out;

void main()
{
    vs_out.TexCoords = TexCoords;   
    
    vec3 T = normalize(mat3(Model) * Tangent);
    vec3 B = normalize(mat3(Model) * Bitangent);
    vec3 N = normalize(mat3(Model) * Normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentLightPos = TBN * vec3(Sun_Posit,1);
    vec4 Posit = Model * vec4(Position, 1.0);
    vs_out.TangentFragPos  = TBN * vec3(Posit); 
    
    gl_Position = Proj * View * Posit;
}