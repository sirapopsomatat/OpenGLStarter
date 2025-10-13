#version 330 core

layout (location = 0) in vec3 pos;

out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightView;
uniform mat4 lightProjection;

void main()
{
    fragPosLightSpace = lightProjection * lightView * model * vec4(pos, 1.0);
    gl_Position = projection * view * model * vec4(pos, 1.0);
    
}