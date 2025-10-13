#version 330 core

out vec4 colour;

uniform vec3 bgColour;
in vec4 fragPosLightSpace;

uniform sampler2D shadowMap;

float ShadowCalculation()
{
    float shadow = 0.0;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float bias = 0.005;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // clamp projCoords to avoid sampling outside the shadow map
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
            {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 0.7 : 0.0;
            }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    float shadow = ShadowCalculation();
    colour = vec4((1.0 - shadow) * bgColour, 1.0);
}