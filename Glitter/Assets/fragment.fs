#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Match exactly the vertex shader outputs
in vec2 tex;
in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // ===== Ambient =====
    vec3 ambient = light.ambient *
                   texture(material.texture_diffuse1, tex).rgb;

    // ===== Diffuse =====
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff *
                   texture(material.texture_diffuse1, tex).rgb;

    // ===== Specular =====
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),
                     material.shininess);
    vec3 specular = light.specular * spec *
                    texture(material.texture_specular1, tex).rgb;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}

