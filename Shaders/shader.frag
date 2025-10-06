#version 330 core

out vec4 colour;
in vec4 vCol;
in vec2 TexCoord;

uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
    // tex1 = texture(texture1,TexCoord);
    //vec4 tex2 = texture(texture2,TexCoord);
    //colour = vec4(1.0, 0.0, 0.0, 1.0);
    colour = texture(texture1,TexCoord);
 
}