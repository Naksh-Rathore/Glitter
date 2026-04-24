#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 tex;
in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLightSpace;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

uniform bool shouldAttenuate;

uniform sampler2D shadowMap;

float shadow() {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0 ||
       projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    vec3 light = normalize(-light.direction);
    vec3 norm = normalize(normal);

    float bias = max(0.05 * (1.0 - dot(normal, light)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    for(int x = -1; x <= 1; ++x) {

        for(int y = -1; y <= 1; ++y) {

            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;

        }

    }

    return shadow / 9.0;
}

void main() {
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, tex).rgb;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, tex).rgb;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.texture_specular1, tex).rgb;

    vec3 result = ambient + (1.0 - shadow()) * (diffuse + specular);

    FragColor = vec4(result, 1.0f);
}

