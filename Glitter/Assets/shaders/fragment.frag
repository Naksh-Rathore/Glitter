#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec2 tex;
    vec3 tangLightPos;
    vec3 tangViewPos;
    vec3 tangFragPos;
} fs_in;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

uniform float heightScale;

vec2 parallaxMapping(vec2 tex, vec3 viewDir) {
    const float numLayers = 10;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    vec2 P = viewDir.xy * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
    
    vec2 currentTexCoords = tex;
    float currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;  
        currentLayerDepth += layerDepth;
    }

    return currentTexCoords;
}

void main() {
    vec3 viewDir = normalize(fs_in.tangViewPos - fs_in.tangFragPos);
    vec2 tex = parallaxMapping(fs_in.tex,  viewDir);

    if(tex.x > 1.0 || tex.y > 1.0 || tex.x < 0.0 || tex.y < 0.0)
        discard;


    vec3 norm = texture(material.texture_normal1, tex).rgb;
    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0); 

    vec3 ambient = light.ambient * texture(material.texture_diffuse1, tex).rgb;

    vec3 lightDir = normalize(fs_in.tangLightPos - fs_in.tangFragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, tex).rgb;

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * 0.2;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}

