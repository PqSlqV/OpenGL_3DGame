#version 460 core
out vec4 FragColor;

uniform sampler2DArray Texture;

in VS {
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentFragPos;
} fs_in;

void main()
{  
    //Диффузная карта        
    vec3 color = texture(Texture, vec3(fs_in.TexCoords,0)).rgb;
    //Карты нормалей
    vec3 normal = texture(Texture, vec3(fs_in.TexCoords,1)).rgb;
    normal = normalize(normal * 2.0 - 1.0);  
    //Фоновая составляющая
    vec3 ambient = 0.1 * color;
    //Диффузная составляющая
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    FragColor = vec4(ambient + diffuse, 1.0);
}