#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float ambientStrength; 

uniform sampler2D shadowMap;


float shadow()
{
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0 ||
       projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    vec3 light = normalize(-lightDir);
    vec3 normal = normalize(Normal);

    float bias = 0.0005;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir);

    float diff = max(dot(norm, light), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = ambientStrength * lightColor;

    vec3 result = (ambient + (1.0 - shadow()) * diffuse) * objectColor;

    FragColor = vec4(result, 1.0);
}
