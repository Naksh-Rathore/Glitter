#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 tex;
in vec3 normal;
in vec3 fragPos;
in vec3 tangLightPos;
in vec3 tangFragPos;
in vec3 tangViewPos;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {
    vec3 norm = texture(material.texture_normal1, tex).rgb;

    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);   

    vec3 ambient = light.ambient * texture(material.texture_diffuse1, tex).rgb;

    vec3 lightDir = normalize(tangLightPos - tangFragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, tex).rgb;

    vec3 viewDir = normalize(tangViewPos - tangFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * 0.2;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}

