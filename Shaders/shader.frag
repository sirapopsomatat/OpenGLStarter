#version 330 core

out vec4 colour;
in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 ambientLight()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColour;
    return ambient;
}

vec3 diffuseLight()
{
    float diffuseStrength = 0.5;
    //diffuse light = normal dot light direction
    vec3 norm = normalize(Normal);
    //light direction = light position - fragment position
    //fragment position = position of the fragment in world space
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColour;
    return diffuse;
}   

vec3 specularLight()
{
    float specularStrength = 0.8;
    float shininess = 1024.0;
    //specular light = view direction dot reflect direction
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(viewPos - FragPos);

    //Phonr specular
    //float spec = pow(max(dot9(viewDir, reflectDir), 0.0), shininess);

    //Bling-Phong specular
    vec3 halfDir = normalize((lightDir + viewDir)/2);
    float spec = pow(max(dot(norm, halfDir), 0.0), shininess);

    vec3 specular = specularStrength * spec * lightColour;
    return specular;
}

void main()
{
    //phong = ambient + diffuse + specular;
    colour = texture(texture1, TexCoord) * vec4(ambientLight() + diffuseLight() + specularLight(), 1.0);
    //colour = texture(texture1, TexCoord) * vec4(ambientLight() + diffuseLight(), 1.0);
    //colour = texture(texture1, TexCoord) * vec4(specularLight(), 1.0);
}