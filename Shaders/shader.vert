#version 330 core

out vec4 colour;
in vec4 vCol;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 lightColour;
//uniform sampler2D texture2;

// vec3 ambientLight()
// {
//     float ambientStrength = 0.5;
//     vec3 ambient = ambientStrength * lightColour;
//     return ambient;
// }

void main()
{
    //phong = ambient + diffuse + specular;
    float ambientStrength = 0.3;
    vec3 ambientLight = ambientStrength * vec3(0.0, 1.0, 1.0);
    colour = texture(texture1, TexCoord) * vec4(ambientLight, 1.0);
    
}