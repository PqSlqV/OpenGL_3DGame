#version 460 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;
layout (location = 5) in uvec4 ID_Bone;
layout (location = 6) in vec4 Weight;

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
uniform mat4 Animation[20];

out VS {
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentFragPos;
} vs_out;

void main()
{
    vs_out.TexCoords = TexCoords;
    mat4 AnimPos = Weight[0] * Animation[ID_Bone[0]] + Weight[1] * Animation[ID_Bone[1]] + Weight[2] * Animation[ID_Bone[2]] +Weight[3] * Animation[ID_Bone[3]];
    mat4 Offside = Model * AnimPos;
    vec3 T = normalize(mat3(Offside) * Tangent);
    vec3 B = normalize(mat3(Offside) * Bitangent);
    vec3 N = normalize(mat3(Offside) * Normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.TangentLightPos = TBN * vec3(Sun_Posit,1);
    vec4 ModPosit = Offside * vec4(Position, 1.0);
    vs_out.TangentFragPos  = TBN * vec3(ModPosit); 
    
    gl_Position = Proj * View * ModPosit;
}